/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_SOURCE_PROXY_H
#define DISTRIBUTED_INPUT_SOURCE_PROXY_H

#include "i_distributed_source_input.h"

#include <iostream>

#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceProxy : public IRemoteProxy<IDistributedSourceInput> {
public:
    explicit DistributedInputSourceProxy(const sptr<IRemoteObject> &object);
    ~DistributedInputSourceProxy() override;

    int32_t Init() override;

    int32_t Release() override;

    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &parameters, sptr<IRegisterDInputCallback> callback) override;

    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        sptr<IUnregisterDInputCallback> callback) override;

    int32_t PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> callback) override;

    int32_t UnprepareRemoteInput(const std::string &deviceId, sptr<IUnprepareDInputCallback> callback) override;

    int32_t StartRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback) override;

    int32_t StopRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback) override;

    int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStartDInputCallback> callback) override;

    int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStopDInputCallback> callback) override;

    int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IPrepareDInputCallback> callback) override;

    int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IUnprepareDInputCallback> callback) override;

    int32_t StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback) override;

    int32_t StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback) override;

    int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback) override;

    int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback) override;

    int32_t RegisterAddWhiteListCallback(sptr<IAddWhiteListInfosCallback> addWhiteListCallback) override;
    int32_t RegisterDelWhiteListCallback(sptr<IDelWhiteListInfosCallback> delWhiteListCallback) override;

    int32_t RegisterSimulationEventListener(sptr<ISimulationEventListener> listener) override;
    int32_t UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener) override;

    int32_t RegisterSessionStateCb(sptr<ISessionStateCallback> callback) override;
    int32_t UnregisterSessionStateCb() override;

private:
    bool SendRequest(const uint32_t code, MessageParcel &data, MessageParcel &reply);

    static inline BrokerDelegator<DistributedInputSourceProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_PROXY_H
