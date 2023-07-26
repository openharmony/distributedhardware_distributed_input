/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dinput_state.h"

#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "constants_dinput.h"
#include "distributed_input_collector.h"
#include "distributed_input_inject.h"
#include "distributed_input_sink_transport.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DInputState::~DInputState()
{
    Release();
}

int32_t DInputState::Init()
{
    DHLOGI("DInputState Init.");
    return DH_SUCCESS;
}

int32_t DInputState::Release()
{
    DHLOGI("DInputState Release.");
    std::unique_lock<std::mutex> mapLock(operationMutex_);
    dhidStateMap_.clear();
    return DH_SUCCESS;
}

int32_t DInputState::RecordDhids(const std::vector<std::string> &dhids, DhidState state, const int32_t &sessionId)
{
    DHLOGI("RecordDhids dhids size = %zu", dhids.size());
    std::unique_lock<std::mutex> mapLock(operationMutex_);
    for (auto &dhid : dhids) {
        DHLOGD("add dhid : %s, state : %d.", GetAnonyString(dhid).c_str(), state);
        dhidStateMap_[dhid] = state;
    }

    if (state == DhidState::THROUGH_OUT) {
        CreateSpecialEventInjectThread(sessionId, dhids);
    }
    return DH_SUCCESS;
}

int32_t DInputState::RemoveDhids(const std::vector<std::string> &dhids)
{
    DHLOGI("RemoveDhids dhids size = %zu", dhids.size());
    std::unique_lock<std::mutex> mapLock(operationMutex_);
    for (auto &dhid : dhids) {
        DHLOGD("delete dhid : %s", GetAnonyString(dhid).c_str());
        dhidStateMap_.erase(dhid);
    }
    return DH_SUCCESS;
}

DhidState DInputState::GetStateByDhid(std::string &dhid)
{
    std::unique_lock<std::mutex> mapLock(operationMutex_);
    if (dhidStateMap_.find(dhid) == dhidStateMap_.end()) {
        DHLOGE("dhid : %s not exist.", GetAnonyString(dhid).c_str());
        return DhidState::THROUGH_IN;
    }
    return dhidStateMap_[dhid];
}

void DInputState::CreateSpecialEventInjectThread(const int32_t &sessionId, const std::vector<std::string> &dhids)
{
    DHLOGI("CreateSpecialEventInjectThread enter, dhids.size = %d, sessionId = %d.", dhids.size(), sessionId);
    std::thread specEventInjectThread =
        std::thread(&DInputState::SpecEventInject, this, sessionId, dhids);
    int32_t ret = pthread_setname_np(specEventInjectThread.native_handle(), CHECK_KEY_STATUS_THREAD_NAME);
    if (ret != 0) {
        DHLOGE("specEventInjectThread setname failed.");
    }
    specEventInjectThread.detach();
}

void DInputState::RecordEventLog(const input_event &event)
{
    std::string eventType = "";
    switch (event.type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_SYN:
            eventType = "EV_SYN";
            break;
        default:
            eventType = "other type";
            break;
    }
    DHLOGD("5.E2E-Test Source write event into input driver, EventType: %s, Code: %d, Value: %d",
        eventType.c_str(), event.code, event.value);
}

void DInputState::WriteEventToDev(int &fd, const input_event &event)
{
    if (write(fd, &event, sizeof(event)) < static_cast<ssize_t>(sizeof(event))) {
        DHLOGE("could not inject event, removed? (fd: %d)", fd);
        return;
    }
    RecordEventLog(event);
}

void DInputState::SpecEventInject(const int32_t &sessionId, std::vector<std::string> dhids)
{
    DHLOGI("SpecEveInject enter");
    // mouse event send to remote device
    if (sessionId != -1) {
        std::string mouseNodePath;
        std::string mouseNodeDhId;
        DistributedInputCollector::GetInstance().GetMouseNodePath(dhids, mouseNodePath, mouseNodeDhId);
        CheckMouseKeyState(sessionId, mouseNodePath, mouseNodeDhId);
    }

    // keyboard up event inject local device
    std::vector<std::string> keyboardNodePaths;
    std::vector<std::string> keyboardNodeDhIds;
    DistributedInputCollector::GetInstance().GetShareKeyboardPathsByDhIds(dhids, keyboardNodePaths, keyboardNodeDhIds);
    DistributedInputInject::GetInstance().GetVirtualKeyboardPathsByDhIds(dhids, keyboardNodePaths, keyboardNodeDhIds);
    ssize_t len = keyboardNodePaths.size();
    for (ssize_t i = 0; i < len; ++i) {
        std::vector<uint32_t> keyboardPressedKeys;
        int fd = -1;
        CheckKeyboardState(keyboardNodeDhIds[i], keyboardNodePaths[i], keyboardPressedKeys, fd);
        for (auto &code : keyboardPressedKeys) {
            struct input_event event = {
                .type = EV_KEY,
                .code = code,
                .value = KEY_UP_STATE
            };
            WriteEventToDev(fd, event);
            event.type = EV_SYN;
            event.code = 0;
            WriteEventToDev(fd, event);
        }
        CloseFd(fd);
    }
}

