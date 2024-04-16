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

#include "distributed_input_sink_stub.h"

#include "accesstoken_kit.h"
#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_ipc_interface_code.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "i_sharing_dhid_listener.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputSinkStub::DistributedInputSinkStub()
{
    DHLOGI("DistributedInputSinkStub ctor!");
    memberFuncMap_[static_cast<uint32_t>(IDInputSinkInterfaceCode::INIT)] =
        &DistributedInputSinkStub::InitInner;
    memberFuncMap_[static_cast<uint32_t>(IDInputSinkInterfaceCode::RELEASE)] =
        &DistributedInputSinkStub::ReleaseInner;
    memberFuncMap_[static_cast<uint32_t>(IDInputSinkInterfaceCode::NOTIFY_START_DSCREEN)] =
        &DistributedInputSinkStub::NotifyStartDScreenInner;
    memberFuncMap_[static_cast<uint32_t>(IDInputSinkInterfaceCode::NOTIFY_STOP_DSCREEN)] =
        &DistributedInputSinkStub::NotifyStopDScreenInner;
    memberFuncMap_[static_cast<uint32_t>(IDInputSinkInterfaceCode::REGISTER_SHARING_DHID_LISTENER)] =
        &DistributedInputSinkStub::RegisterSharingDhIdListenerInner;
    memberFuncMap_[static_cast<uint32_t>(IDInputSinkInterfaceCode::GET_SINK_SCREEN_INFOS)] =
        &DistributedInputSinkStub::RegisterGetSinkScreenInfosInner;
}

DistributedInputSinkStub::~DistributedInputSinkStub()
{
    DHLOGI("DistributedInputSinkStub dtor!");
    memberFuncMap_.clear();
}

bool DistributedInputSinkStub::HasEnableDHPermission()
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    const std::string permissionName = "ohos.permission.ENABLE_DISTRIBUTED_HARDWARE";
    int32_t result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken,
        permissionName);
    return (result == Security::AccessToken::PERMISSION_GRANTED);
}

int32_t DistributedInputSinkStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("DistributedInputSinkStub read token valid failed");
        return ERR_DH_INPUT_IPC_READ_TOKEN_VALID_FAIL;
    }
    auto iter = memberFuncMap_.find(code);
    if (iter == memberFuncMap_.end()) {
        DHLOGE("invalid request code is %{public}d.", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    DistributedInputSinkFunc &func = iter->second;
    return (this->*func)(data, reply, option);
}

int32_t DistributedInputSinkStub::InitInner(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (!HasEnableDHPermission()) {
        DHLOGE("The caller has no ENABLE_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SINK_ENABLE_PERMISSION_CHECK_FAIL;
    }
    DHLOGI("DistributedInputSinkStub InitInner start");
    int32_t ret = Init();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSinkStub write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return ret;
}

int32_t DistributedInputSinkStub::ReleaseInner(MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (!HasEnableDHPermission()) {
        DHLOGE("The caller has no ENABLE_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SINK_ENABLE_PERMISSION_CHECK_FAIL;
    }
    int32_t ret = Release();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSinkStub write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return ret;
}

int32_t DistributedInputSinkStub::NotifyStartDScreenInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    int32_t sessionId = data.ReadInt32();
    std::string uuid = data.ReadString();
    uint64_t sourceWinId = data.ReadUint64();
    uint32_t sourceWinWidth = data.ReadUint32();
    uint32_t sourceWinHeight = data.ReadUint32();
    std::string sourcePhyId = data.ReadString();
    uint32_t sourcePhyFd = data.ReadUint32();
    uint32_t sourcePhyWidth = data.ReadUint32();
    uint32_t sourcePhyHeight = data.ReadUint32();
    DHLOGI("OnRemoteRequest the data: devId: %{public}s, sourceWinId: %{public}" PRIu64 ", sourceWinWidth: %{public}d, "
        "sourceWinHeight: %{public}d, sourcePhyId: %{public}s, sourcePhyFd: %{public}d, sourcePhyWidth: %{public}d, "
        "sourcePhyHeight: %{public}d", GetAnonyString(devId).c_str(), sourceWinId, sourceWinWidth, sourceWinHeight,
        GetAnonyString(sourcePhyId).c_str(), sourcePhyFd, sourcePhyWidth, sourcePhyHeight);
    SrcScreenInfo srcScreenInfo = {
        .devId = devId,
        .uuid = uuid,
        .sessionId = sessionId,
        .sourceWinId = sourceWinId,
        .sourceWinWidth = sourceWinWidth,
        .sourceWinHeight = sourceWinHeight,
        .sourcePhyId = sourcePhyId,
        .sourcePhyFd = sourcePhyFd,
        .sourcePhyWidth = sourcePhyWidth,
        .sourcePhyHeight = sourcePhyHeight,
    };
    int32_t ret = NotifyStartDScreen(srcScreenInfo);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("write reply failed ret = %{public}d", ret);
        return ERR_DH_INPUT_RPC_REPLY_FAIL;
    }
    return ret;
}

int32_t DistributedInputSinkStub::NotifyStopDScreenInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string srcScreenInfoKey = data.ReadString();
    DHLOGI("OnRemoteRequest srcScreenInfoKey: %{public}s", GetAnonyString(srcScreenInfoKey).c_str());
    int ret = NotifyStopDScreen(srcScreenInfoKey);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("write version failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_RPC_REPLY_FAIL;
    }
    return ret;
}

int32_t DistributedInputSinkStub::RegisterSharingDhIdListenerInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    sptr<ISharingDhIdListener> listener = iface_cast<ISharingDhIdListener>(data.ReadRemoteObject());
    if (listener == nullptr) {
        DHLOGE("RegisterSharingDhIdListenerInner failed, listener is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterSharingDhIdListener(listener);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("RegisterSharingDhIdListenerInner write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_SINK_STUB_REGISTER_SHARING_DHID_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSinkStub::RegisterGetSinkScreenInfosInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    sptr<IGetSinkScreenInfosCallback> callback =
        iface_cast<IGetSinkScreenInfosCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("RegisterGetSinkScreenInfosInner failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterGetSinkScreenInfosCallback(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return ret;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
