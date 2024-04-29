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

#include "distributedinputstub_fuzzer.h"

#include "add_white_list_infos_call_back_stub.h"
#include "del_white_list_infos_call_back_stub.h"
#include "get_sink_screen_infos_call_back_stub.h"
#include "input_node_listener_stub.h"
#include "prepare_d_input_call_back_stub.h"
#include "register_d_input_call_back_stub.h"
#include "register_session_state_callback_stub.h"
#include "sharing_dhid_listener_stub.h"
#include "simulation_event_listener_stub.h"
#include "start_d_input_call_back_stub.h"
#include "start_stop_d_inputs_call_back_stub.h"
#include "start_stop_result_call_back_stub.h"
#include "stop_d_input_call_back_stub.h"
#include "unprepare_d_input_call_back_stub.h"
#include "unregister_d_input_call_back_stub.h"

namespace OHOS {
namespace DistributedHardware {

class MyAddWhiteListInfosCallbackStub : public DistributedInput::AddWhiteListInfosCallbackStub {
public:
    void OnResult(const std::string &deviceId, const std::string &strJson) override
    {
        return;
    }
};

void AddWhiteListInfosCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    pdata.WriteInterfaceToken(DistributedInput::AddWhiteListInfosCallbackStub::GetDescriptor());
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    if (code == (uint32_t)DistributedInput::IAddWhiteListInfosCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::AddWhiteListInfosCallbackStub> addWhiteListInfos(new (std::nothrow)
        MyAddWhiteListInfosCallbackStub());
    addWhiteListInfos->OnRemoteRequest(code, pdata, reply, option);
}

class MyDelWhiteListInfosCallbackStub : public DistributedInput::DelWhiteListInfosCallbackStub {
public:
    void OnResult(const std::string &deviceId) override
    {
        return;
    }
};

void DelWhiteListInfosCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::DelWhiteListInfosCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IDelWhiteListInfosCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }

    sptr<DistributedInput::DelWhiteListInfosCallbackStub> delWhiteListInfos(new (std::nothrow)
        MyDelWhiteListInfosCallbackStub());
    delWhiteListInfos->OnRemoteRequest(code, pdata, reply, option);
}

int32_t DistributedInputSinkStubFuzz::Init()
{
    return DistributedInput::DH_SUCCESS;
}

int32_t DistributedInputSinkStubFuzz::Release()
{
    return DistributedInput::DH_SUCCESS;
}

