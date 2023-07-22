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

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "constants_dinput.h"
#include "distributed_input_collector.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {

StateMachine::~StateMachine()
{
    Release();
}

int32_t StateMachine::Init()
{
    DHLOGI("StateMachine Init.");
    return DH_SUCCESS;
}

int32_t StateMachine::Release()
{
    DHLOGI("StateMachine Release.");
    dhidStateMap_.clear();
    return DH_SUCCESS;
}

int32_t StateMachine::AddDhids(std::vector<std::string> &dhids)
{
    DHLOGI("AddDhid dhids size = %zu", dhids.size());
    for (auto &dhid : dhids) {
        DHLOGD("add dhid : %s", GetAnonyString(dhid).c_str());
        if (IsExistDhid(dhid)) {
            DHLOGI("dhid : %s already exist.", GetAnonyString(dhid).c_str());
        } else {
            dhidStateMap_[dhid] = DhidState::INIT;
        }
    }
    return DH_SUCCESS;
}

int32_t StateMachine::DeleteDhids(std::vector<std::string> &dhids)
{
    DHLOGI("DeleteDhid dhids size = %zu", dhids.size());
    for (auto &dhid : dhids) {
        DHLOGD("delete dhid : %s", GetAnonyString(dhid).c_str());
        if (!IsExistDhid(dhid)) {
            DHLOGE("dhid : %s not exist.", GetAnonyString(dhid).c_str());
        } else {
            dhidStateMap_.erase(dhid);
        }
    }
    return DH_SUCCESS;
}

int32_t StateMachine::SwitchState(std::vector<std::string> &dhids, DhidState state)
{
    for (auto &dhid : dhids) {
        DHLOGD("SwitchState dhid : %s, state : %d.", GetAnonyString(dhid).c_str(), state);
        if (!IsExistDhid(dhid)) {
            DHLOGE("dhid : %s not exist.", GetAnonyString(dhid).c_str());
        } else {
            dhidStateMap_[dhid] = state;
        }
    }

    if (state == DhidState::THROUGH_OUT) {
        CreateKeyUpInjectThread(dhids);
    }

    return DH_SUCCESS;
}

DhidState StateMachine::GetStateByDhid(std::string &dhid)
{
    if (!IsExistDhid(dhid)) {
        DHLOGE("dhid : %s not exist.", GetAnonyString(dhid).c_str());
        return DhidState::INIT;
    }
    return dhidStateMap_[dhid];
}

bool StateMachine::IsExistDhid(std::string &dhid)
{
    if (dhidStateMap_.find(dhid) == dhidStateMap_.end()) {
        return false;
    }
    return true;
}

void StateMachine::CreateKeyUpInjectThread(const std::vector<std::string> &dhids)
{
    std::vector<std::string> keyboardNodePaths;
    std::vector<std::string> shareDhIds;
    DistributedInputCollector::GetInstance().GetKeyboardNodePath(dhids, keyboardNodePaths, shareDhIds);
    
    DistributedInputInject::GetInstance().GetVirtualKeyboardPathByDhId(dhids, keyboardNodePaths, shareDhIds);

    std::thread keyUpInjectThread =
        std::thread(&StateMachine::KeyUpInject, this, std::ref(keyboardNodePaths), std::ref(shareDhIds));
    int32_t ret = pthread_setname_np(keyUpInjectThread.native_handle(), KEYBOARD_UP_INJECT_THREAD_NAME);
    if (ret != 0) {
        DHLOGE("CreateKeyUpInjectThread setname failed.");
    }
    keyUpInjectThread.detach();
}

void StateMachine::KeyUpInject(std::vector<std::string> &shareDhidsPaths, std::vector<std::string> &shareDhIds)
{
    ssize len = shareDhidsPaths.size();
    for (int32_t i = 0; i < len; ++i) {
        CheckKeyState(shareDhIds[i], shareDhidsPaths[i]);
    }
}

int BitIsSet(const unsigned long *array, int bit)
{
    return !!(array[bit / LONG_BITS] & (1LL << (bit % LONG_BITS)));
}

void StateMachine::CheckKeyState(std::string &dhid, std::string &keyboardNodePath)
{
    char canonicalPath[PATH_MAX + 1] = {0x00};
    if (keyboardNodePath.length() == 0 || keyboardNodePath.length() > PATH_MAX ||
        realpath(keyboardNodePath.c_str(), canonicalPath) == nullptr) {
        DHLOGE("keyboard Nodepath check fail, error path: %s", keyboardNodePath.c_str());
        return;
    }

    int fd = open(canonicalPath, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        DHLOGE("open keyboard Node Path error:", errno);
        return;
    }

    uint32_t count = 0;
    std::vector<uint32_t> keyboardPressedKeys;
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
        for (int32_t yalv = 0; yalv < KEY_MAX; yalv++) {
            if (BitIsSet(keystate, yalv)) {
                keyboardPressedKeys.push_back(yalv);
            }
        }
        break;
    }
    UpInject(fd, keyboardPressedKeys, dhid);
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

void StateMachine::UpInject(int fd, std::vector<uint32_t> &keyboardPressedKeys, std::string &dhid)
{
    for (auto &code: keyboardPressedKeys) {
        struct input_event event = {
            .type = EV_KEY,
            .code = code,
            .value = KEY_UP_STATE
        };
        if (write(fd, &event, sizeof(event)) < static_cast<ssize_t>(sizeof(event))) {
            DHLOGE("could not inject event, removed? (fd: %d)", fd);
            return;
        }
        RecordEventLog(event);

        struct input_event event = {
            .type = EV_SYN,
            .code = 0,
            .value = KEY_UP_STATE
        };
        if (write(fd, &event, sizeof(event)) < static_cast<ssize_t>(sizeof(event))) {
            DHLOGE("could not inject event, removed? (fd: %d)", fd);
            return;
        }
        RecordEventLog(event);
    }
}

void StateMachine::RecordEventLog(const input_event& event)
{
    std::string eventType = "";
    switch (event.type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_REL:
            eventType = "EV_REL";
            break;
        case EV_ABS:
            eventType = "EV_ABS";
            break;
        default:
            eventType = "other type";
            break;
    }
    DHLOGD("4.E2E-Test Source write event into input driver, EventType: %s, Code: %d, Value: %d, Sec: %ld, Sec1: %ld",
        eventType.c_str(), event.code, event.value, event.input_event_sec, event.input_event_usec);
}

} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOSs