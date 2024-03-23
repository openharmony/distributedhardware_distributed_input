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

#include "distributed_input_source_transport.h"

#include <cstring>
#include <pthread.h>

#include "distributed_hardware_fwk_kit.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_hitrace.h"
#include "dinput_log.h"
#include "dinput_softbus_define.h"
#include "dinput_utils_tool.h"
#include "distributed_input_inject.h"
#include "hidumper.h"

#include "distributed_input_transport_base.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    const uint64_t MSG_LATENCY_ALARM_US = 20 * 1000;
}
DistributedInputSourceTransport::~DistributedInputSourceTransport()
{
    DHLOGI("Dtor DistributedInputSourceTransport");
    Release();
}

DistributedInputSourceTransport &DistributedInputSourceTransport::GetInstance()
{
    static DistributedInputSourceTransport instance;
    return instance;
}

int32_t DistributedInputSourceTransport::Init()
{
    DHLOGI("Init");

    int32_t ret = DistributedInputTransportBase::GetInstance().Init();
    if (ret != DH_SUCCESS) {
        DHLOGE("Init Source Transport failed.");
        return ret;
    }

    statuslistener_ = std::make_shared<DInputTransbaseSourceListener>(this);
    DistributedInputTransportBase::GetInstance().RegisterSrcHandleSessionCallback(statuslistener_);
    RegRespFunMap();
    return DH_SUCCESS;
}

void DistributedInputSourceTransport::RegRespFunMap()
{
    memberFuncMap_[TRANS_SINK_MSG_ONPREPARE] = &DistributedInputSourceTransport::NotifyResponsePrepareRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_ONUNPREPARE] = &DistributedInputSourceTransport::NotifyResponseUnprepareRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_ONSTART] = &DistributedInputSourceTransport::NotifyResponseStartRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_ONSTOP] = &DistributedInputSourceTransport::NotifyResponseStopRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_BODY_DATA] = &DistributedInputSourceTransport::NotifyReceivedEventRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_LATENCY] = &DistributedInputSourceTransport::CalculateLatency;
    memberFuncMap_[TRANS_SINK_MSG_DHID_ONSTART] = &DistributedInputSourceTransport::NotifyResponseStartRemoteInputDhid;
    memberFuncMap_[TRANS_SINK_MSG_DHID_ONSTOP] = &DistributedInputSourceTransport::NotifyResponseStopRemoteInputDhid;
    memberFuncMap_[TRANS_SINK_MSG_KEY_STATE] = &DistributedInputSourceTransport::NotifyResponseKeyState;
    memberFuncMap_[TRANS_SINK_MSG_KEY_STATE_BATCH] = &DistributedInputSourceTransport::NotifyResponseKeyStateBatch;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_PREPARE] = &DistributedInputSourceTransport::ReceiveSrcTSrcRelayPrepare;
    memberFuncMap_[TRANS_SINK_MSG_ON_RELAY_PREPARE] =
        &DistributedInputSourceTransport::NotifyResponseRelayPrepareRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_ON_RELAY_UNPREPARE] =
        &DistributedInputSourceTransport::NotifyResponseRelayUnprepareRemoteInput;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE] =
        &DistributedInputSourceTransport::ReceiveSrcTSrcRelayUnprepare;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_PREPARE_RESULT] =
        &DistributedInputSourceTransport::ReceiveRelayPrepareResult;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE_RESULT] =
        &DistributedInputSourceTransport::ReceiveRelayUnprepareResult;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_START_DHID] =
        &DistributedInputSourceTransport::ReceiveSrcTSrcRelayStartDhid;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID] =
        &DistributedInputSourceTransport::ReceiveSrcTSrcRelayStopDhid;
    memberFuncMap_[TRANS_SINK_MSG_ON_RELAY_STARTDHID] =
        &DistributedInputSourceTransport::NotifyResponseRelayStartDhidRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_ON_RELAY_STOPDHID] =
        &DistributedInputSourceTransport::NotifyResponseRelayStopDhidRemoteInput;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_START_DHID_RESULT] =
        &DistributedInputSourceTransport::ReceiveRelayStartDhidResult;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID_RESULT] =
        &DistributedInputSourceTransport::ReceiveRelayStopDhidResult;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE] =
        &DistributedInputSourceTransport::ReceiveSrcTSrcRelayStartType;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE] =
        &DistributedInputSourceTransport::ReceiveSrcTSrcRelayStopType;
    memberFuncMap_[TRANS_SINK_MSG_ON_RELAY_STARTTYPE] =
        &DistributedInputSourceTransport::NotifyResponseRelayStartTypeRemoteInput;
    memberFuncMap_[TRANS_SINK_MSG_ON_RELAY_STOPTYPE] =
        &DistributedInputSourceTransport::NotifyResponseRelayStopTypeRemoteInput;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE_RESULT] =
        &DistributedInputSourceTransport::ReceiveRelayStartTypeResult;
    memberFuncMap_[TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE_RESULT] =
        &DistributedInputSourceTransport::ReceiveRelayStopTypeResult;
}

void DistributedInputSourceTransport::Release()
{
    DHLOGI("Release Source Transport");
    if (injectThreadNum > 0) {
        DHLOGI("InjectThread stopped.");
        DistributedInputInject::GetInstance().StopInjectThread();
        injectThreadNum = 0;
    }
    if (latencyThreadNum > 0) {
        DHLOGI("LatencyThread stopped.");
        StopLatencyThread();
        latencyThreadNum = 0;
    }
    return;
}

int32_t DistributedInputSourceTransport::OpenInputSoftbus(const std::string &remoteDevId, bool isToSrc)
{
    int32_t ret = DistributedInputTransportBase::GetInstance().StartSession(remoteDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("StartSession fail! remoteDevId:%{public}s.", GetAnonyString(remoteDevId).c_str());
        return ret;
    }

    if (isToSrc) {
        return DH_SUCCESS;
    }

    if (latencyThreadNum == 0) {
        StartLatencyThread(remoteDevId);
        DHLOGI("LatencyThread started, remoteDevId: %{public}s.", GetAnonyString(remoteDevId).c_str());
    } else {
        DHLOGI("LatencyThread already started.");
    }
    latencyThreadNum++;

    if (injectThreadNum == 0) {
        DistributedInputInject::GetInstance().StartInjectThread();
        DHLOGI("InjectThread started.");
    } else {
        DHLOGI("InjectThread already started.");
    }
    injectThreadNum++;

    return DH_SUCCESS;
}

void DistributedInputSourceTransport::CloseInputSoftbus(const std::string &remoteDevId, bool isToSrc)
{
    DistributedInputTransportBase::GetInstance().StopSession(remoteDevId);

    if (isToSrc) {
        return;
    }

    SessionClosed();
    return;
}

void DistributedInputSourceTransport::RegisterSourceRespCallback(std::shared_ptr<DInputSourceTransCallback> callback)
{
    DHLOGI("RegisterSourceRespCallback");
    callback_ = callback;
}

/*
 * PrepareRemoteInput.
 * @param  deviceId is remote device
 * @return Returns 0 is success, other is fail.
 */
int32_t DistributedInputSourceTransport::PrepareRemoteInput(const std::string &deviceId)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("PrepareRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("PrepareRemoteInput sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_PREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("PrepareRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("PrepareRemoteInput devId:%{public}s, sessionId:%{public}d, msg:%{public}s.",
        GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::UnprepareRemoteInput(const std::string &deviceId)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("UnprepareRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_UNPREPARE_FAIL;
    }
    DHLOGI("UnprepareRemoteInput sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_UNPREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("UnprepareRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_UNPREPARE_FAIL;
    }
    DHLOGI("UnprepareRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::PrepareRemoteInput(int32_t srcTsrcSeId, const std::string &deviceId)
{
    int32_t sinkSessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sinkSessionId < 0) {
        DHLOGE("PrepareRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }

    DHLOGI("PrepareRemoteInput srcTsrcSeId:%{public}d, sinkSessionId:%{public}d.", srcTsrcSeId, sinkSessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_PREPARE_FOR_REL;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = srcTsrcSeId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sinkSessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("PrepareRemoteInput deviceId:%{public}s, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("PrepareRemoteInput send success, devId:%{public}s, msg:%{public}s.",
        GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}
int32_t DistributedInputSourceTransport::UnprepareRemoteInput(int32_t srcTsrcSeId, const std::string &deviceId)
{
    int32_t sinkSessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sinkSessionId < 0) {
        DHLOGE("UnprepareRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_UNPREPARE_FAIL;
    }
    DHLOGI("UnprepareRemoteInput srcTsrcSeId:%{public}d, sinkSessionId:%{public}d.", srcTsrcSeId, sinkSessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_UNPREPARE_FOR_REL;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = srcTsrcSeId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sinkSessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("UnprepareRemoteInput deviceId:%{public}s, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_UNPREPARE_FAIL;
    }
    DHLOGI("UnprepareRemoteInput send success, devId:%{public}s, msg:%{public}s.",
        GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::StartRemoteInputDhids(int32_t srcTsrcSeId, const std::string &deviceId,
    const std::string &dhids)
{
    int32_t sinkSessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sinkSessionId < 0) {
        DHLOGE("StartRemoteInputDhids error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInputDhids srcTsrcSeId:%{public}d, sinkSessionId:%{public}d.", srcTsrcSeId, sinkSessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_START_DHID_FOR_REL;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = srcTsrcSeId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = dhids;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sinkSessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StartRemoteInputDhids deviceId:%{public}s, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInputDhids send success, devId:%{public}s, msg:%{public}s.", GetAnonyString(deviceId).c_str(),
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::StopRemoteInputDhids(int32_t srcTsrcSeId, const std::string &deviceId,
    const std::string &dhids)
{
    int32_t sinkSessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sinkSessionId < 0) {
        DHLOGE("StopRemoteInputDhids error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInputDhids srcTsrcSeId:%{public}d, sinkSessionId:%{public}d.", srcTsrcSeId, sinkSessionId);
    std::vector<std::string> dhIdsVec = SplitDhIdString(dhids);
    ResetKeyboardKeyState(deviceId, dhIdsVec);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_STOP_DHID_FOR_REL;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = srcTsrcSeId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = dhids;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sinkSessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StopRemoteInputDhids deviceId:%{public}s, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInputDhids send success, devId:%{public}s, msg:%{public}s.", GetAnonyString(deviceId).c_str(),
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::StartRemoteInputType(int32_t srcTsrcSeId, const std::string &deviceId,
    const uint32_t &inputTypes)
{
    int32_t sinkSessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sinkSessionId < 0) {
        DHLOGE("StartRemoteInputType error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInputType srcTsrcSeId:%{public}d, sinkSessionId:%{public}d.", srcTsrcSeId, sinkSessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_START_TYPE_FOR_REL;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = srcTsrcSeId;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sinkSessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StartRemoteInputType deviceId:%{public}s, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInputType send success, devId:%{public}s, msg:%{public}s.", GetAnonyString(deviceId).c_str(),
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::StopRemoteInputType(int32_t srcTsrcSeId, const std::string &deviceId,
    const uint32_t &inputTypes)
{
    int32_t sinkSessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sinkSessionId < 0) {
        DHLOGE("StopRemoteInputType error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInputType srcTsrcSeId:%{public}d, sinkSessionId:%{public}d.", srcTsrcSeId, sinkSessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_STOP_TYPE_FOR_REL;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = srcTsrcSeId;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sinkSessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StopRemoteInputType deviceId:%{public}s, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            GetAnonyString(deviceId).c_str(), SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInputType send success, devId:%{public}s, msg:%{public}s.", GetAnonyString(deviceId).c_str(),
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendRelayPrepareRequest(const std::string &srcId, const std::string &sinkId)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(srcId);
    if (sessionId < 0) {
        DHLOGE("SendRelayPrepareRequest error, not find this device:%{public}s.", GetAnonyString(srcId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("SendRelayPrepareRequest sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_PREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = sinkId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendRelayPrepareRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("SendRelayPrepareRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendRelayUnprepareRequest(const std::string &srcId, const std::string &sinkId)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(srcId);
    if (sessionId < 0) {
        DHLOGE("SendRelayUnprepareRequest error, not find this device:%{public}s.", GetAnonyString(srcId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("SendRelayUnprepareRequest sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = sinkId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendRelayUnprepareRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("SendRelayUnprepareRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::NotifyOriginPrepareResult(int32_t srcTsrcSeId, const std::string &srcId,
    const std::string &sinkId, int32_t status)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_PREPARE_RESULT;
    jsonStr[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcId;
    jsonStr[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = status;

    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(srcTsrcSeId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyOriginPrepareResult srcTsrcSeId:%{public}d, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            srcTsrcSeId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PREPARE_FAIL;
    }
    DHLOGI("NotifyOriginPrepareResult srcTsrcSeId:%{public}d, smsg:%{public}s.", srcTsrcSeId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::NotifyOriginUnprepareResult(int32_t srcTsrcSeId, const std::string &srcId,
    const std::string &sinkId, int32_t status)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE_RESULT;
    jsonStr[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcId;
    jsonStr[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = status;

    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(srcTsrcSeId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyOriginUnprepareResult srcTsrcSeId:%{public}d, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            srcTsrcSeId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_UNPREPARE_FAIL;
    }
    DHLOGI("NotifyOriginUnprepareResult srcTsrcSeId:%{public}d, smsg:%{public}s.", srcTsrcSeId,
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::NotifyOriginStartDhidResult(int32_t srcTsrcSeId, const std::string &srcId,
    const std::string &sinkId, int32_t status, const std::string &dhids)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_START_DHID_RESULT;
    jsonStr[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcId;
    jsonStr[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = status;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = dhids;

    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(srcTsrcSeId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyOriginStartDhidResult srcTsrcSeId:%{public}d, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            srcTsrcSeId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("NotifyOriginStartDhidResult srcTsrcSeId:%{public}d, smsg:%{public}s.", srcTsrcSeId,
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::NotifyOriginStopDhidResult(int32_t srcTsrcSeId, const std::string &srcId,
    const std::string &sinkId, int32_t status, const std::string &dhids)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID_RESULT;
    jsonStr[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcId;
    jsonStr[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = status;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = dhids;

    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(srcTsrcSeId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyOriginStopDhidResult srcTsrcSeId:%{public}d, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            srcTsrcSeId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("NotifyOriginStopDhidResult srcTsrcSeId:%{public}d, smsg:%{public}s.", srcTsrcSeId,
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::NotifyOriginStartTypeResult(int32_t srcTsrcSeId, const std::string &srcId,
    const std::string &sinkId, int32_t status, uint32_t inputTypes)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE_RESULT;
    jsonStr[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcId;
    jsonStr[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = status;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;

    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(srcTsrcSeId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyOriginStartTypeResult srcTsrcSeId:%{public}d, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            srcTsrcSeId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("NotifyOriginStartTypeResult srcTsrcSeId:%{public}d, smsg:%{public}s.", srcTsrcSeId,
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::NotifyOriginStopTypeResult(int32_t srcTsrcSeId, const std::string &srcId,
    const std::string &sinkId, int32_t status, uint32_t inputTypes)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE_RESULT;
    jsonStr[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcId;
    jsonStr[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = status;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;

    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(srcTsrcSeId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyOriginStopTypeResult srcTsrcSeId:%{public}d, smsg:%{public}s, SendMsg error, ret:%{public}d.",
            srcTsrcSeId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("NotifyOriginStopTypeResult srcTsrcSeId:%{public}d, smsg:%{public}s.", srcTsrcSeId,
        SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::StartRemoteInput(const std::string &deviceId, const uint32_t &inputTypes)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("StartRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInput sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_START_TYPE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StartRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::StopRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("StopRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInput sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_STOP_TYPE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StopRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInput deviceId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::LatencyCount(const std::string &deviceId)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("LatencyCount error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_LATENCY_FAIL;
    }

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_LATENCY;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("LatencyCount deviceId:%{public}s, sessionId: %{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_LATENCY_FAIL;
    }

    return DH_SUCCESS;
}

void DistributedInputSourceTransport::StartLatencyCount(const std::string &deviceId)
{
    int32_t ret = pthread_setname_np(pthread_self(), LATENCY_COUNT_THREAD_NAME);
    if (ret != 0) {
        DHLOGE("StartLatencyCount setname failed.");
    }
    while (isLatencyThreadRunning_.load()) {
        if (sendNum_ >= INPUT_LATENCY_DELAY_TIMES) {
            uint64_t latency = (uint64_t)(deltaTimeAll_ / 2 / INPUT_LATENCY_DELAY_TIMES);
            DHLOGI("LatencyCount average single-channel latency is %{public}" PRIu64 " us, send times is %{public}d, "
                "recive times is %{public}d.", latency, sendNum_, recvNum_);
            DHLOGD("each RTT latency details is %{public}s", eachLatencyDetails_.c_str());
            deltaTimeAll_ = 0;
            sendNum_ = 0;
            recvNum_ = 0;
            eachLatencyDetails_ = "";
        }
        sendTime_ = GetCurrentTimeUs();
        LatencyCount(deviceId);
        sendNum_ += 1;
        usleep(INPUT_LATENCY_DELAYTIME_US);
    }
}

void DistributedInputSourceTransport::StartLatencyThread(const std::string &deviceId)
{
    DHLOGI("start");
    isLatencyThreadRunning_.store(true);
    latencyThread_ = std::thread(&DistributedInputSourceTransport::StartLatencyCount, this, deviceId);
    DHLOGI("end");
}

void DistributedInputSourceTransport::StopLatencyThread()
{
    DHLOGI("start");
    isLatencyThreadRunning_.store(false);
    if (latencyThread_.joinable()) {
        latencyThread_.join();
    }
    DHLOGI("end");
}

void DistributedInputSourceTransport::DInputTransbaseSourceListener::NotifySessionClosed()
{
    DistributedInputSourceTransport::GetInstance().SessionClosed();
}

void DistributedInputSourceTransport::SessionClosed()
{
    if (injectThreadNum == 0) {
        DHLOGI("InjectThread already stopped, or InjectThread not start.");
    } else if (injectThreadNum > 1) {
        injectThreadNum--;
    } else if (injectThreadNum == 1) {
        DHLOGI("InjectThread stopped.");
        DistributedInputInject::GetInstance().StopInjectThread();
        injectThreadNum--;
    }

    if (latencyThreadNum == 0) {
        DHLOGI("InjectThread already stopped, or InjectThread not start.");
    } else if (latencyThreadNum > 1) {
        latencyThreadNum--;
    } else if (latencyThreadNum == 1) {
        StopLatencyThread();
        DHLOGI("LatencyThread stopped.");
        latencyThreadNum--;
    }
}

int32_t DistributedInputSourceTransport::StartRemoteInput(const std::string &deviceId,
    const std::vector<std::string> &dhids)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("StartRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInput sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_START_DHID;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = JointDhIds(dhids);
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StartRemoteInput deviceId:%{public}s, sessionId: %{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("StartRemoteInput deviceId:%{public}s, sessionId: %{public}d, smsg:%{public}s.",
        GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

void DistributedInputSourceTransport::ResetKeyboardKeyState(const std::string &deviceId,
    const std::vector<std::string> &dhids)
{
    std::vector<std::string> keyboardNodePaths;
    DistributedInputInject::GetInstance().GetVirtualKeyboardPaths(deviceId, dhids, keyboardNodePaths);
    DHLOGI("Try reset keyboard states, dhIds: %{public}s, nodePaths: %{public}s",
        GetString(dhids).c_str(), GetString(keyboardNodePaths).c_str());
    ResetVirtualDevicePressedKeys(keyboardNodePaths);
}

int32_t DistributedInputSourceTransport::StopRemoteInput(const std::string &deviceId,
    const std::vector<std::string> &dhids)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(deviceId);
    if (sessionId < 0) {
        DHLOGE("StopRemoteInput error, not find this device:%{public}s.", GetAnonyString(deviceId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInput sessionId:%{public}d.", sessionId);
    ResetKeyboardKeyState(deviceId, dhids);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_STOP_DHID;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = sessionId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = JointDhIds(dhids);
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("StopRemoteInput deviceId:%{public}s, sessionId: %{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.",  GetAnonyString(deviceId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("StopRemoteInput deviceId:%{public}s, sessionId: %{public}d, smsg:%{public}s.",
        GetAnonyString(deviceId).c_str(),
        sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendRelayStartDhidRequest(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhids)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(srcId);
    if (sessionId < 0) {
        DHLOGE("SendRelayStartDhidRequest error, not find this device:%{public}s.", GetAnonyString(srcId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("SendRelayStartDhidRequest sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_START_DHID;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = JointDhIds(dhids);
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendRelayStartDhidRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("SendRelayStartDhidRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendRelayStopDhidRequest(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhids)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(srcId);
    if (sessionId < 0) {
        DHLOGE("SendRelayStopDhidRequest error, not find this device:%{public}s.", GetAnonyString(srcId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("SendRelayStopDhidRequest sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = JointDhIds(dhids);
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendRelayStopDhidRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("SendRelayStopDhidRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendRelayStartTypeRequest(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(srcId);
    if (sessionId < 0) {
        DHLOGE("SendRelayStartTypeRequest error, not find this device:%{public}s.", GetAnonyString(srcId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("SendRelayStartTypeRequest sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendRelayStartTypeRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_START_FAIL;
    }
    DHLOGI("SendRelayStartTypeRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendRelayStopTypeRequest(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes)
{
    int32_t sessionId = DistributedInputTransportBase::GetInstance().GetSessionIdByDevId(srcId);
    if (sessionId < 0) {
        DHLOGE("SendRelayStopTypeRequest error, not find this device:%{public}s.", GetAnonyString(srcId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("SendRelayStopTypeRequest sessionId:%{public}d.", sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE;
    jsonStr[DINPUT_SOFTBUS_KEY_DEVICE_ID] = sinkId;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    std::string smsg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendRelayStopTypeRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s, SendMsg error, "
            "ret:%{public}d.", GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str(), ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_STOP_FAIL;
    }
    DHLOGI("SendRelayStopTypeRequest srcId:%{public}s, sessionId:%{public}d, smsg:%{public}s.",
        GetAnonyString(srcId).c_str(), sessionId, SetAnonyId(smsg).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputSourceTransport::SendMessage(int32_t sessionId, std::string &message)
{
    return DistributedInputTransportBase::GetInstance().SendMsg(sessionId, message);
}

int32_t DistributedInputSourceTransport::GetCurrentSessionId()
{
    return DistributedInputTransportBase::GetInstance().GetCurrentSessionId();
}

DistributedInputSourceTransport::DInputTransbaseSourceListener::DInputTransbaseSourceListener(
    DistributedInputSourceTransport *transport)
{
    sourceTransportObj_ = transport;
    DHLOGI("DInputTransbaseSourceListener init.");
}

DistributedInputSourceTransport::DInputTransbaseSourceListener::~DInputTransbaseSourceListener()
{
    sourceTransportObj_ = nullptr;
    DHLOGI("DInputTransbaseSourceListener destory.");
}

void DistributedInputSourceTransport::NotifyResponsePrepareRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ONPREPARE.");
    if (!IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_WHITE_LIST)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONPREPARE, deviceId is error.");
        return;
    }
    callback_->OnResponsePrepareRemoteInput(deviceId, recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE],
        recMsg[DINPUT_SOFTBUS_KEY_WHITE_LIST]);
}

void DistributedInputSourceTransport::NotifyResponseUnprepareRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ONUNPREPARE.");
    if (!IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONUNPREPARE, deviceId is error.");
        return;
    }
    callback_->OnResponseUnprepareRemoteInput(deviceId, recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE]);
    CloseInputSoftbus(deviceId, false);
}

void DistributedInputSourceTransport::NotifyResponseStartRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ONSTART.");
    if (!IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONSTART, deviceId is error.");
        return;
    }
    callback_->OnResponseStartRemoteInput(
        deviceId, recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE], recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE]);
}

void DistributedInputSourceTransport::NotifyResponseStopRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ONSTOP.");
    if (!IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType TRANS_SINK_MSG_ONSTOP, deviceId is error.");
        return;
    }
    callback_->OnResponseStopRemoteInput(
        deviceId, recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE], recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE]);
}

void DistributedInputSourceTransport::NotifyResponseStartRemoteInputDhid(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_DHID_ONSTART.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_DHID_ONSTART, deviceId is error.");
        return;
    }
    callback_->OnResponseStartRemoteInputDhid(
        deviceId, recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID], recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE]);
}

void DistributedInputSourceTransport::NotifyResponseStopRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_DHID_ONSTOP.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_DHID_ONSTOP, deviceId is error.");
        return;
    }
    callback_->OnResponseStopRemoteInputDhid(
        deviceId, recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID], recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE]);
}

void DistributedInputSourceTransport::NotifyResponseKeyState(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_KEY_STATE.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_KEYSTATE_DHID) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_KEYSTATE_TYPE) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_KEYSTATE_CODE) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_KEYSTATE_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_KEY_STATE, deviceId is error.");
        return;
    }
    callback_->OnResponseKeyState(deviceId, recMsg[DINPUT_SOFTBUS_KEY_KEYSTATE_DHID],
        recMsg[DINPUT_SOFTBUS_KEY_KEYSTATE_TYPE], recMsg[DINPUT_SOFTBUS_KEY_KEYSTATE_CODE],
        recMsg[DINPUT_SOFTBUS_KEY_KEYSTATE_VALUE]);
}

void DistributedInputSourceTransport::NotifyResponseKeyStateBatch(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_KEY_STATE_BATCH.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_INPUT_DATA)) {
        DHLOGE("The DINPUT_SOFTBUS_KEY_INPUT_DATA is invalid");
        return;
    }
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_KEY_STATE_BATCH, deviceId is error.");
        return;
    }
    std::string inputDataStr = recMsg[DINPUT_SOFTBUS_KEY_INPUT_DATA];
    callback_->OnResponseKeyStateBatch(deviceId, inputDataStr);
}

void DistributedInputSourceTransport::NotifyReceivedEventRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGD("OnBytesReceived cmdType is TRANS_SINK_MSG_BODY_DATA.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_INPUT_DATA)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_BODY_DATA, deviceId is error.");
        return;
    }
    std::string inputDataStr = recMsg[DINPUT_SOFTBUS_KEY_INPUT_DATA];
    callback_->OnReceivedEventRemoteInput(deviceId, inputDataStr);
}

void DistributedInputSourceTransport::CalculateLatency(int32_t sessionId, const nlohmann::json &recMsg)
{
    std::string deviceId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (deviceId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_LATENCY, deviceId is error.");
        return;
    }

    uint64_t curTimeUs = GetCurrentTimeUs();
    if (curTimeUs <= sendTime_) {
        DHLOGE("Latency time error, currtime is before than send time, curTime: %{public}" PRIu64 " us, "
            "sendTime: %{public}" PRIu64 " us", curTimeUs, sendTime_);
        return;
    }

    deltaTime_ = curTimeUs - sendTime_;
    deltaTimeAll_ += deltaTime_;
    recvNum_ += 1;
    eachLatencyDetails_ += (std::to_string(deltaTime_) + DINPUT_SPLIT_COMMA);
    if (deltaTime_ >= MSG_LATENCY_ALARM_US) {
        DHLOGW("The RTT time between send req and receive rsp is too long: %{public}" PRIu64 " us", deltaTime_);
    }
}

void DistributedInputSourceTransport::ReceiveSrcTSrcRelayPrepare(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_PREPARE.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];

    // continue notify to A_sink_trans
    int32_t ret = OpenInputSoftbus(deviceId, false);
    if (ret != DH_SUCCESS) {
        callback_->OnResponseRelayPrepareRemoteInput(sessionId, deviceId, false, "");
        return;
    }

    ret = PrepareRemoteInput(sessionId, deviceId);
    if (ret != DH_SUCCESS) {
        callback_->OnResponseRelayPrepareRemoteInput(sessionId, deviceId, false, "");
        return;
    }
}

void DistributedInputSourceTransport::ReceiveSrcTSrcRelayUnprepare(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t ret = UnprepareRemoteInput(sessionId, deviceId);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, unprepare fail.");
        callback_->OnResponseRelayUnprepareRemoteInput(sessionId, deviceId, false);
        return;
    }
}

void DistributedInputSourceTransport::NotifyResponseRelayPrepareRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_PREPARE.");
    if (!IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_WHITE_LIST)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string sinkDevId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (sinkDevId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_PREPARE, sinkDevId is error.");
        return;
    }
    callback_->OnResponseRelayPrepareRemoteInput(recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID], sinkDevId,
        recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE], recMsg[DINPUT_SOFTBUS_KEY_WHITE_LIST]);
}

void DistributedInputSourceTransport::NotifyResponseRelayUnprepareRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_UNPREPARE.");
    if (!IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string sinkDevId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (sinkDevId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_UNPREPARE, sinkDevId is error.");
        return;
    }
    callback_->OnResponseRelayUnprepareRemoteInput(recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID], sinkDevId,
        recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE]);

    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    if (toSrcSessionId != sessionId) {
        DHLOGE("Close to sink session.");
        CloseInputSoftbus(sinkDevId, false);
    }
}

void DistributedInputSourceTransport::ReceiveRelayPrepareResult(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_PREPARE_RESULT.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_SRC_DEV_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SINK_DEV_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string srcId = recMsg[DINPUT_SOFTBUS_KEY_SRC_DEV_ID];
    std::string sinkId = recMsg[DINPUT_SOFTBUS_KEY_SINK_DEV_ID];
    int32_t status = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    callback_->OnReceiveRelayPrepareResult(status, srcId, sinkId);
}

void DistributedInputSourceTransport::ReceiveRelayUnprepareResult(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE_RESULT.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_SRC_DEV_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SINK_DEV_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string srcId = recMsg[DINPUT_SOFTBUS_KEY_SRC_DEV_ID];
    std::string sinkId = recMsg[DINPUT_SOFTBUS_KEY_SINK_DEV_ID];
    int32_t status = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    callback_->OnReceiveRelayUnprepareResult(status, srcId, sinkId);
    CloseInputSoftbus(srcId, true);
}

void DistributedInputSourceTransport::ReceiveSrcTSrcRelayStartDhid(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_START_DHID.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    std::string dhids =  recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    int32_t ret = StartRemoteInputDhids(sessionId, deviceId, dhids);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        std::string localNetworkId = GetLocalNetworkId();
        if (localNetworkId.empty()) {
            DHLOGE("Could not get local device id.");
            return;
        }
        NotifyOriginStartDhidResult(sessionId, localNetworkId, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_MSG_IS_BAD, dhids);
    }
}

void DistributedInputSourceTransport::ReceiveSrcTSrcRelayStopDhid(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    std::string dhids =  recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    int32_t ret = StopRemoteInputDhids(sessionId, deviceId, dhids);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        std::string localNetworkId = GetLocalNetworkId();
        if (localNetworkId.empty()) {
            DHLOGE("Could not get local device id.");
            return;
        }
        NotifyOriginStopDhidResult(sessionId, localNetworkId, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_MSG_IS_BAD, dhids);
    }
}

void DistributedInputSourceTransport::NotifyResponseRelayStartDhidRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STARTDHID.");
    if (!IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string sinkDevId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (sinkDevId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STARTDHID, sinkDevId is error.");
        return;
    }
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        DHLOGE("Could not get local device id.");
        return;
    }
    int32_t srcTsrcSeId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    bool result = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    std::string dhids = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    NotifyOriginStartDhidResult(srcTsrcSeId, localNetworkId, sinkDevId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_MSG_IS_BAD, dhids);
}

void DistributedInputSourceTransport::NotifyResponseRelayStopDhidRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STOPDHID.");
    if (!IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string sinkDevId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (sinkDevId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STOPDHID, sinkDevId is error.");
        return;
    }
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        DHLOGE("Could not get local device id.");
        return;
    }
    int32_t srcTsrcSeId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    bool result = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    std::string dhids = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    NotifyOriginStopDhidResult(srcTsrcSeId, localNetworkId, sinkDevId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_MSG_IS_BAD, dhids);
}

void DistributedInputSourceTransport::ReceiveRelayStartDhidResult(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_START_DHID_RESULT.");
    if (!IsInt32(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SRC_DEV_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SINK_DEV_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string srcId = recMsg[DINPUT_SOFTBUS_KEY_SRC_DEV_ID];
    std::string sinkId = recMsg[DINPUT_SOFTBUS_KEY_SINK_DEV_ID];
    int32_t status = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    std::string dhids = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    callback_->OnReceiveRelayStartDhidResult(status, srcId, sinkId, dhids);
}

void DistributedInputSourceTransport::ReceiveRelayStopDhidResult(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID_RESULT.");
    if (!IsInt32(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SRC_DEV_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SINK_DEV_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string srcId = recMsg[DINPUT_SOFTBUS_KEY_SRC_DEV_ID];
    std::string sinkId = recMsg[DINPUT_SOFTBUS_KEY_SINK_DEV_ID];
    int32_t status = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    std::string dhids = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    callback_->OnReceiveRelayStopDhidResult(status, srcId, sinkId, dhids);
}

void DistributedInputSourceTransport::ReceiveSrcTSrcRelayStartType(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t inputTypes =  recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    int32_t ret = StartRemoteInputType(sessionId, deviceId, inputTypes);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        std::string localNetworkId = GetLocalNetworkId();
        if (localNetworkId.empty()) {
            DHLOGE("Could not get local device id.");
            return;
        }
        NotifyOriginStartTypeResult(sessionId, localNetworkId, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_MSG_IS_BAD, inputTypes);
    }
}

void DistributedInputSourceTransport::ReceiveSrcTSrcRelayStopType(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t inputTypes =  recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    int32_t ret = StopRemoteInputType(sessionId, deviceId, inputTypes);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        std::string localNetworkId = GetLocalNetworkId();
        if (localNetworkId.empty()) {
            DHLOGE("Could not get local device id.");
            return;
        }
        NotifyOriginStopTypeResult(sessionId, localNetworkId, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_MSG_IS_BAD, inputTypes);
    }
}

void DistributedInputSourceTransport::NotifyResponseRelayStartTypeRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STARTTYPE.");
    if (!IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string sinkDevId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (sinkDevId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STARTTYPE, sinkDevId is error.");
        return;
    }
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        DHLOGE("Could not get local device id.");
        return;
    }
    int32_t srcTsrcSeId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    bool result = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    NotifyOriginStartTypeResult(srcTsrcSeId, localNetworkId, sinkDevId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_MSG_IS_BAD, inputTypes);
}

void DistributedInputSourceTransport::NotifyResponseRelayStopTypeRemoteInput(int32_t sessionId,
    const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STOPTYPE.");
    if (!IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsBoolean(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string sinkDevId = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    if (sinkDevId.empty()) {
        DHLOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ON_RELAY_STOPTYPE, sinkDevId is error.");
        return;
    }
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        DHLOGE("Could not get local device id.");
        return;
    }
    int32_t srcTsrcSeId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    bool result = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    NotifyOriginStopTypeResult(srcTsrcSeId, localNetworkId, sinkDevId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_MSG_IS_BAD, inputTypes);
}

void DistributedInputSourceTransport::ReceiveRelayStartTypeResult(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE_RESULT.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_SRC_DEV_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SINK_DEV_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string srcId = recMsg[DINPUT_SOFTBUS_KEY_SRC_DEV_ID];
    std::string sinkId = recMsg[DINPUT_SOFTBUS_KEY_SINK_DEV_ID];
    int32_t status = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    callback_->OnReceiveRelayStartTypeResult(status, srcId, sinkId, inputTypes);
}

void DistributedInputSourceTransport::ReceiveRelayStopTypeResult(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE_RESULT.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_SRC_DEV_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_SINK_DEV_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_RESP_VALUE) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }

    std::string srcId = recMsg[DINPUT_SOFTBUS_KEY_SRC_DEV_ID];
    std::string sinkId = recMsg[DINPUT_SOFTBUS_KEY_SINK_DEV_ID];
    int32_t status = recMsg[DINPUT_SOFTBUS_KEY_RESP_VALUE];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    callback_->OnReceiveRelayStopTypeResult(status, srcId, sinkId, inputTypes);
}

void DistributedInputSourceTransport::DInputTransbaseSourceListener::HandleSessionData(int32_t sessionId,
    const std::string &message)
{
    DistributedInputSourceTransport::GetInstance().HandleData(sessionId, message);
}

void DistributedInputSourceTransport::HandleData(int32_t sessionId, const std::string &message)
{
    if (callback_ == nullptr) {
        DHLOGE("OnBytesReceived the callback_ is null, the message:%{public}s abort.", SetAnonyId(message).c_str());
        return;
    }

    nlohmann::json recMsg = nlohmann::json::parse(message, nullptr, false);
    if (recMsg.is_discarded()) {
        DHLOGE("recMsg parse failed!");
        return;
    }
    if (!IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_CMD_TYPE)) {
        DHLOGE("softbus cmd key is invalid");
        return;
    }
    uint32_t cmdType = recMsg[DINPUT_SOFTBUS_KEY_CMD_TYPE];
    auto iter = memberFuncMap_.find(cmdType);
    if (iter == memberFuncMap_.end()) {
        DHLOGE("OnBytesReceived cmdType %{public}u is undefined.", cmdType);
        return;
    }
    SourceTransportFunc &func = iter->second;
    (this->*func)(sessionId, recMsg);
}

} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
