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

#include "simulation_event_listener_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
SimulationEventListenerProxy::SimulationEventListenerProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<ISimulationEventListener>(object)
{
}

SimulationEventListenerProxy::~SimulationEventListenerProxy() {}

int32_t SimulationEventListenerProxy::OnSimulationEvent(uint32_t type, uint32_t code, int32_t value)
{
    int32_t result = ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("SimulationEventListenerProxy get remote failed");
        return result;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("SimulationEventListenerProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteUint32(type)) {
        DHLOGE("SimulationEventListenerProxy write type failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteUint32(code)) {
        DHLOGE("SimulationEventListenerProxy write code failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteInt32(value)) {
        DHLOGE("SimulationEventListenerProxy write value failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(ISimulationEventListener::Message::RESULT_ON), data, reply, option);
    if (ret == DH_SUCCESS) {
        result = reply.ReadInt32();
    } else {
        DHLOGE("SimulationEventListenerProxy SendRequest error:%{public}d", ret);
    }
    return result;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