int32_t DistributedInputSinkStubFuzz::RegisterGetSinkScreenInfosCallback(
    sptr<DistributedInput::IGetSinkScreenInfosCallback> callback)
{
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DistributedInputSinkStubFuzz::NotifyStartDScreen(
    const DistributedInput::SrcScreenInfo &remoteCtrlInfo)
{
    (void)remoteCtrlInfo;
    return DistributedInput::DH_SUCCESS;
}

int32_t DistributedInputSinkStubFuzz::NotifyStopDScreen(
    const std::string &srcScreenInfoKey)
{
    (void)srcScreenInfoKey;
    return DistributedInput::DH_SUCCESS;
}

int32_t DistributedInputSinkStubFuzz::RegisterSharingDhIdListener(
    sptr<DistributedInput::ISharingDhIdListener> sharingDhIdListener)
{
    (void)sharingDhIdListener;
    return DistributedInput::DH_SUCCESS;
}

void DistributedInputSinkStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    pdata.WriteInterfaceToken(DistributedInput::DistributedInputSinkStub::GetDescriptor());
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t type = *(reinterpret_cast<const uint32_t*>(data));
    std::string content(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    pdata.WriteUint32(type);
    pdata.WriteString(content);
    pdata.WriteString(devId);

    sptr<DistributedInput::DistributedInputSinkStub> distributedInputSinkStub(new (std::nothrow)
        DistributedInputSinkStubFuzz());
    distributedInputSinkStub->HasEnableDHPermission();
    distributedInputSinkStub->OnRemoteRequest(code, pdata, reply, option);
    distributedInputSinkStub->InitInner(pdata, reply, option);
    distributedInputSinkStub->ReleaseInner(pdata, reply, option);
    distributedInputSinkStub->NotifyStartDScreenInner(pdata, reply, option);
    distributedInputSinkStub->NotifyStopDScreenInner(pdata, reply, option);
    distributedInputSinkStub->RegisterSharingDhIdListenerInner(pdata, reply, option);
    distributedInputSinkStub->RegisterGetSinkScreenInfosInner(pdata, reply, option);
}

int32_t DInputSourceCallBackStubFuzz::Init()
{
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::Release()
{
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::RegisterDistributedHardware(
    const std::string &devId, const std::string &dhId, const std::string &parameters,
    sptr<DistributedInput::IRegisterDInputCallback> callback)
{
    (void)devId;
    (void)dhId;
    (void)parameters;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::UnregisterDistributedHardware(
    const std::string &devId, const std::string &dhId,
    sptr<DistributedInput::IUnregisterDInputCallback> callback)
{
    (void)devId;
    (void)dhId;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::PrepareRemoteInput(
    const std::string &deviceId, sptr<DistributedInput::IPrepareDInputCallback> callback)
{
    (void)deviceId;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::UnprepareRemoteInput(
    const std::string &deviceId, sptr<DistributedInput::IUnprepareDInputCallback> callback)
{
    (void)deviceId;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StartRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<DistributedInput::IStartDInputCallback> callback)
{
    (void)deviceId;
    (void)inputTypes;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StopRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<DistributedInput::IStopDInputCallback> callback)
{
    (void)deviceId;
    (void)inputTypes;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StartRemoteInput(
    const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
    sptr<DistributedInput::IStartDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)inputTypes;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StopRemoteInput(
    const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
    sptr<DistributedInput::IStopDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)inputTypes;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::PrepareRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    sptr<DistributedInput::IPrepareDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::UnprepareRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    sptr<DistributedInput::IUnprepareDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StartRemoteInput(
    const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<DistributedInput::IStartStopDInputsCallback> callback)
{
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StopRemoteInput(
    const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<DistributedInput::IStartStopDInputsCallback> callback)
{
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StartRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<DistributedInput::IStartStopDInputsCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::StopRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<DistributedInput::IStartStopDInputsCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::RegisterAddWhiteListCallback(
    sptr<DistributedInput::IAddWhiteListInfosCallback> addWhiteListCallback)
{
    (void)addWhiteListCallback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::RegisterDelWhiteListCallback(
    sptr<DistributedInput::IDelWhiteListInfosCallback> delWhiteListCallback)
{
    (void)delWhiteListCallback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::RegisterSimulationEventListener(
    sptr<DistributedInput::ISimulationEventListener> listener)
{
    (void)listener;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::UnregisterSimulationEventListener(
    sptr<DistributedInput::ISimulationEventListener> listener)
{
    (void)listener;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::RegisterSessionStateCb(
    sptr<DistributedInput::ISessionStateCallback> callback)
{
    (void)callback;
    return DistributedInput::DH_SUCCESS;
}

int32_t DInputSourceCallBackStubFuzz::UnregisterSessionStateCb()
{
    return DistributedInput::DH_SUCCESS;
}

void DInputSourceCallBackStubFuzz::OnResult(const std::string &deviceId,
    const std::string &strJson)
{
    deviceId_ = deviceId;
    strJson_ = strJson;
}

void DistributedInputSourceStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    pdata.WriteInterfaceToken(DistributedInput::DistributedInputSourceStub::GetDescriptor());
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t type = *(reinterpret_cast<const uint32_t*>(data));
    std::string content(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    pdata.WriteUint32(type);
    pdata.WriteString(content);
    pdata.WriteString(devId);

    sptr<DistributedInput::DistributedInputSourceStub> distributedInputSourceStub(new (std::nothrow)
        DInputSourceCallBackStubFuzz());
    distributedInputSourceStub->RegRespFunMap();
    distributedInputSourceStub->HasEnableDHPermission();
    distributedInputSourceStub->HasAccessDHPermission();
    distributedInputSourceStub->HandleInitDistributedHardware(reply);
    distributedInputSourceStub->HandleReleaseDistributedHardware(reply);
    distributedInputSourceStub->HandleRegisterDistributedHardware(pdata, reply);
    distributedInputSourceStub->HandleUnregisterDistributedHardware(pdata, reply);
    distributedInputSourceStub->HandlePrepareRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleUnprepareRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStartRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStopRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStartRelayTypeRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStopRelayTypeRemoteInput(pdata, reply);
    distributedInputSourceStub->HandlePrepareRelayRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleUnprepareRelayRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStartDhidRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStopDhidRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStartRelayDhidRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleStopRelayDhidRemoteInput(pdata, reply);
    distributedInputSourceStub->HandleRegisterAddWhiteListCallback(pdata, reply);
    distributedInputSourceStub->HandleRegisterDelWhiteListCallback(pdata, reply);
    distributedInputSourceStub->HandleRegisterSimulationEventListener(pdata, reply);
    distributedInputSourceStub->HandleUnregisterSimulationEventListener(pdata, reply);
    distributedInputSourceStub->HandleRegisterSessionStateCb(pdata, reply);
    distributedInputSourceStub->HandleUnregisterSessionStateCb(pdata, reply);
    distributedInputSourceStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyGetSinkScreenInfosCallbackStub : public DistributedInput::GetSinkScreenInfosCallbackStub {
public:
    void OnResult(const std::string &deviceId) override
    {
        return;
    }
};

void GetSinkScreenInfosCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::GetSinkScreenInfosCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IGetSinkScreenInfosCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }

    sptr<DistributedInput::GetSinkScreenInfosCallbackStub> getSinkScreenInfosCbStub(new (std::nothrow)
        MyGetSinkScreenInfosCallbackStub());
    getSinkScreenInfosCbStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyInputNodeListenerStub : public DistributedInput::InputNodeListenerStub {
public:
    void OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId,
        const std::string &sinkNodeId, const std::string &sinkNodeDesc) override
    {
        (void)srcDevId;
        (void)sinkDevId;
        (void)sinkNodeId;
        (void)sinkNodeDesc;
        return;
    }

    void OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId,
        const std::string &sinkNodeId) override
    {
        (void)srcDevId;
        (void)sinkDevId;
        (void)sinkNodeId;
        return;
    }
};

void InputNodeListenerStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::InputNodeListenerStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::InputNodeListener::Message::RESULT_ON) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else if (code == (uint32_t)DistributedInput::InputNodeListener::Message::RESULT_OFF) {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    } else {
        uint32_t type = *(reinterpret_cast<const uint32_t*>(data));
        std::string content(reinterpret_cast<const char*>(data), size);
        std::string devId(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(type);
        pdata.WriteString(content);
        pdata.WriteString(devId);
    }

    sptr<DistributedInput::InputNodeListenerStub> inputNodeListenerStub(new (std::nothrow)
        MyInputNodeListenerStub());
    inputNodeListenerStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyPrepareDInputCallbackStub : public DistributedInput::PrepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override
    {
        return;
    }
};

void PrepareDInputCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::PrepareDInputCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IPrepareDInputCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::PrepareDInputCallbackStub> prepareDInputCallbackStub(new (std::nothrow)
        MyPrepareDInputCallbackStub());
    prepareDInputCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyRegisterDInputCallbackStub : public DistributedInput::RegisterDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status) override
    {
        return;
    }
};

void RegisterDInputCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::RegisterDInputCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IRegisterDInputCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::RegisterDInputCallbackStub> registerDInputCallbackStub(new (std::nothrow)
        MyRegisterDInputCallbackStub());
    registerDInputCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyRegisterSessionStateCallbackStub : public DistributedInput::RegisterSessionStateCallbackStub {
public:
    void OnResult(const std::string &devId, const uint32_t status) override
    {
        return;
    }
};

void RegisterSessionStateCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::RegisterSessionStateCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::ISessionStateCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::RegisterSessionStateCallbackStub> registerSessionStateCbkStub(new (std::nothrow)
        MyRegisterSessionStateCallbackStub());
    registerSessionStateCbkStub->OnRemoteRequest(code, pdata, reply, option);
}

class MySharingDhIdListenerStub : public DistributedInput::SharingDhIdListenerStub {
public:
    int32_t OnSharing(const std::string &dhId) override
    {
        (void)dhId;
        return 0;
    }

    int32_t OnNoSharing(const std::string &dhId) override
    {
        (void)dhId;
        return 0;
    }
};

void SharingDhIdListenerStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::SharingDhIdListenerStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::ISharingDhIdListener::Message::SHARING) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else if (code == (uint32_t)DistributedInput::ISharingDhIdListener::Message::NO_SHARING) {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    } else {
        uint32_t type = *(reinterpret_cast<const uint32_t*>(data));
        std::string content(reinterpret_cast<const char*>(data), size);
        std::string devId(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(type);
        pdata.WriteString(content);
        pdata.WriteString(devId);
    }
    sptr<DistributedInput::SharingDhIdListenerStub> sharingDhIdListenerStub(new (std::nothrow)
        MySharingDhIdListenerStub());
    sharingDhIdListenerStub->OnRemoteRequest(code, pdata, reply, option);
}

class MySimulationEventListenerStub : public DistributedInput::SimulationEventListenerStub {
public:
    int32_t OnSimulationEvent(uint32_t type, uint32_t code, int32_t value) override
    {
        return 0;
    }
};

void SimulationEventListenerStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::SimulationEventListenerStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::ISimulationEventListener::Message::RESULT_ON) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::SimulationEventListenerStub> simulationEventListenerStub(new (std::nothrow)
        MySimulationEventListenerStub());
    simulationEventListenerStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyStartDInputCallbackStub : public DistributedInput::StartDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status) override
    {
        return;
    }
};

void StartDInputCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::StartDInputCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IStartDInputCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::StartDInputCallbackStub> startDInputCallbackStub(new (std::nothrow)
        MyStartDInputCallbackStub());
    startDInputCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyStartStopDInputsCallbackStub : public DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override
    {
        return;
    }
};

void StartStopDInputsCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::StartStopDInputsCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IStartStopDInputsCallback::Message::RESULT_STRING) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::StartStopDInputsCallbackStub> startStopDInputsCallbackStub(new (std::nothrow)
        MyStartStopDInputsCallbackStub());
    startStopDInputsCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyStartStopResultCallbackStub : public DistributedInput::StartStopResultCallbackStub {
public:
    void OnStart(const std::string &srcId, const std::string &sinkId,
        std::vector<std::string> &dhIds) override
    {
        return;
    }
    void OnStop(const std::string &srcId, const std::string &sinkId,
        std::vector<std::string> &dhIds) override
    {
        return;
    }
};

void StartStopResultCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::StartStopResultCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IStartStopResultCallback::Message::RESULT_START) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else if (code == (uint32_t)DistributedInput::IStartStopResultCallback::Message::RESULT_STOP) {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    } else {
        uint32_t type = *(reinterpret_cast<const uint32_t*>(data));
        std::string content(reinterpret_cast<const char*>(data), size);
        std::string devId(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(type);
        pdata.WriteString(content);
        pdata.WriteString(devId);
    }
    sptr<DistributedInput::StartStopResultCallbackStub> startStopResultCallbackStub(new (std::nothrow)
        MyStartStopResultCallbackStub());
    startStopResultCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyStopDInputCallbackStub : public DistributedInput::StopDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status) override
    {
        return;
    }
};

void StopDInputCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::StopDInputCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IStopDInputCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::StopDInputCallbackStub> stopDInputCallbackStub(new (std::nothrow)
        MyStopDInputCallbackStub());
    stopDInputCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyUnprepareDInputCallbackStub : public DistributedInput::UnprepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override
    {
        return;
    }
};

void UnprepareDInputCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::UnprepareDInputCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IUnprepareDInputCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::UnprepareDInputCallbackStub> unprepareDInputCallbackStub(new (std::nothrow)
        MyUnprepareDInputCallbackStub());
    unprepareDInputCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}

class MyUnregisterDInputCallbackStub : public DistributedInput::UnregisterDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status) override
    {
        return;
    }
};

void UnregisterDInputCallbackStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    pdata.WriteInterfaceToken(DistributedInput::UnregisterDInputCallbackStub::GetDescriptor());
    if (code == (uint32_t)DistributedInput::IUnprepareDInputCallback::Message::RESULT) {
        uint32_t tag = *(reinterpret_cast<const uint32_t*>(data));
        std::string value(reinterpret_cast<const char*>(data), size);
        pdata.WriteUint32(tag);
        pdata.WriteString(value);
    } else {
        int32_t fd = *(reinterpret_cast<const int32_t*>(data));
        int32_t len = *(reinterpret_cast<const int32_t*>(data));
        std::string name(reinterpret_cast<const char*>(data), size);
        pdata.WriteFileDescriptor(fd);
        pdata.WriteInt32(len);
        pdata.WriteString(name);
    }
    sptr<DistributedInput::UnregisterDInputCallbackStub> unregisterDInputCallbackStub(new (std::nothrow)
        MyUnregisterDInputCallbackStub());
    unregisterDInputCallbackStub->OnRemoteRequest(code, pdata, reply, option);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::AddWhiteListInfosCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::DelWhiteListInfosCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::DistributedInputSinkStubFuzzTest(data, size);
    OHOS::DistributedHardware::DistributedInputSourceStubFuzzTest(data, size);
    OHOS::DistributedHardware::GetSinkScreenInfosCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::InputNodeListenerStubFuzzTest(data, size);
    OHOS::DistributedHardware::PrepareDInputCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::RegisterDInputCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::RegisterSessionStateCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::SharingDhIdListenerStubFuzzTest(data, size);
    OHOS::DistributedHardware::SimulationEventListenerStubFuzzTest(data, size);
    OHOS::DistributedHardware::StartDInputCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::StartStopDInputsCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::StartStopResultCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::StopDInputCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::UnprepareDInputCallbackStubFuzzTest(data, size);
    OHOS::DistributedHardware::UnregisterDInputCallbackStubFuzzTest(data, size);
    return 0;
}