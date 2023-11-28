/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DISRIBUTED_INPUT_SOURCE_STUB_H
#define DISRIBUTED_INPUT_SOURCE_STUB_H

#include "i_distributed_source_input.h"

#include <atomic>
#include <iostream>
#include <mutex>

#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceStub : public IRemoteStub<IDistributedSourceInput> {
public:
    DistributedInputSourceStub();
    ~DistributedInputSourceStub() override;

    void RegRespFunMap();
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t HandleInitDistributedHardware(MessageParcel &reply);
    int32_t HandleReleaseDistributedHardware(MessageParcel &reply);
    int32_t HandleRegisterDistributedHardware(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnregisterDistributedHardware(MessageParcel &data, MessageParcel &reply);
    int32_t HandlePrepareRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnprepareRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStopRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartRelayTypeRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStopRelayTypeRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandlePrepareRelayRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnprepareRelayRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartDhidRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStopDhidRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartRelayDhidRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStopRelayDhidRemoteInput(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterAddWhiteListCallback(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterDelWhiteListCallback(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterSimulationEventListener(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnregisterSimulationEventListener(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterSessionStateCb(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnregisterSessionStateCb(MessageParcel &data, MessageParcel &reply);
    bool HasEnableDHPermission();
    bool HasAccessDHPermission();
    DISALLOW_COPY_AND_MOVE(DistributedInputSourceStub);
private:
    std::atomic<bool> sourceManagerInitFlag_ {false};
    std::mutex operatorMutex_;
    using DistributedInputSourceFunc = int32_t (DistributedInputSourceStub::*)(MessageParcel &data,
        MessageParcel &reply);
    std::map<uint32_t, DistributedInputSourceFunc> memberFuncMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_STUB_H
