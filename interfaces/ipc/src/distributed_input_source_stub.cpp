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

#include "distributed_input_source_stub.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_ipc_interface_code.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputSourceStub::DistributedInputSourceStub()
{}

DistributedInputSourceStub::~DistributedInputSourceStub()
{}

int32_t DistributedInputSourceStub::HandleInitDistributedHardware(MessageParcel &reply)
{
    int32_t ret = Init();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub Init write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleReleaseDistributedHardware(MessageParcel &reply)
{
    int32_t ret = Release();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub release write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterDistributedHardware(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleRegisterDistributedHardware failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string params = data.ReadString();
    sptr<IRegisterDInputCallback> callback = iface_cast<IRegisterDInputCallback>(data.ReadRemoteObject());
    int32_t ret = RegisterDistributedHardware(devId, dhId, params, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub registerDistributedHardware write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnregisterDistributedHardware(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleUnregisterDistributedHardware failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    sptr<IUnregisterDInputCallback> callback = iface_cast<IUnregisterDInputCallback>(data.ReadRemoteObject());
    int32_t ret = UnregisterDistributedHardware(devId, dhId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub unregisterDistributedHardware write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandlePrepareRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandlePrepareRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string deviceId = data.ReadString();
    sptr<IPrepareDInputCallback> callback = iface_cast<IPrepareDInputCallback>(data.ReadRemoteObject());
    int32_t ret = PrepareRemoteInput(deviceId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandlePrepareRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnprepareRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleUnprepareRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string deviceId = data.ReadString();
    sptr<IUnprepareDInputCallback> callback = iface_cast<IUnprepareDInputCallback>(data.ReadRemoteObject());
    int32_t ret = UnprepareRemoteInput(deviceId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnprepareRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStartRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string deviceId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStartDInputCallback> callback = iface_cast<IStartDInputCallback>(data.ReadRemoteObject());
    int32_t ret = StartRemoteInput(deviceId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub startRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStopRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string deviceId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStopDInputCallback> callback = iface_cast<IStopDInputCallback>(data.ReadRemoteObject());
    int32_t ret = StopRemoteInput(deviceId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub stopRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRelayTypeRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStartRelayTypeRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStartDInputCallback> callback = iface_cast<IStartDInputCallback>(data.ReadRemoteObject());
    int32_t ret = StartRemoteInput(srcId, sinkId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRelayTypeRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStopRelayTypeRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStopDInputCallback> callback = iface_cast<IStopDInputCallback>(data.ReadRemoteObject());
    int32_t ret = StopRemoteInput(srcId, sinkId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandlePrepareRelayRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandlePrepareRelayRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    sptr<IPrepareDInputCallback> callback = iface_cast<IPrepareDInputCallback>(data.ReadRemoteObject());
    int32_t ret = PrepareRemoteInput(srcId, sinkId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandlePrepareRelayRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnprepareRelayRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleUnprepareRelayRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    sptr<IUnprepareDInputCallback> callback = iface_cast<IUnprepareDInputCallback>(data.ReadRemoteObject());
    int32_t ret = UnprepareRemoteInput(srcId, sinkId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnprepareRelayRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStartDhidRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string sinkId = data.ReadString();

    std::vector<std::string> tempVector;
    uint32_t vecSize = data.ReadUint32();
    if (vecSize > IPC_VECTOR_MAX_SIZE) {
        DHLOGE("HandleStartDhidRemoteInput vecSize too large");
        return ERR_DH_INPUT_IPC_READ_VALID_FAIL;
    }

    for (uint32_t i = 0; i < vecSize; i++) {
        std::string dhid = data.ReadString();
        if (dhid.empty()) {
            DHLOGE("HandleStartDhidRemoteInput dhid is empty");
            continue;
        }
        tempVector.push_back(dhid);
    }

    sptr<IStartStopDInputsCallback> callback = iface_cast<IStartStopDInputsCallback>(data.ReadRemoteObject());
    int32_t ret = StartRemoteInput(sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStartDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStopDhidRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string sinkId = data.ReadString();

    std::vector<std::string> tempVector;
    uint32_t vecSize = data.ReadUint32();
    if (vecSize > IPC_VECTOR_MAX_SIZE) {
        DHLOGE("HandleStopDhidRemoteInput vecSize too large");
        return ERR_DH_INPUT_IPC_READ_VALID_FAIL;
    }

    for (uint32_t i = 0; i < vecSize; i++) {
        std::string dhid = data.ReadString();
        if (dhid.empty()) {
            DHLOGE("HandleStopDhidRemoteInput dhid is empty");
            continue;
        }
        tempVector.push_back(dhid);
    }

    sptr<IStartStopDInputsCallback> callback = iface_cast<IStartStopDInputsCallback>(data.ReadRemoteObject());
    int32_t ret = StopRemoteInput(sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStopDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRelayDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStartRelayDhidRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();

    std::vector<std::string> tempVector;
    uint32_t vecSize = data.ReadUint32();
    if (vecSize > IPC_VECTOR_MAX_SIZE) {
        DHLOGE("HandleStartRelayDhidRemoteInput vecSize too large");
        return ERR_DH_INPUT_IPC_READ_VALID_FAIL;
    }

    for (uint32_t i = 0; i < vecSize; i++) {
        std::string dhid = data.ReadString();
        if (dhid.empty()) {
            DHLOGE("HandleStartRelayDhidRemoteInput dhid is empty");
            continue;
        }
        tempVector.push_back(dhid);
    }

    sptr<IStartStopDInputsCallback> callback = iface_cast<IStartStopDInputsCallback>(data.ReadRemoteObject());
    int32_t ret = StartRemoteInput(srcId, sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStartRelayDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRelayDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleStopRelayDhidRemoteInput failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();

    std::vector<std::string> tempVector;
    uint32_t vecSize = data.ReadUint32();
    if (vecSize > IPC_VECTOR_MAX_SIZE) {
        DHLOGE("HandleStopRelayDhidRemoteInput vecSize too large");
        return ERR_DH_INPUT_IPC_READ_VALID_FAIL;
    }

    for (uint32_t i = 0; i < vecSize; i++) {
        std::string dhid = data.ReadString();
        if (dhid.empty()) {
            DHLOGE("HandleStopRelayDhidRemoteInput dhid is empty");
            continue;
        }
        tempVector.push_back(dhid);
    }

    sptr<IStartStopDInputsCallback> callback = iface_cast<IStartStopDInputsCallback>(data.ReadRemoteObject());
    int32_t ret = StopRemoteInput(srcId, sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStopRelayDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleSyncNodeInfoRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    std::string userDevId = data.ReadString();
    std::string dhid = data.ReadString();
    std::string nodeDesc = data.ReadString();

    int32_t ret = SyncNodeInfoRemoteInput(userDevId, dhid, nodeDesc);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleSyncNodeInfoRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterAddWhiteListCallback(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleRegisterAddWhiteListCallback failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    sptr<IAddWhiteListInfosCallback> callback = iface_cast<IAddWhiteListInfosCallback>(data.ReadRemoteObject());
    int32_t ret = RegisterAddWhiteListCallback(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterAddWhiteListCallback write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterDelWhiteListCallback(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleRegisterDelWhiteListCallback failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    sptr<IDelWhiteListInfosCallback> callback = iface_cast<IDelWhiteListInfosCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGI("HandleRegisterDelWhiteListCallback callback is null");
    }

    int32_t ret = RegisterDelWhiteListCallback(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterDelWhiteListCallback write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterInputNodeListener(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleRegisterInputNodeListener failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    sptr<InputNodeListener> callback = iface_cast<InputNodeListener>(data.ReadRemoteObject());
    int32_t ret = RegisterInputNodeListener(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterInputNodeListener write ret failed");
        return ERR_DH_INPUT_SOURCE_STUB_REGISTER_NODE_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnRegisterInputNodeListener(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleUnRegisterInputNodeListener failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    sptr<InputNodeListener> callback = iface_cast<InputNodeListener>(data.ReadRemoteObject());
    int32_t ret = RegisterInputNodeListener(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnRegisterInputNodeListener write ret failed");
        return ERR_DH_INPUT_SOURCE_STUB_UNREGISTER_NODE_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterSimulationEventListener(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleRegisterSimulationEventListener failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    sptr<ISimulationEventListener> callback = iface_cast<ISimulationEventListener>(data.ReadRemoteObject());
    int32_t ret = RegisterSimulationEventListener(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterSimulationEventListener write ret failed, ret = %d", ret);
        return ERR_DH_INPUT_SOURCE_STUB_REGISTER_SIMULATION_EVENT_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnregisterSimulationEventListener(MessageParcel &data, MessageParcel &reply)
{
    if (data.ReadRemoteObject() == nullptr) {
        DHLOGE("HandleUnregisterSimulationEventListener failed, data.ReadRemoteObject is nullptr.");
        return ERR_DH_INPUT_NULLPTR_NOT_VERIFY;
    }
    sptr<ISimulationEventListener> callback = iface_cast<ISimulationEventListener>(data.ReadRemoteObject());
    int32_t ret = UnregisterSimulationEventListener(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnregisterSimulationEventListener write ret failed, ret = %d", ret);
        return ERR_DH_INPUT_SOURCE_STUB_UNREGISTER_SIMULATION_EVENT_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("DistributedInputSourceStub read token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    switch (code) {
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::INIT): {
            return HandleInitDistributedHardware(reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::RELEASE): {
            return HandleReleaseDistributedHardware(reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_REMOTE_INPUT): {
            return HandleRegisterDistributedHardware(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_REMOTE_INPUT): {
            return HandleUnregisterDistributedHardware(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::PREPARE_REMOTE_INPUT): {
            return HandlePrepareRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::UNPREPARE_REMOTE_INPUT): {
            return HandleUnprepareRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::START_REMOTE_INPUT): {
            return HandleStartRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_REMOTE_INPUT): {
            return HandleStopRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::START_RELAY_TYPE_REMOTE_INPUT): {
            return HandleStartRelayTypeRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_RELAY_TYPE_REMOTE_INPUT): {
            return HandleStopRelayTypeRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::PREPARE_RELAY_REMOTE_INPUT): {
            return HandlePrepareRelayRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::UNPREPARE_RELAY_REMOTE_INPUT): {
            return HandleUnprepareRelayRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::START_DHID_REMOTE_INPUT): {
            return HandleStartDhidRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_DHID_REMOTE_INPUT): {
            return HandleStopDhidRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::START_RELAY_DHID_REMOTE_INPUT): {
            return HandleStartRelayDhidRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_RELAY_DHID_REMOTE_INPUT): {
            return HandleStopRelayDhidRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_ADD_WHITE_LIST_CB_REMOTE_INPUT): {
            return HandleRegisterAddWhiteListCallback(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_DEL_WHITE_LIST_CB_REMOTE_INPUT): {
            return HandleRegisterDelWhiteListCallback(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_NODE_LISTENER): {
            return HandleRegisterInputNodeListener(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_NODE_LISTENER): {
            return HandleUnRegisterInputNodeListener(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_SIMULATION_EVENT_LISTENER): {
            return HandleRegisterSimulationEventListener(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_SIMULATION_EVENT_LISTENER): {
            return HandleUnregisterSimulationEventListener(data, reply);
        }
        case static_cast<uint32_t>(IDInputSourceInterfaceCode::SYNC_NODE_INFO_REMOTE_INPUT): {
            return HandleSyncNodeInfoRemoteInput(data, reply);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
