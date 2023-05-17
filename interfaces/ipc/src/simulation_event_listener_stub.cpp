/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "simulation_event_listener_stub.h"

#include "string_ex.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
SimulationEventListenerStub::SimulationEventListenerStub() {}

SimulationEventListenerStub::~SimulationEventListenerStub() {}

int32_t SimulationEventListenerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("SimulationEventListenerStub read token valid failed");
        return ERR_DH_INPUT_IPC_READ_TOKEN_VALID_FAIL;
    }
    ISimulationEventListener::Message msgCode = static_cast<ISimulationEventListener::Message>(code);
    switch (msgCode) {
        case ISimulationEventListener::Message::RESULT_ON: {
            uint32_t eventType = data.ReadUint32();
            uint32_t eventCode = data.ReadUint32();
            int32_t eventValue = data.ReadInt32();
            int32_t ret = OnSimulationEvent(eventType, eventCode, eventValue);
            if (!reply.WriteInt32(ret)) {
                DHLOGE("SimulationEventListenerStub write ret failed");
                return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
            }
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return DH_SUCCESS;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
