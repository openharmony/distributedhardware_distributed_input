/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTEDINPUTSTUB_FUZZER_H
#define DISTRIBUTEDINPUTSTUB_FUZZER_H

#define FUZZ_PROJECT_NAME "distributedinputstub_fuzzer.cpp"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "distributed_input_sink_stub.h"
#include "distributed_input_source_stub.h"

namespace OHOS {
namespace DistributedHardware {

class DistributedInputSinkStubFuzz : public
    OHOS::DistributedHardware::DistributedInput::DistributedInputSinkStub {
public:
    DistributedInputSinkStubFuzz() = default;
    virtual ~DistributedInputSinkStubFuzz() = default;
    int32_t Init();
    int32_t Release();
    int32_t RegisterGetSinkScreenInfosCallback(sptr<DistributedInput::IGetSinkScreenInfosCallback> callback);
    int32_t NotifyStartDScreen(const DistributedInput::SrcScreenInfo &remoteCtrlInfo);
    int32_t NotifyStopDScreen(const std::string &srcScreenInfoKey);
    int32_t RegisterSharingDhIdListener(sptr<DistributedInput::ISharingDhIdListener> sharingDhIdListener);
};

class DInputSourceCallBackStubFuzz : public
    OHOS::DistributedHardware::DistributedInput::DistributedInputSourceStub {
public:
    DInputSourceCallBackStubFuzz() = default;
    virtual ~DInputSourceCallBackStubFuzz() = default;
    int32_t Init();
    int32_t Release() ;
    int32_t RegisterDistributedHardware(
        const std::string &devId, const std::string &dhId, const std::string &parameters,
        sptr<DistributedInput::IRegisterDInputCallback> callback);
    int32_t UnregisterDistributedHardware(
        const std::string &devId, const std::string &dhId, sptr<DistributedInput::IUnregisterDInputCallback> callback);
    int32_t PrepareRemoteInput(const std::string &deviceId, sptr<DistributedInput::IPrepareDInputCallback> callback);
    int32_t UnprepareRemoteInput(const std::string &deviceId,
        sptr<DistributedInput::IUnprepareDInputCallback> callback);
    int32_t StartRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes,
        sptr<DistributedInput::IStartDInputCallback> callback);
    int32_t StopRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes,
        sptr<DistributedInput::IStopDInputCallback> callback);
    int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<DistributedInput::IStartDInputCallback> callback);
    int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<DistributedInput::IStopDInputCallback> callback);
    int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<DistributedInput::IPrepareDInputCallback> callback);
    int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<DistributedInput::IUnprepareDInputCallback> callback);
    int32_t StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<DistributedInput::IStartStopDInputsCallback> callback);
    int32_t StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<DistributedInput::IStartStopDInputsCallback> callback);
    int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<DistributedInput::IStartStopDInputsCallback> callback);
    int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<DistributedInput::IStartStopDInputsCallback> callback);

    int32_t RegisterAddWhiteListCallback(sptr<DistributedInput::IAddWhiteListInfosCallback> addWhiteListCallback);
    int32_t RegisterDelWhiteListCallback(sptr<DistributedInput::IDelWhiteListInfosCallback> delWhiteListCallback);
    int32_t RegisterSimulationEventListener(sptr<DistributedInput::ISimulationEventListener> listener);
    int32_t UnregisterSimulationEventListener(sptr<DistributedInput::ISimulationEventListener> listener);
    int32_t RegisterSessionStateCb(sptr<DistributedInput::ISessionStateCallback> callback);
    int32_t UnregisterSessionStateCb();
    void OnResult(const std::string &deviceId, const std::string &strJson);
    std::string deviceId_;
    std::string strJson_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif