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

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "distributed_input_collector.h"
#include "distributed_input_inject.h"
#include "distributed_input_sink_transport.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DInputState);
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
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    dhIdStateMap_.clear();
    return DH_SUCCESS;
}

int32_t DInputState::RecordDhIds(const std::vector<std::string> &dhIds, DhIdState state, const int32_t sessionId)
{
    DHLOGI("RecordDhIds dhIds size = %zu", dhIds.size());
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    for (const auto &dhid : dhIds) {
        DHLOGD("add dhid : %s, state : %d.", GetAnonyString(dhid).c_str(), state);
        dhIdStateMap_[dhid] = state;
    }

    if (state == DhIdState::THROUGH_OUT) {
        CreateSpecialEventInjectThread(sessionId, dhIds);
    }
    return DH_SUCCESS;
}

int32_t DInputState::RemoveDhIds(const std::vector<std::string> &dhIds)
{
    DHLOGI("RemoveDhIds dhIds size = %zu", dhIds.size());
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    for (const auto &dhid : dhIds) {
        DHLOGD("delete dhid : %s", GetAnonyString(dhid).c_str());
        dhIdStateMap_.erase(dhid);
    }
    return DH_SUCCESS;
}

DhIdState DInputState::GetStateByDhid(const std::string &dhId)
{
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    if (dhIdStateMap_.find(dhId) == dhIdStateMap_.end()) {
        DHLOGE("dhId : %s not exist.", GetAnonyString(dhId).c_str());
        return DhIdState::THROUGH_IN;
    }
    return dhIdStateMap_[dhId];
}

void DInputState::CreateSpecialEventInjectThread(const int32_t sessionId, const std::vector<std::string> &dhIds)
{
    DHLOGI("CreateSpecialEventInjectThread enter, dhIds.size = %d, sessionId = %d.", dhIds.size(), sessionId);
    std::thread specEventInjectThread =
        std::thread(&DInputState::SpecEventInject, this, sessionId, dhIds);
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

void DInputState::WriteEventToDev(const int fd, const input_event &event)
{
    if (write(fd, &event, sizeof(event)) < static_cast<ssize_t>(sizeof(event))) {
        DHLOGE("could not inject event, removed? (fd: %d)", fd);
        return;
    }
    RecordEventLog(event);
}

void DInputState::SpecEventInject(const int32_t sessionId, const std::vector<std::string> &dhIds)
{
    DHLOGI("SpecEveInject enter, sessionId %d, dhIds size %d", sessionId, dhIds.size());
    // mouse event send to remote device
    if (sessionId != -1) {
        std::string mouseNodePath;
        std::string mouseNodeDhId;
        DistributedInputCollector::GetInstance().GetMouseNodePath(dhIds, mouseNodePath, mouseNodeDhId);
        SyncMouseKeyState(sessionId, mouseNodePath, mouseNodeDhId);
    }

    // keyboard up event inject local device
    std::vector<std::string> keyboardNodePaths;
    std::vector<std::string> keyboardNodeDhIds;
    DistributedInputCollector::GetInstance().GetSharedKeyboardPathsByDhIds(dhIds, keyboardNodePaths, keyboardNodeDhIds);
    DistributedInputInject::GetInstance().GetVirtualKeyboardPathsByDhIds(dhIds, keyboardNodePaths, keyboardNodeDhIds);
    size_t len = keyboardNodePaths.size();
    for (size_t i = 0; i < len; ++i) {
        std::vector<uint32_t> pressedKeys;
        int fd = UN_INIT_FD_VALUE;
        CheckKeyboardState(keyboardNodeDhIds[i], keyboardNodePaths[i], pressedKeys, fd);
        struct input_event event = {
            .type = EV_KEY,
            .code = 0,
            .value = KEY_UP_STATE
        };
        for (auto &code : pressedKeys) {
            event.type = EV_KEY;
            event.code = code;
            WriteEventToDev(fd, event);
            event.type = EV_SYN;
            event.code = 0;
            WriteEventToDev(fd, event);
        }
        CloseFd(fd);
    }
}

void DInputState::CheckKeyboardState(const std::string &dhId, const std::string &keyboardNodePath,
    std::vector<uint32_t> &pressedKeys, int &fd)
{
    DHLOGI("CheckKeyboardState enter,  dhId %s, keyboardNodePath %s.", GetAnonyString(dhId).c_str(),
        keyboardNodePath.c_str());
    char canonicalPath[PATH_MAX] = {0x00};
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
    unsigned long keyState[NLONGS(KEY_CNT)] = { 0 };
    while (true) {
        if (count > READ_RETRY_MAX) {
            break;
        }
        int rc = ioctl(fd, EVIOCGKEY(sizeof(keyState)), keyState);
        if (rc < 0) {
            DHLOGE("read all key state failed, rc=%d ", rc);
            count += 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(READ_SLEEP_TIME_MS));
            continue;
        }
        for (int32_t keyIndex = 0; keyIndex < KEY_MAX; keyIndex++) {
            if (BitIsSet(keyState, keyIndex)) {
                DHLOGD("keyIndex = %d, not up.", keyIndex);
                pressedKeys.push_back(keyIndex);
            }
        }
        break;
    }
}

void DInputState::SyncMouseKeyState(const int32_t sessionId, const std::string &mouseNodePath,
    const std::string &mouseNodeDhId)
{
    DHLOGI("SyncMouseKeyState enter, mouseNodePath %s, mouseNodeDhId %s, sessionId %d.", mouseNodePath.c_str(),
        GetAnonyString(mouseNodeDhId).c_str(), sessionId);
    char canonicalPath[PATH_MAX] = {0x00};
    if (mouseNodePath.length() == 0 || mouseNodePath.length() >= PATH_MAX ||
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
    unsigned long keyState[NLONGS(KEY_CNT)] = { 0 };
    while (true) {
        if (count > READ_RETRY_MAX) {
            break;
        }
        // Query all key state
        int rc = ioctl(fd, EVIOCGKEY(sizeof(keyState)), keyState);
        if (rc < 0) {
            DHLOGE("read all key state failed, rc=%d ", rc);
            count += 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(READ_SLEEP_TIME_MS));
            continue;
        }
        leftKeyVal = BitIsSet(keyState, BTN_LEFT);
        if (leftKeyVal != 0) {
            DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, mouseNodeDhId, BTN_LEFT);
        }
        rightKeyVal = BitIsSet(keyState, BTN_RIGHT);
        if (rightKeyVal != 0) {
            DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, mouseNodeDhId, BTN_RIGHT);
        }
        midKeyVal = BitIsSet(keyState, BTN_MIDDLE);
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