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

#include "distributed_input_source_proxy.h"

#include "dinput_errcode.h"
#include "dinput_ipc_interface_code.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputSourceProxy::DistributedInputSourceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IDistributedSourceInput>(object)
{}

DistributedInputSourceProxy::~DistributedInputSourceProxy()
{}

int32_t DistributedInputSourceProxy::Init()
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_INIT_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::INIT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_RELEASE_FAIL;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::RELEASE), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::string &parameters, sptr<IRegisterDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(devId)) {
        DHLOGE("DistributedInputSourceProxy write devId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(dhId)) {
        DHLOGE("DistributedInputSourceProxy write dhId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(parameters)) {
        DHLOGE("DistributedInputSourceProxy write parameters failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_REGISTER_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
    sptr<IUnregisterDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(devId)) {
        DHLOGE("DistributedInputSourceProxy write devId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(dhId)) {
        DHLOGE("DistributedInputSourceProxy write dhId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_UNREGISTER_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::PrepareRemoteInput(
    const std::string &deviceId, sptr<IPrepareDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(deviceId)) {
        DHLOGE("DistributedInputSourceProxy write deviceId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_PREPARE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::PREPARE_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("PrepareRemoteInput end, result:%{public}d", result);
    return result;
}

int32_t DistributedInputSourceProxy::UnprepareRemoteInput(const std::string &deviceId,
    sptr<IUnprepareDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(deviceId)) {
        DHLOGE("DistributedInputSourceProxy write deviceId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_UNPREPARE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::UNPREPARE_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGE("UnprepareRemoteInput end, result=%{public}d", result);
    return result;
}

int32_t DistributedInputSourceProxy::StartRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    DHLOGI("source proxy StartRemoteInput start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(deviceId)) {
        DHLOGE("DistributedInputSourceProxy write deviceId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteUint32(inputTypes)) {
        DHLOGE("DistributedInputSourceProxy write inputTypes failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_START_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::START_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("source proxy StartRemoteInput end, result:%{public}d.", result);
    return result;
}

int32_t DistributedInputSourceProxy::StopRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(deviceId)) {
        DHLOGE("DistributedInputSourceProxy write deviceId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteUint32(inputTypes)) {
        DHLOGE("DistributedInputSourceProxy write inputTypes failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_STOP_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    DHLOGI("source proxy StartRemoteInput start");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("DistributedInputSourceProxy write srcId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("DistributedInputSourceProxy write sinkId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteUint32(inputTypes)) {
        DHLOGE("DistributedInputSourceProxy write inputTypes failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_START_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::START_RELAY_TYPE_REMOTE_INPUT), data,
        reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("source proxy StartRemoteInput end, result:%{public}d.", result);
    return result;
}

int32_t DistributedInputSourceProxy::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("DistributedInputSourceProxy write srcId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("DistributedInputSourceProxy write sinkId failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteUint32(inputTypes)) {
        DHLOGE("DistributedInputSourceProxy write inputTypes failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_STOP_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_RELAY_TYPE_REMOTE_INPUT),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IPrepareDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("Write PrepareRemoteInput relay srcid to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("Write PrepareRemoteInput relay sinkid to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write PrepareRemoteInput relay callback to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_PREPARE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::PREPARE_RELAY_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("PrepareRemoteInput relay result:%{public}d", result);
    return result;
}

int32_t DistributedInputSourceProxy::UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IUnprepareDInputCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("Write UnprepareRemoteInput relay srcid to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("Write UnprepareRemoteInput relay sinkid to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write UnprepareRemoteInput relay callback to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_UNPREPARE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::UNPREPARE_RELAY_REMOTE_INPUT),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("UnprepareRemoteInput relay result:%{public}d", result);
    return result;
}

int32_t DistributedInputSourceProxy::StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("Write StartRemoteInput dhid sinkId to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    if (!data.WriteUint32(dhIds.size())) {
        DHLOGE("Write StartRemoteInput dhid size to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    for (auto it = dhIds.begin(); it != dhIds.end(); ++it) {
        if (!data.WriteString(*it)) {
            DHLOGE("Write StartRemoteInput dhid dhid to parcel failed");
            return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
        }
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write StartRemoteInput dhid callback to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_START_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::START_DHID_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("Source proxy StartRemoteInput dhid end, result:%{public}d.", result);
    return result;
}

int32_t DistributedInputSourceProxy::StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("Write StopRemoteInput dhid sinkId to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    if (!data.WriteUint32(dhIds.size())) {
        DHLOGE("Write StopRemoteInput dhid size to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    for (auto it = dhIds.begin(); it != dhIds.end(); ++it) {
        if (!data.WriteString(*it)) {
            DHLOGE("Write StopRemoteInput dhid dhid to parcel failed");
            return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
        }
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write StopRemoteInput dhid callback to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_STOP_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_DHID_REMOTE_INPUT), data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("Source proxy StopRemoteInput dhid end, result:%{public}d.", result);
    return result;
}

int32_t DistributedInputSourceProxy::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy relay dhid write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("Write StartRemoteInput relay dhid srcId to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("Write StartRemoteInput relay dhid sinkId to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    if (!data.WriteUint32(dhIds.size())) {
        DHLOGE("Write StartRemoteInput relay dhid size to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    for (auto it = dhIds.begin(); it != dhIds.end(); ++it) {
        if (!data.WriteString(*it)) {
            DHLOGE("Write StartRemoteInput relay dhid dhid to parcel failed");
            return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
        }
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write StartRemoteInput relay dhid callback to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_START_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::START_RELAY_DHID_REMOTE_INPUT),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("Source proxy StartRemoteInput relay dhid end, result:%{public}d.", result);
    return result;
}

int32_t DistributedInputSourceProxy::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("Write StopRemoteInput relay dhid srcId to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("Write StopRemoteInput relay dhid sinkId to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    if (!data.WriteUint32(dhIds.size())) {
        DHLOGE("Write StopRemoteInput relay dhid size to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    for (auto it = dhIds.begin(); it != dhIds.end(); ++it) {
        if (!data.WriteString(*it)) {
            DHLOGE("Write StopRemoteInput relay dhid dhid to parcel failed");
            return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
        }
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write StopRemoteInput relay dhid callback to parcel failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_STOP_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::STOP_RELAY_DHID_REMOTE_INPUT),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    DHLOGI("Source proxy StopRemoteInput relay dhid end, result:%{public}d.", result);
    return result;
}

int32_t DistributedInputSourceProxy::RegisterAddWhiteListCallback(sptr<IAddWhiteListInfosCallback> addWhiteListCallback)
    __attribute__((no_sanitize("cfi")))
{
    if (addWhiteListCallback == nullptr) {
        DHLOGE("DistributedInputSourceProxy addWhiteListCallback is null.");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(addWhiteListCallback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_REGISTER_WHITELIST_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_ADD_WHITE_LIST_CB_REMOTE_INPUT),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::RegisterDelWhiteListCallback(sptr<IDelWhiteListInfosCallback> delWhiteListCallback)
{
    if (delWhiteListCallback == nullptr) {
        DHLOGE("DistributedInputSourceProxy delWhiteListCallback is null.");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("DistributedInputSourceProxy write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }
    if (!data.WriteRemoteObject(delWhiteListCallback->AsObject())) {
        DHLOGE("DistributedInputSourceProxy write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }
    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_REGISTER_WHITELIST_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_DEL_WHITE_LIST_CB_REMOTE_INPUT),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::RegisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("RegisterSimulationEventListener write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }

    if (!data.WriteRemoteObject(listener->AsObject())) {
        DHLOGE("RegisterSimulationEventListener write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_REGISTER_SIMULATION_LISTENER_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_SIMULATION_EVENT_LISTENER),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("UnregisterSimulationEventListener write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }

    if (!data.WriteRemoteObject(listener->AsObject())) {
        DHLOGE("UnregisterSimulationEventListener write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SOURCE_PROXY_UNREGISTER_SIMULATION_LISTENER_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_SIMULATION_EVENT_LISTENER),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::RegisterSessionStateCb(sptr<ISessionStateCallback> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("RegisterSessionStateCb write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }

    if (!data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("RegisterSessionStateCb write callback failed");
        return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SRC_STUB_REGISTER_SESSION_STATE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::REGISTER_SESSION_STATE_CB),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

int32_t DistributedInputSourceProxy::UnregisterSessionStateCb()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("UnregisterSessionStateCb write token valid failed");
        return ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL;
    }

    MessageParcel reply;
    int32_t result = ERR_DH_INPUT_SRC_STUB_UNREGISTER_SESSION_STATE_FAIL;
    bool ret = SendRequest(static_cast<uint32_t>(IDInputSourceInterfaceCode::UNREGISTER_SESSION_STATE_CB),
        data, reply);
    if (ret) {
        result = reply.ReadInt32();
    }
    return result;
}

bool DistributedInputSourceProxy::SendRequest(const uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DistributedInputSourceProxy SendRequest remote is null.");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int32_t result = remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
    if (result != DH_SUCCESS) {
        DHLOGE("DistributedInputSourceProxy SendRequest error, result:%{public}d.", result);
        return false;
    }
    return true;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
