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

#ifndef OHOS_DISTRIBUTED_INPUT_CONTEXT_H
#define OHOS_DISTRIBUTED_INPUT_CONTEXT_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <refbase.h>

#include "single_instance.h"

#include "constants.h"
#include "device_type.h"
#include "distributed_hardware_fwk_kit.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "system_ability_definition.h"

#include "dinput_log.h"
#include "i_dinput_context.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
const nlohmann::json ENABLE_LOW_LATENCY = {
    {DH_TYPE, DHType::INPUT},
    {LOW_LATENCY_ENABLE, true},
};

const nlohmann::json DISABLE_LOW_LATENCY = {
    {DH_TYPE, DHType::INPUT},
    {LOW_LATENCY_ENABLE, false},
};
class DInputContext {
DECLARE_SINGLE_INSTANCE_BASE(DInputContext);
public:
    std::string GetScreenInfoKey(const std::string &devId, const uint64_t sourceWinId);
    int32_t RemoveSinkScreenInfo(const std::string &screenInfoKey);
    int32_t UpdateSinkScreenInfo(const std::string &screenInfoKey, const SinkScreenInfo &sinkScreenInfo);
    SinkScreenInfo GetSinkScreenInfo(const std::string &screenInfoKey);
    const std::unordered_map<std::string, SinkScreenInfo> &GetAllSinkScreenInfo();

    int32_t RemoveSrcScreenInfo(const std::string &screenInfoKey);
    int32_t UpdateSrcScreenInfo(const std::string &screenInfoKey, const SrcScreenInfo &srcScreenInfo);
    SrcScreenInfo GetSrcScreenInfo(const std::string &screenInfoKey);

    void SetLocalTouchScreenInfo(const LocalTouchScreenInfo &localTouchScreenInfo);
    LocalTouchScreenInfo GetLocalTouchScreenInfo();
    std::shared_ptr<DistributedHardwareFwkKit> GetDHFwkKit();
    sptr<IRemoteObject> GetRemoteObject(const int32_t saId);
    void AddRemoteObject(const int32_t saId, const sptr<IRemoteObject> &remoteObject);
    void RemoveRemoteObject(const int32_t saId);

private:
    int32_t CalculateTransformInfo(SinkScreenInfo &sinkScreenInfo);

private:
    DInputContext() = default;
    ~DInputContext();

    /* the key is Combination of sink's localDeviceId and windowId, the value is sinkScreenInfo */
    std::unordered_map<std::string, SinkScreenInfo> sinkScreenInfoMap_;

    /* the key is Combination of source's localDeviceId and windowId, the value is sourceScreenInfo */
    std::unordered_map<std::string, SrcScreenInfo> srcScreenInfoMap_;
    LocalTouchScreenInfo localTouchScreenInfo_;
    std::mutex srcMapMutex_;
    std::mutex sinkMapMutex_;
    std::mutex localTouchScreenInfoMutex_;
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit_;
    std::mutex dhFwkKitMutex_;
    std::unordered_map<int32_t, sptr<IRemoteObject>> remoteObjects_;
    std::mutex remoteObjectsMutex_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_INPUT_CONTEXT_H