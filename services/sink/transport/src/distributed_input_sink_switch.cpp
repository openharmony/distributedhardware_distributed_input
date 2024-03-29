/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "distributed_input_sink_switch.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputSinkSwitch::DistributedInputSinkSwitch()
{
    InitSwitch();
}

DistributedInputSinkSwitch::~DistributedInputSinkSwitch()
{
    DHLOGI("~DistributedInputSinkSwitch()");
    InitSwitch();
}

DistributedInputSinkSwitch &DistributedInputSinkSwitch::GetInstance()
{
    static DistributedInputSinkSwitch instance;
    return instance;
}

void DistributedInputSinkSwitch::InitSwitch()
{
    DHLOGI("InitSwitch.");
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    switchVector_.clear();
}

int32_t DistributedInputSinkSwitch::StartSwitch(int32_t sessionId)
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    if (switchVector_.empty()) {
        DHLOGE("StartSwitch sessionId: %{public}d fail,switchVector_ is null.", sessionId);
        return ERR_DH_INPUT_SERVER_SINK_START_SWITCH_FAIL;
    } else {
        bool findOld = false;
        for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end(); ++it) {
            if ((*it).sessionId == sessionId) {
                (*it).switchState = true;
                findOld = true;
                break;
            }
        }

        if (findOld) {
            DHLOGI("StartSwitch sessionId: %{public}d is find.", sessionId);
            return DH_SUCCESS;
        } else {
            DHLOGE("StartSwitch sessionId: %{public}d fail, not found.", sessionId);
            return ERR_DH_INPUT_SERVER_SINK_START_SWITCH_FAIL;
        }
    }
}

void DistributedInputSinkSwitch::StopSwitch(int32_t sessionId)
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    if (switchVector_.empty()) {
        DHLOGE("StopSwitch sessionId: %{public}d fail,switchVector_ is null.", sessionId);
    } else {
        bool findOld = false;
        for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end(); ++it) {
            if ((*it).sessionId == sessionId) {
                (*it).switchState = false;
                findOld = true;
                break;
            }
        }

        if (findOld) {
            DHLOGI("StopSwitch sessionId: %{public}d is success.", sessionId);
        } else {
            DHLOGE("StopSwitch sessionId: %{public}d fail,not find it.", sessionId);
        }
    }
}

void DistributedInputSinkSwitch::StopAllSwitch()
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    if (switchVector_.empty()) {
        DHLOGW("StopAllSwitch switchVector_ is null.");
    } else {
        for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end(); ++it) {
            (*it).switchState = false;
        }
        DHLOGI("StopAllSwitch success.");
    }
}

void DistributedInputSinkSwitch::AddSession(int32_t sessionId)
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    if (switchVector_.empty()) {
        SwitchStateData tmp(sessionId, false);
        switchVector_.push_back(tmp);
        DHLOGI("AddSession sessionId: %{public}d add first.", sessionId);
    } else {
        bool findOld = false;
        for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end(); ++it) {
            if ((*it).sessionId == sessionId) {
                findOld = true;
                break;
            }
        }

        if (findOld) {
            DHLOGI("AddSession sessionId: %{public}d is find.", sessionId);
        } else {
            SwitchStateData tmp(sessionId, false);
            switchVector_.push_back(tmp);
            DHLOGI("AddSession sessionId: %{public}d add new.", sessionId);
        }
    }
}

void DistributedInputSinkSwitch::RemoveSession(int32_t sessionId)
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    if (switchVector_.empty()) {
        DHLOGE("RemoveSession sessionId: %{public}d fail,switch_vector is null.", sessionId);
    } else {
        bool findOld = false;
        for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end();) {
            if ((*it).sessionId == sessionId) {
                it = switchVector_.erase(it);
                findOld = true;
                break;
            } else {
                ++it;
            }
        }
        if (findOld) {
            DHLOGI("RemoveSession sessionId: %{public}d is success.", sessionId);
        } else {
            DHLOGE("RemoveSession sessionId: %{public}d fail,not find it.", sessionId);
        }
    }
}

std::vector<int32_t> DistributedInputSinkSwitch::GetAllSessionId()
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    std::vector<int32_t> tmpVecSession;
    for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end(); ++it) {
        tmpVecSession.push_back((*it).sessionId);
    }
    return tmpVecSession;
}

// get current session which state is on, if error return -1.
int32_t DistributedInputSinkSwitch::GetSwitchOpenedSession()
{
    std::unique_lock<std::mutex> switchLock(operationMutex_);
    if (switchVector_.empty()) {
        DHLOGE("GetSwitchOpenedSession error, no data.");
        return ERR_DH_INPUT_SERVER_SINK_GET_OPEN_SESSION_FAIL;
    }
    for (std::vector<SwitchStateData>::iterator it = switchVector_.begin(); it < switchVector_.end(); ++it) {
        if ((*it).switchState == true) {
            return (*it).sessionId;
        }
    }
    DHLOGE("GetSwitchOpenedSession no session is open.");
    return ERR_DH_INPUT_SERVER_SINK_GET_OPEN_SESSION_FAIL;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
