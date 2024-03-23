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

#include "distributed_input_source_stub.h"

#include "accesstoken_kit.h"
#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_ipc_interface_code.h"
#include "dinput_log.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputSourceStub::DistributedInputSourceStub()
{
    RegRespFunMap();
}

DistributedInputSourceStub::~DistributedInputSourceStub()
{
    memberFuncMap_.clear();
}

void DistributedInputSourceStub::RegRespFunMap()
{
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleRegisterDistributedHardware;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleUnregisterDistributedHardware;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::PREPARE_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandlePrepareRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::UNPREPARE_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleUnprepareRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::START_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStartRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStopRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::START_RELAY_TYPE_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStartRelayTypeRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_RELAY_TYPE_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStopRelayTypeRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::PREPARE_RELAY_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandlePrepareRelayRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::UNPREPARE_RELAY_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleUnprepareRelayRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::START_DHID_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStartDhidRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_DHID_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStopDhidRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::START_RELAY_DHID_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStartRelayDhidRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_RELAY_DHID_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleStopRelayDhidRemoteInput;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_ADD_WHITE_LIST_CB_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleRegisterAddWhiteListCallback;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_DEL_WHITE_LIST_CB_REMOTE_INPUT)] =
        &DistributedInputSourceStub::HandleRegisterDelWhiteListCallback;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_SIMULATION_EVENT_LISTENER)] =
        &DistributedInputSourceStub::HandleRegisterSimulationEventListener;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_SIMULATION_EVENT_LISTENER)] =
        &DistributedInputSourceStub::HandleUnregisterSimulationEventListener;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_SESSION_STATE_CB)] =
        &DistributedInputSourceStub::HandleRegisterSessionStateCb;
    memberFuncMap_[static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_SESSION_STATE_CB)] =
        &DistributedInputSourceStub::HandleUnregisterSessionStateCb;
}

bool DistributedInputSourceStub::HasEnableDHPermission()
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    const std::string permissionName = "ohos.permission.ENABLE_DISTRIBUTED_HARDWARE";
    int32_t result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken,
        permissionName);
    return (result == Security::AccessToken::PERMISSION_GRANTED);
}

bool DistributedInputSourceStub::HasAccessDHPermission()
{
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    const std::string permissionName = "ohos.permission.ACCESS_DISTRIBUTED_HARDWARE";
    int32_t result = Security::AccessToken::AccessTokenKit::VerifyAccessToken(callerToken,
        permissionName);
    return (result == Security::AccessToken::PERMISSION_GRANTED);
}

int32_t DistributedInputSourceStub::HandleInitDistributedHardware(MessageParcel &reply)
{
    if (!HasEnableDHPermission()) {
        DHLOGE("The caller has no ENABLE_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ENABLE_PERMISSION_CHECK_FAIL;
    }
    std::unique_lock<std::mutex> lock(operatorMutex_);
    if (sourceManagerInitFlag_.load()) {
        DHLOGE("DistributedInputSourceStub already init.");
        return DH_SUCCESS;
    }
    int32_t ret = Init();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub Init write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    sourceManagerInitFlag_.store(true);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleReleaseDistributedHardware(MessageParcel &reply)
{
    if (!HasEnableDHPermission()) {
        DHLOGE("The caller has no ENABLE_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ENABLE_PERMISSION_CHECK_FAIL;
    }
    std::unique_lock<std::mutex> lock(operatorMutex_);
    if (!sourceManagerInitFlag_.load()) {
        DHLOGE("DistributedInputSourceStub already Release.");
        return DH_SUCCESS;
    }
    int32_t ret = Release();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("DistributedInputSourceStub release write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    sourceManagerInitFlag_.store(false);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterDistributedHardware(MessageParcel &data, MessageParcel &reply)
{
    if (!HasEnableDHPermission()) {
        DHLOGE("The caller has no ENABLE_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ENABLE_PERMISSION_CHECK_FAIL;
    }
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string params = data.ReadString();
    sptr<IRegisterDInputCallback> callback = iface_cast<IRegisterDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleRegisterDistributedHardware failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterDistributedHardware(devId, dhId, params, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterDistributedHardware write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnregisterDistributedHardware(MessageParcel &data, MessageParcel &reply)
{
    if (!HasEnableDHPermission()) {
        DHLOGE("The caller has no ENABLE_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ENABLE_PERMISSION_CHECK_FAIL;
    }
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    sptr<IUnregisterDInputCallback> callback = iface_cast<IUnregisterDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleUnregisterDistributedHardware failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = UnregisterDistributedHardware(devId, dhId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnregisterDistributedHardware write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandlePrepareRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string deviceId = data.ReadString();
    sptr<IPrepareDInputCallback> callback = iface_cast<IPrepareDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandlePrepareRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = PrepareRemoteInput(deviceId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandlePrepareRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnprepareRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string deviceId = data.ReadString();
    sptr<IUnprepareDInputCallback> callback = iface_cast<IUnprepareDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleUnprepareRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = UnprepareRemoteInput(deviceId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnprepareRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string deviceId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStartDInputCallback> callback = iface_cast<IStartDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleStartRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StartRemoteInput(deviceId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStartRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string deviceId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStopDInputCallback> callback = iface_cast<IStopDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleStopRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StopRemoteInput(deviceId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStopRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRelayTypeRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStartDInputCallback> callback = iface_cast<IStartDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleStartRelayTypeRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StartRemoteInput(srcId, sinkId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStartRelayTypeRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRelayTypeRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    uint32_t inputTypes = data.ReadUint32();
    sptr<IStopDInputCallback> callback = iface_cast<IStopDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleStopRelayTypeRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StopRemoteInput(srcId, sinkId, inputTypes, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStopRelayTypeRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandlePrepareRelayRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    sptr<IPrepareDInputCallback> callback = iface_cast<IPrepareDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandlePrepareRelayRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = PrepareRemoteInput(srcId, sinkId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandlePrepareRelayRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnprepareRelayRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    std::string srcId = data.ReadString();
    std::string sinkId = data.ReadString();
    sptr<IUnprepareDInputCallback> callback = iface_cast<IUnprepareDInputCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleUnprepareRelayRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = UnprepareRemoteInput(srcId, sinkId, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnprepareRelayRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
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
    if (callback == nullptr) {
        DHLOGE("HandleStartDhidRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StartRemoteInput(sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStartDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
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
    if (callback == nullptr) {
        DHLOGE("HandleStopDhidRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StopRemoteInput(sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStopDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStartRelayDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
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
    if (callback == nullptr) {
        DHLOGE("HandleStartRelayDhidRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StartRemoteInput(srcId, sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStartRelayDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleStopRelayDhidRemoteInput(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
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
    if (callback == nullptr) {
        DHLOGE("HandleStopRelayDhidRemoteInput failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = StopRemoteInput(srcId, sinkId, tempVector, callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleStopRelayDhidRemoteInput write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterAddWhiteListCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IAddWhiteListInfosCallback> callback = iface_cast<IAddWhiteListInfosCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleRegisterAddWhiteListCallback failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterAddWhiteListCallback(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterAddWhiteListCallback write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterDelWhiteListCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IDelWhiteListInfosCallback> callback = iface_cast<IDelWhiteListInfosCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleRegisterDelWhiteListCallback failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterDelWhiteListCallback(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterDelWhiteListCallback write ret failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterSimulationEventListener(MessageParcel &data, MessageParcel &reply)
{
    sptr<ISimulationEventListener> callback = iface_cast<ISimulationEventListener>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleRegisterSimulationEventListener failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterSimulationEventListener(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterSimulationEventListener write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_SOURCE_STUB_REGISTER_SIMULATION_EVENT_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnregisterSimulationEventListener(MessageParcel &data, MessageParcel &reply)
{
    sptr<ISimulationEventListener> callback = iface_cast<ISimulationEventListener>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleUnregisterSimulationEventListener failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = UnregisterSimulationEventListener(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnregisterSimulationEventListener write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_SOURCE_STUB_UNREGISTER_SIMULATION_EVENT_LISTENER_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleRegisterSessionStateCb(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    sptr<ISessionStateCallback> callback = iface_cast<ISessionStateCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        DHLOGE("HandleRegisterSessionStateCb failed, callback is nullptr.");
        return ERR_DH_INPUT_POINTER_NULL;
    }
    int32_t ret = RegisterSessionStateCb(callback);
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleRegisterSessionStateCb write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_SRC_STUB_REGISTER_SESSION_STATE_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::HandleUnregisterSessionStateCb(MessageParcel &data, MessageParcel &reply)
{
    if (!HasAccessDHPermission()) {
        DHLOGE("The caller has no ACCESS_DISTRIBUTED_HARDWARE permission.");
        return ERR_DH_INPUT_SRC_ACCESS_PERMISSION_CHECK_FAIL;
    }
    int32_t ret = UnregisterSessionStateCb();
    if (!reply.WriteInt32(ret)) {
        DHLOGE("HandleUnregisterSessionStateCb write ret failed, ret = %{public}d", ret);
        return ERR_DH_INPUT_SRC_STUB_UNREGISTER_SESSION_STATE_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DHLOGI("OnRemoteRequest code: %{public}u.", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("DistributedInputSourceStub read token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (code == static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_REMOTE_INPUT)) {
        DHLOGI("Receive Register DInput cmd");
    }

    if (code == static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_REMOTE_INPUT)) {
        DHLOGI("Receive UnRegister DInput cmd");
    }
    if (code == static_cast<uint32_t>(IDInputSourceInterfaceCode::INIT)) {
        return HandleInitDistributedHardware(reply);
    } else if (code == static_cast<uint32_t>(IDInputSourceInterfaceCode::RELEASE)) {
        return HandleReleaseDistributedHardware(reply);
    } else {
        auto iter = memberFuncMap_.find(code);
        if (iter != memberFuncMap_.end()) {
            const DistributedInputSourceFunc &func = iter->second;
            return (this->*func)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
