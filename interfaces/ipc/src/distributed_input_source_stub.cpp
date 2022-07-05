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

#include "distributed_input_source_stub.h"

#include "distributed_hardware_log.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"

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
    std::string deviceId = data.ReadString();
    sptr<IPrepareDInputCallback> callback =
        iface_cast<IPrepareDInputCallback>(data.ReadRemoteObject());
    sptr<IAddWhiteListInfosCallback> addCallback =
        iface_cast<IAddWhiteListInfosCallback>(data.ReadRemoteObject());
    int32_t ret = PrepareRemoteInput(deviceId, callback, addCallback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub prepareRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnprepareRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    std::string deviceId = data.ReadString();
    sptr<IUnprepareDInputCallback> callback =
        iface_cast<IUnprepareDInputCallback>(data.ReadRemoteObject());
    sptr<IDelWhiteListInfosCallback> delCallback =
        iface_cast<IDelWhiteListInfosCallback>(data.ReadRemoteObject());
    int32_t ret = UnprepareRemoteInput(deviceId, callback, delCallback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub unprepareRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    std::string deviceId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStartDInputCallback> callback = iface_cast<IStartDInputCallback>(data.ReadRemoteObject());
    int32_t ret = StartRemoteInput(deviceId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub startRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    std::string deviceId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStopDInputCallback> callback = iface_cast<IStopDInputCallback>(data.ReadRemoteObject());
    int32_t ret = StopRemoteInput(deviceId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub stopRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleIsStartDistributedInput(MessageParcel &data, MessageParcel &reply)
{
    uint32_t inputType = data.ReadUint32();
    sptr<IStartDInputServerCallback> callback = iface_cast<IStartDInputServerCallback>(data.ReadRemoteObject());
    int32_t ret = IsStartDistributedInput(inputType, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub isStartDistributedInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("DistributedInputSourceStub read token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    switch (code) {
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::INIT): {
            return HandleInitDistributedHardware(reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::RELEASE): {
            return HandleReleaseDistributedHardware(reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::REGISTER_REMOTE_INPUT): {
            return HandleRegisterDistributedHardware(data, reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::UNREGISTER_REMOTE_INPUT): {
            return HandleUnregisterDistributedHardware(data, reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::PREPARE_REMOTE_INPUT): {
            return HandlePrepareRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::UNPREPARE_REMOTE_INPUT): {
            return HandleUnprepareRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::START_REMOTE_INPUT): {
            return HandleStartRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::STOP_REMOTE_INPUT): {
            return HandleStopRemoteInput(data, reply);
        }
        case static_cast<uint32_t>(IDistributedSourceInput::MessageCode::ISSTART_REMOTE_INPUT): {
            return HandleIsStartDistributedInput(data, reply);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
