/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "distributed_input_sink_proxy.h"

#include "dinput_errcode.h"
#include "dinput_ipc_interface_code.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "i_get_sink_screen_infos_call_back.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputSinkProxy::DistributedInputSinkProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IDistributedSinkInput>(object)
{}

DistributedInputSinkProxy::~DistributedInputSinkProxy()
{}

int32_t DistributedInputSinkProxy::Init()
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSinkProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    int32_t result = ERR_DH_INPUT_SINK_PROXY_INIT_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSinkInterfaceCode::INIT), data, reply);
    if (!ret) {
        DHLOGE("SendRequest fail!");
        return ERR_DH_INPUT_SINK_PROXY_INIT_FAIL;
    }
    result = reply.ReadInt32();
    return result;
}

int32_t DistributedInputSinkProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSinkProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    int32_t result = ERR_DH_INPUT_SINK_PROXY_RELEASE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSinkInterfaceCode::RELEASE), data, reply);
    if (!ret) {
        DHLOGE("SendRequest fail!");
        return ERR_DH_INPUT_SINK_PROXY_RELEASE_FAIL;
    }
    result = reply.ReadInt32();
    return result;
}

int32_t DistributedInputSinkProxy::RegisterGetSinkScreenInfosCallback(sptr<IGetSinkScreenInfosCallback> callback)
{
    if (callback == nullptr) {
        DHLOGE("getSinkScreenInfosCallback is null.");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SINK_PROXY_REGISTER_GETSINKSCREENINFOS_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSinkInterfaceCode::GET_SINK_SCREEN_INFOS), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSinkProxy::NotifyStartDScreen(const SrcScreenInfo &remoteCtrlInfo)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken fail!");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    DHLOGI("DinputSinkProxy the data: devId: %{public}s, sourceWinId: %{public}" PRIu64 ", sourceWinWidth: %{public}d, "
        "sourceWinHeight: %{public}d, sourcePhyId: %{public}s, sourcePhyFd: %{public}d, sourcePhyWidth: %{public}d, "
        "sourcePhyHeight: %{public}d", GetAnonyString(remoteCtrlInfo.devId).c_str(), remoteCtrlInfo.sourceWinId,
        remoteCtrlInfo.sourceWinWidth, remoteCtrlInfo.sourceWinHeight,
        GetAnonyString(remoteCtrlInfo.sourcePhyId).c_str(), remoteCtrlInfo.sourcePhyFd,
        remoteCtrlInfo.sourcePhyWidth, remoteCtrlInfo.sourcePhyHeight);
    if (!data.WriteString(remoteCtrlInfo.devId) || !data.WriteInt32(remoteCtrlInfo.sessionId) ||
        !data.WriteString(remoteCtrlInfo.uuid) || !data.WriteUint64(remoteCtrlInfo.sourceWinId) ||
        !data.WriteUint32(remoteCtrlInfo.sourceWinWidth) || !data.WriteUint32(remoteCtrlInfo.sourceWinHeight) ||
        !data.WriteString(remoteCtrlInfo.sourcePhyId) || !data.WriteUint32(remoteCtrlInfo.sourcePhyFd) ||
        !data.WriteUint32(remoteCtrlInfo.sourcePhyWidth) || !data.WriteUint32(remoteCtrlInfo.sourcePhyHeight)) {
        DHLOGE("DistributedInputSinkProxy write params failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    int32_t result = ERR_DH_INPUT_NOTIFY_START_DSCREEN_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSinkInterfaceCode::NOTIFY_START_DSCREEN), data, reply);
    if (!ret) {
        DHLOGE("SendRequest fail!");
        return ERR_DH_INPUT_NOTIFY_START_DSCREEN_FAIL;
    }
    result = reply.ReadInt32();
    return result;
}

int32_t DistributedInputSinkProxy::NotifyStopDScreen(const std::string &srcScreenInfoKey)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken fail!");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcScreenInfoKey)) {
        DHLOGE("DistributedInputSinkProxy write params failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    int32_t result = ERR_DH_INPUT_NOTIFY_STOP_DSCREEN_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSinkInterfaceCode::NOTIFY_STOP_DSCREEN), data, reply);
    if (!ret) {
        DHLOGE("SendRequest fail!");
        return ERR_DH_INPUT_NOTIFY_STOP_DSCREEN_FAIL;
    }
    result = reply.ReadInt32();
    return result;
}

int32_t DistributedInputSinkProxy::RegisterSharingDhIdListener(sptr<ISharingDhIdListener> sharingDhIdListener)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("RegisterSharingDhIdListener write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(sharingDhIdListener->AsObject())) {
        DHLOGE("RegisterSharingDhIdListener write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SINK_PROXY_REGISTER_SHARING_DHID_LISTENER_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSinkInterfaceCode::REGISTER_SHARING_DHID_LISTENER),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

bool DistributedInputSinkProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != DH_SUCCESS) {
        return false;
    }
    return true;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
