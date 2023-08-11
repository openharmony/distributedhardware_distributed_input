/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "input_check_param.h"

#include "constants_dinput.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DInputCheckParam &DInputCheckParam::GetInstance(void)
{
    static DInputCheckParam instance;
    return instance;
}
bool DInputCheckParam::CheckParam(const std::string &deviceId, sptr<IRemoteBroker> callback)
{
    if (deviceId.empty() || deviceId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam deviceId is empty or deviceId size too long.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckParam(const std::string &deviceId, const uint32_t &inputTypes,
    sptr<IRemoteBroker> callback)
{
    if (deviceId.empty() || deviceId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam deviceId is empty or deviceId size too long.");
        return false;
    }
    if (inputTypes > static_cast<uint32_t>(DInputDeviceType::ALL) ||
        inputTypes == static_cast<uint32_t>(DInputDeviceType::NONE) ||
        !(inputTypes & static_cast<uint32_t>(DInputDeviceType::ALL))) {
        DHLOGE("CheckParam, inputTypes is invalids.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckParam(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
    sptr<IRemoteBroker> callback)
{
    if (srcId.empty() || srcId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam srcId is empty or srcId size too long.");
        return false;
    }
    if (sinkId.empty() || sinkId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam sinkId is empty or sinkId size too long.");
        return false;
    }
    if (inputTypes > static_cast<uint32_t>(DInputDeviceType::ALL) ||
        inputTypes == static_cast<uint32_t>(DInputDeviceType::NONE) ||
        !(inputTypes & static_cast<uint32_t>(DInputDeviceType::ALL))) {
        DHLOGE("CheckParam, inputTypes is invalids.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckParam(const std::string &srcId, const std::string &sinkId,
    sptr<IRemoteBroker> callback)
{
    if (srcId.empty() || srcId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam srcId is empty or srcId size too long.");
        return false;
    }
    if (sinkId.empty() || sinkId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam sinkId is empty or sinkId size too long.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckParam(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IRemoteBroker> callback)
{
    if (sinkId.empty() || sinkId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam sinkId is empty or sinkId size too long.");
        return false;
    }
    if (dhIds.empty()) {
        DHLOGE("CheckParam dhIds is empty.");
        return false;
    }
    for (auto iter : dhIds) {
        if (iter.size() > DH_ID_LENGTH_MAX) {
            DHLOGE("CheckParam dhId size is too long.");
            return false;
        }
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckParam(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IRemoteBroker> callback)
{
    if (srcId.empty() || srcId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam srcId is empty or srcId size too long.");
        return false;
    }
    if (sinkId.empty() || sinkId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam sinkId is empty or sinkId size too long.");
        return false;
    }
    if (dhIds.empty()) {
        DHLOGE("CheckParam dhIds is empty.");
        return false;
    }
    for (auto iter : dhIds) {
        if (iter.size() > DH_ID_LENGTH_MAX) {
            DHLOGE("CheckParam dhId size is too long.");
            return false;
        }
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckRegisterParam(const std::string &devId, const std::string &dhId,
    const std::string &parameters, const std::shared_ptr<RegisterCallback> &callback)
{
    if (devId.empty() || devId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam devId is empty or devId size too long.");
        return false;
    }
    if (dhId.empty() || dhId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam dhId is empty or dhId size too long.");
        return false;
    }
    if (parameters.empty()) {
        DHLOGE("CheckParam parameters is empty.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

bool DInputCheckParam::CheckUnregisterParam(const std::string &devId, const std::string &dhId,
    const std::shared_ptr<UnregisterCallback> &callback)
{
    if (devId.empty() || devId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam devId is empty or devId size too long.");
        return false;
    }
    if (dhId.empty() || dhId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam dhId is empty or dhId size too long.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS