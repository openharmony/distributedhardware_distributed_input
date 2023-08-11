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

#ifndef OHOS_DISTRIBUTED_INPUT_UTILS_TOOL_H
#define OHOS_DISTRIBUTED_INPUT_UTILS_TOOL_H

#include <cstdint>
#include <string>

#include "nlohmann/json.hpp"

#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
struct DevInfo {
    std::string networkId;
    std::string deviceName;
    uint16_t deviceType;
};

DevInfo GetLocalDeviceInfo();
std::string GetLocalNetworkId();
std::string GetUUIDBySoftBus(const std::string &networkId);
uint64_t GetCurrentTime();
std::string SetAnonyId(const std::string &message);
/* IsString, IsUInt32 and IsUInt64 are used to valid json key */
bool IsBoolean(const nlohmann::json &jsonObj, const std::string &key);
bool IsString(const nlohmann::json &jsonObj, const std::string &key);
bool IsInt32(const nlohmann::json &jsonObj, const std::string &key);
bool IsInt64(const nlohmann::json &jsonObj, const std::string &key);
bool IsUInt16(const nlohmann::json &jsonObj, const std::string &key);
bool IsUInt32(const nlohmann::json &jsonObj, const std::string &key);
bool IsUInt64(const nlohmann::json &jsonObj, const std::string &key);
bool IsArray(const nlohmann::json &jsonObj, const std::string &key);
std::string GetNodeDesc(std::string parameters);
std::string GetAnonyString(const std::string &value);
std::string GetAnonyInt32(const int32_t value);
std::string Sha256(const std::string &string);
void CloseFd(int &fd);
int BitIsSet(const unsigned long *array, int bit);
void StringSplitToVector(const std::string &str, const char split, std::vector<std::string> &vecStr);
int OpenInputDeviceFdByPath(const std::string &devicePath);
std::string ConvertErrNo();
void ScanInputDevicesPath(const std::string &dirName, std::vector<std::string> &vecInputDevPath);
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_INPUT_UTILS_TOOL_H