void DInputState::CheckKeyboardState(std::string &dhid, std::string &keyboardNodePath,
    std::vector<uint32_t> &keyboardPressedKeys, int &fd)
{
    DHLOGI("CheckKeyboardState enter,  dhid %s, keyboardNodePath %s.", GetAnonyString(dhid).c_str(),
        keyboardNodePath.c_str());
    char* canonicalPath = new char(PATH_MAX);
    if (keyboardNodePath.length() == 0 || keyboardNodePath.length() >= PATH_MAX ||
        realpath(keyboardNodePath.c_str(), canonicalPath) == nullptr) {
        DHLOGE("keyboard Nodepath check fail, error path: %s", keyboardNodePath.c_str());
        return;
    }
    fd = open(canonicalPath, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        DHLOGE("open keyboard Node Path error:", errno);
        return;
    }

    uint32_t count = 0;
    unsigned long keystate[NLONGS(KEY_CNT)] = { 0 };
    while (true) {
        if (count > READ_RETRY_MAX) {
            break;
        }
        int rc = ioctl(fd, EVIOCGKEY(sizeof(keystate)), keystate);
        if (rc < 0) {
            DHLOGE("read all key state failed, rc=%d ", rc);
            count += 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(READ_SLEEP_TIME_MS));
            continue;
        }
        for (int32_t yalv = 0; yalv < KEY_MAX; yalv++) {
            if (BitIsSet(keystate, yalv)) {
                DHLOGD("yalv = %d, not up.", yalv);
                keyboardPressedKeys.push_back(yalv);
            }
        }
        break;
    }
}

void DInputState::CheckMouseKeyState(const int32_t &sessionId, const std::string &mouseNodePath,
    const std::string &mouseNodeDhId)
{
    DHLOGI("CheckMouseKeyState enter, mouseNodePath %s, mouseNodeDhId %s, sessionId %d.", mouseNodePath.c_str(),
        GetAnonyString(mouseNodeDhId).c_str(), sessionId);
    char* canonicalPath = new char(PATH_MAX);
    if (mouseNodePath.length() == 0 || mouseNodePath.length() > PATH_MAX ||
        realpath(mouseNodePath.c_str(), canonicalPath) == nullptr) {
        DHLOGE("mouse Nodepath check fail, error path: %s", mouseNodePath.c_str());
        return;
    }
    int fd = open(canonicalPath, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        DHLOGE("open mouse Node Path error:", errno);
        return;
    }

    uint32_t count = 0;
    int leftKeyVal = 0;
    int rightKeyVal = 0;
    int midKeyVal = 0;
    unsigned long keystate[NLONGS(KEY_CNT)] = { 0 };
    while (true) {
        if (count > READ_RETRY_MAX) {
            break;
        }
        // Query all key state
        int rc = ioctl(fd, EVIOCGKEY(sizeof(keystate)), keystate);
        if (rc < 0) {
            DHLOGE("read all key state failed, rc=%d ", rc);
            count += 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(READ_SLEEP_TIME_MS));
            continue;
        }
        leftKeyVal = BitIsSet(keystate, BTN_LEFT);
        if (leftKeyVal != 0) {
            DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, mouseNodeDhId, BTN_LEFT);
        }
        rightKeyVal = BitIsSet(keystate, BTN_RIGHT);
        if (rightKeyVal != 0) {
            DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, mouseNodeDhId, BTN_RIGHT);
        }
        midKeyVal = BitIsSet(keystate, BTN_MIDDLE);
        if (midKeyVal != 0) {
            DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, mouseNodeDhId, BTN_MIDDLE);
        }
        break;
    }
    CloseFd(fd);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOSs