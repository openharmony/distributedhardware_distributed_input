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

#include "distributed_input_sink_transport.h"

#include <cinttypes>

#include "linux/input.h"

#include "distributed_hardware_fwk_kit.h"
#include "securec.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_softbus_define.h"
#include "dinput_utils_tool.h"
#include "hidumper.h"
#include "softbus_bus_center.h"
#include "xcollie/watchdog.h"

#include "distributed_input_transport_base.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    // each time, we send msg batch with MAX 20 events.
    constexpr int32_t MSG_BTACH_MAX_SIZE = 20;
}
DistributedInputSinkTransport::DistributedInputSinkTransport() : mySessionName_("")
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    eventHandler_ = std::make_shared<DistributedInputSinkTransport::DInputSinkEventHandler>(runner);

    if (OHOS::HiviewDFX::Watchdog::GetInstance().AddThread("dinputwatchdog", eventHandler_,
        WATCHDOG_INTERVAL_TIME_MS)) {
        DHLOGE("HiviewDFX::Watchdog::GetInstance().AddThread() Failed.");
    }
    DHLOGI("DistributedInputSinkTransport ctor.");
}

DistributedInputSinkTransport::~DistributedInputSinkTransport()
{
    DHLOGI("DistributedInputSinkTransport dtor.");
}

DistributedInputSinkTransport::DInputSinkEventHandler::DInputSinkEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner) : AppExecFwk::EventHandler(runner)
{
}

DistributedInputSinkTransport &DistributedInputSinkTransport::GetInstance()
{
    static DistributedInputSinkTransport instance;
    return instance;
}

void DistributedInputSinkTransport::DInputSinkEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    EHandlerMsgType eventId = static_cast<EHandlerMsgType>(event->GetInnerEventId());
    switch (eventId) {
        case EHandlerMsgType::DINPUT_SINK_EVENT_HANDLER_MSG: {
            std::shared_ptr<nlohmann::json> innerMsg = event->GetSharedObject<nlohmann::json>();
            nlohmann::json jsonStr;
            jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_BODY_DATA;
            jsonStr[DINPUT_SOFTBUS_KEY_INPUT_DATA] = innerMsg->dump();
            std::string smsg = jsonStr.dump();
            RecordEventLog(innerMsg);
            int32_t sessionId = DistributedInputSinkSwitch::GetInstance().GetSwitchOpenedSession();
            if (sessionId > 0) {
                DistributedInputSinkTransport::GetInstance().SendMessage(sessionId, smsg);
            } else {
                DHLOGE("ProcessEvent can't send input data, because no session switch on.");
            }
            break;
        }
        default:
            DHLOGE("ProcessEvent error, because eventId is unkonwn.");
            break;
    }
}

int32_t DistributedInputSinkTransport::Init()
{
    DHLOGI("Init");

    int32_t ret = DistributedInputTransportBase::GetInstance().Init();
    if (ret != DH_SUCCESS) {
        DHLOGE("Init Sink Transport Failed");
        return ret;
    }

    statuslistener_ = std::make_shared<DInputTransbaseSinkListener>(this);
    DistributedInputTransportBase::GetInstance().RegisterSinkHandleSessionCallback(statuslistener_);
    RegRespFunMap();
    return DH_SUCCESS;
}

std::shared_ptr<DistributedInputSinkTransport::DInputSinkEventHandler> DistributedInputSinkTransport::GetEventHandler()
{
    DHLOGI("GetEventHandler");
    return eventHandler_;
}

void DistributedInputSinkTransport::RegistSinkRespCallback(std::shared_ptr<DInputSinkTransCallback> callback)
{
    DHLOGI("RegistSinkRespCallback");
    callback_ = callback;
}

int32_t DistributedInputSinkTransport::RespPrepareRemoteInput(
    const int32_t sessionId, std::string &smsg)
{
    if (sessionId > 0) {
        DHLOGI("RespPrepareRemoteInput sessionId: %{public}d, smsg:%{public}s.", sessionId, SetAnonyId(smsg).c_str());
        int32_t ret = SendMessage(sessionId, smsg);
        if (ret != DH_SUCCESS) {
            DHLOGE("RespPrepareRemoteInput error, SendMessage fail.");
            return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPPREPARE_FAIL;
        }
        return DH_SUCCESS;
    } else {
        DHLOGE("RespPrepareRemoteInput error, sessionId <= 0.");
        return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPPREPARE_FAIL;
    }
}

int32_t DistributedInputSinkTransport::RespUnprepareRemoteInput(const int32_t sessionId, std::string &smsg)
{
    if (sessionId > 0) {
        DHLOGI("RespUnprepareRemoteInput sessionId: %{public}d, smsg:%{public}s.", sessionId, SetAnonyId(smsg).c_str());
        int32_t ret = SendMessage(sessionId, smsg);
        if (ret != DH_SUCCESS) {
            DHLOGE("RespUnprepareRemoteInput error, SendMessage fail.");
            return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPUNPREPARE_FAIL;
        }
        return DH_SUCCESS;
    } else {
        DHLOGE("RespUnprepareRemoteInput error, sessionId <= 0.");
        return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPUNPREPARE_FAIL;
    }
}

int32_t DistributedInputSinkTransport::RespStartRemoteInput(const int32_t sessionId, std::string &smsg)
{
    if (sessionId > 0) {
        DHLOGI("RespStartRemoteInput sessionId: %{public}d, smsg:%{public}s.", sessionId, SetAnonyId(smsg).c_str());
        int32_t ret = SendMessage(sessionId, smsg);
        if (ret != DH_SUCCESS) {
            DHLOGE("RespStartRemoteInput error, SendMessage fail.");
            return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPSTART_FAIL;
        }
        return DH_SUCCESS;
    } else {
        DHLOGE("RespStartRemoteInput error, sessionId <= 0.");
        return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPSTART_FAIL;
    }
}

int32_t DistributedInputSinkTransport::RespStopRemoteInput(const int32_t sessionId, std::string &smsg)
{
    if (sessionId > 0) {
        DHLOGI("RespStopRemoteInput sessionId: %{public}d, smsg:%{public}s.", sessionId, SetAnonyId(smsg).c_str());
        int32_t ret = SendMessage(sessionId, smsg);
        if (ret != DH_SUCCESS) {
            DHLOGE("RespStopRemoteInput error, SendMessage fail.");
            return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPSTOP_FAIL;
        }
        return DH_SUCCESS;
    } else {
        DHLOGE("RespStopRemoteInput error, sessionId <= 0.");
        return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESPSTOP_FAIL;
    }
}

int32_t DistributedInputSinkTransport::RespLatency(const int32_t sessionId, std::string &smsg)
{
    if (sessionId <= 0) {
        DHLOGE("RespLatency error, sessionId <= 0.");
        return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESP_LATENCY_FAIL;
    }

    int32_t ret = SendMessage(sessionId, smsg);
    if (ret != DH_SUCCESS) {
        DHLOGE("RespLatency error, SendMessage fail.");
        return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_RESP_LATENCY_FAIL;
    }

    return DH_SUCCESS;
}

void DistributedInputSinkTransport::SendKeyStateNodeMsg(const int32_t sessionId, const std::string &dhId,
    uint32_t type, const uint32_t btnCode, int32_t value)
{
    if (sessionId <= 0) {
        DHLOGE("SendKeyStateNodeMsg error, sessionId <= 0.");
        return;
    }
    DHLOGI("SendKeyStateNodeMsg sessionId: %{public}d, btnCode: %{public}u.", sessionId, btnCode);
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_KEY_STATE;
    jsonStr[DINPUT_SOFTBUS_KEY_KEYSTATE_DHID] = dhId;
    jsonStr[DINPUT_SOFTBUS_KEY_KEYSTATE_TYPE] = type;
    jsonStr[DINPUT_SOFTBUS_KEY_KEYSTATE_CODE] = btnCode;
    jsonStr[DINPUT_SOFTBUS_KEY_KEYSTATE_VALUE] = value;
    std::string msg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, msg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendKeyStateNodeMsg error, SendMessage fail.");
    }
    RecordEventLog(dhId, type, btnCode, value);
}

void DistributedInputSinkTransport::SendKeyStateNodeMsgBatch(const int32_t sessionId,
    const std::vector<struct RawEvent> &events)
{
    if (sessionId <= 0) {
        DHLOGE("SendKeyStateNodeMsgBatch error, sessionId <= 0.");
        return;
    }
    DHLOGI("SendKeyStateNodeMsgBatch sessionId: %{public}d, event size: %{public}zu ", sessionId, events.size());

    int32_t cnt = 0;
    std::vector<struct RawEvent> eventBatch;
    for (auto ev : events) {
        eventBatch.push_back(ev);
        cnt++;
        if (cnt == MSG_BTACH_MAX_SIZE) {
            DoSendMsgBatch(sessionId, eventBatch);
            eventBatch.clear();
            cnt = 0;
        }
    }

    if (!eventBatch.empty()) {
        DoSendMsgBatch(sessionId, eventBatch);
    }
}

void DistributedInputSinkTransport::DoSendMsgBatch(const int32_t sessionId, const std::vector<struct RawEvent> &events)
{
    int64_t currentTimeNs = static_cast<int64_t>(GetCurrentTimeUs()) * 1000LL;
    std::shared_ptr<nlohmann::json> eventsJsonArr = std::make_shared<nlohmann::json>();
    for (const auto &ev : events) {
        nlohmann::json tmpJson;
        tmpJson[INPUT_KEY_WHEN] = currentTimeNs;
        tmpJson[INPUT_KEY_TYPE] = ev.type;
        tmpJson[INPUT_KEY_CODE] = ev.code;
        tmpJson[INPUT_KEY_VALUE] = ev.value;
        tmpJson[INPUT_KEY_DESCRIPTOR] = ev.descriptor;
        tmpJson[INPUT_KEY_PATH] = ev.path;
        eventsJsonArr->push_back(tmpJson);
    }

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_KEY_STATE_BATCH;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_DATA] = eventsJsonArr->dump();
    std::string msg = jsonStr.dump();
    int32_t ret = SendMessage(sessionId, msg);
    if (ret != DH_SUCCESS) {
        DHLOGE("SendKeyStateNodeMsgBatch error, SendMessage fail.");
    }
    RecordEventLog(events);
}

void DistributedInputSinkTransport::RecordEventLog(const std::vector<struct RawEvent> &events)
{
    for (auto &ev : events) {
        RecordEventLog(ev.descriptor, ev.type, ev.code, ev.value);
    }
}

void DistributedInputSinkTransport::RecordEventLog(const std::string &dhId, int32_t type, int32_t code, int32_t value)
{
    std::string eventType;
    switch (type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_REL:
            eventType = "EV_REL";
            break;
        case EV_ABS:
            eventType = "EV_ABS";
            break;
        default:
            eventType = "other type " + std::to_string(type);
            break;
    }

    DHLOGD("2.E2E-Test Sink softBus send, EventType: %{public}s, Code: %{public}d, Value: %{public}d, dhId: %{public}s",
        eventType.c_str(), code, value, GetAnonyString(dhId).c_str());
}

int32_t DistributedInputSinkTransport::SendMessage(int32_t sessionId, std::string &message)
{
    return DistributedInputTransportBase::GetInstance().SendMsg(sessionId, message);
}

DistributedInputSinkTransport::DInputTransbaseSinkListener::DInputTransbaseSinkListener(
    DistributedInputSinkTransport *transport)
{
    sinkTransportObj_ = transport;
    DHLOGI("DInputTransbaseSinkListener init.");
}

DistributedInputSinkTransport::DInputTransbaseSinkListener::~DInputTransbaseSinkListener()
{
    sinkTransportObj_ = nullptr;
    DHLOGI("DInputTransbaseSinkListener destory.");
}

void DistributedInputSinkTransport::NotifyPrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_PREPARE.");
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_PREPARE deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnPrepareRemoteInput(sessionId, deviceId);
}

void DistributedInputSinkTransport::NotifyUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    DHLOGI("OnBytesReceived cmdType TRANS_SOURCE_MSG_UNPREPARE deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnUnprepareRemoteInput(sessionId);
}

void DistributedInputSinkTransport::NotifyStartRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    DHLOGI("OnBytesRecei,ved cmdType is TRANS_SOURCE_MSG_START_TYPE deviceId:%{public}s inputTypes:%{public}d .",
        GetAnonyString(deviceId).c_str(), inputTypes);
    callback_->OnStartRemoteInput(sessionId, inputTypes);
}

void DistributedInputSinkTransport::NotifyStopRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_STOP_TYPE deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnStopRemoteInput(sessionId, inputTypes);
}

void DistributedInputSinkTransport::NotifyLatency(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_LATENCY;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    std::string smsg = jsonStr.dump();
    RespLatency(sessionId, smsg);
}

void DistributedInputSinkTransport::NotifyStartRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    std::string strTmp = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_START_DHID deviceId:%{public}s .",
           GetAnonyString(deviceId).c_str());
    callback_->OnStartRemoteInputDhid(sessionId, strTmp);
}

void DistributedInputSinkTransport::NotifyStopRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    std::string strTmp = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    DHLOGE("OnBytesReceived cmdType is TRANS_SOURCE_MSG_STOP_DHID deviceId:%{public}s.",
           GetAnonyString(deviceId).c_str());
    callback_->OnStopRemoteInputDhid(sessionId, strTmp);
}

void DistributedInputSinkTransport::NotifyRelayPrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_PREPARE_FOR_REL deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnRelayPrepareRemoteInput(toSrcSessionId, sessionId, deviceId);
}

void DistributedInputSinkTransport::NotifyRelayUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_UNPREPARE_FOR_REL deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnRelayUnprepareRemoteInput(toSrcSessionId, sessionId, deviceId);
}

void DistributedInputSinkTransport::NotifyRelayStartDhidRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    std::string dhids = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_START_DHID_FOR_REL deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnRelayStartDhidRemoteInput(toSrcSessionId, sessionId, deviceId, dhids);
}

void DistributedInputSinkTransport::NotifyRelayStopDhidRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsString(recMsg, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    std::string dhids = recMsg[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_STOP_DHID_FOR_REL deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnRelayStopDhidRemoteInput(toSrcSessionId, sessionId, deviceId, dhids);
}

void DistributedInputSinkTransport::NotifyRelayStartTypeRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_START_TYPE_FOR_REL deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnRelayStartTypeRemoteInput(toSrcSessionId, sessionId, deviceId, inputTypes);
}

void DistributedInputSinkTransport::NotifyRelayStopTypeRemoteInput(int32_t sessionId, const nlohmann::json &recMsg)
{
    if (!IsString(recMsg, DINPUT_SOFTBUS_KEY_DEVICE_ID) ||
        !IsInt32(recMsg, DINPUT_SOFTBUS_KEY_SESSION_ID) ||
        !IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_INPUT_TYPE)) {
        DHLOGE("The key is invaild.");
        return;
    }
    std::string deviceId = recMsg[DINPUT_SOFTBUS_KEY_DEVICE_ID];
    int32_t toSrcSessionId = recMsg[DINPUT_SOFTBUS_KEY_SESSION_ID];
    uint32_t inputTypes = recMsg[DINPUT_SOFTBUS_KEY_INPUT_TYPE];
    DHLOGI("OnBytesReceived cmdType is TRANS_SOURCE_MSG_STOP_TYPE_FOR_REL deviceId:%{public}s.",
        GetAnonyString(deviceId).c_str());
    callback_->OnRelayStopTypeRemoteInput(toSrcSessionId, sessionId, deviceId, inputTypes);
}

void DistributedInputSinkTransport::DInputTransbaseSinkListener::NotifySessionClosed(int32_t sessionId)
{
    DistributedInputSinkSwitch::GetInstance().RemoveSession(sessionId);
}

void DistributedInputSinkTransport::DInputTransbaseSinkListener::HandleSessionData(int32_t sessionId,
    const std::string &message)
{
    DistributedInputSinkTransport::GetInstance().HandleData(sessionId, message);
}

void DistributedInputSinkTransport::RegRespFunMap()
{
    memberFuncMap_[TRANS_SOURCE_MSG_PREPARE] = &DistributedInputSinkTransport::NotifyPrepareRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_UNPREPARE] = &DistributedInputSinkTransport::NotifyUnprepareRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_START_TYPE] = &DistributedInputSinkTransport::NotifyStartRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_STOP_TYPE] = &DistributedInputSinkTransport::NotifyStopRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_LATENCY] = &DistributedInputSinkTransport::NotifyLatency;
    memberFuncMap_[TRANS_SOURCE_MSG_START_DHID] = &DistributedInputSinkTransport::NotifyStartRemoteInputDhid;
    memberFuncMap_[TRANS_SOURCE_MSG_STOP_DHID] = &DistributedInputSinkTransport::NotifyStopRemoteInputDhid;
    memberFuncMap_[TRANS_SOURCE_MSG_PREPARE_FOR_REL] = &DistributedInputSinkTransport::NotifyRelayPrepareRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_UNPREPARE_FOR_REL] =
        &DistributedInputSinkTransport::NotifyRelayUnprepareRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_START_DHID_FOR_REL] =
        &DistributedInputSinkTransport::NotifyRelayStartDhidRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_STOP_DHID_FOR_REL] =
        &DistributedInputSinkTransport::NotifyRelayStopDhidRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_START_TYPE_FOR_REL] =
        &DistributedInputSinkTransport::NotifyRelayStartTypeRemoteInput;
    memberFuncMap_[TRANS_SOURCE_MSG_STOP_TYPE_FOR_REL] =
        &DistributedInputSinkTransport::NotifyRelayStopTypeRemoteInput;
}

void DistributedInputSinkTransport::HandleData(int32_t sessionId, const std::string &message)
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
    SinkTransportFunc &func = iter->second;
    (this->*func)(sessionId, recMsg);
}

void DistributedInputSinkTransport::CloseAllSession()
{
    DistributedInputTransportBase::GetInstance().StopAllSession();
    // clear session data
    DistributedInputSinkSwitch::GetInstance().InitSwitch();
}

void DistributedInputSinkTransport::DInputSinkEventHandler::RecordEventLog(
    const std::shared_ptr<nlohmann::json> &events)
{
    for (nlohmann::json::const_iterator iter = events->cbegin(); iter != events->cend(); ++iter) {
        nlohmann::json event = *iter;
        if (!IsInt32(event, INPUT_KEY_TYPE) || !IsInt64(event, INPUT_KEY_WHEN) ||
            !IsUInt32(event, INPUT_KEY_CODE) || !IsInt32(event, INPUT_KEY_VALUE) ||
            !IsString(event, INPUT_KEY_PATH)) {
            DHLOGE("The key is invaild.");
            continue;
        }
        std::string eventType = "";
        int32_t evType = event[INPUT_KEY_TYPE];
        switch (evType) {
            case EV_KEY:
                eventType = "EV_KEY";
                break;
            case EV_REL:
                eventType = "EV_REL";
                break;
            case EV_ABS:
                eventType = "EV_ABS";
                break;
            default:
                eventType = "other type " + std::to_string(evType);
                break;
        }
        int64_t when = event[INPUT_KEY_WHEN];
        int32_t code = event[INPUT_KEY_CODE];
        int32_t value = event[INPUT_KEY_VALUE];
        std::string path = event[INPUT_KEY_PATH];
        DHLOGD("2.E2E-Test Sink softBus send, EventType: %{public}s, Code: %{public}d, Value: %{public}d, "
            "Path: %{public}s, When: %{public}" PRId64 "", eventType.c_str(), code, value, path.c_str(), when);
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
