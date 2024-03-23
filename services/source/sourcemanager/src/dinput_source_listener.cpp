/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "dinput_source_listener.h"

#include <algorithm>
#include <cinttypes>
#include <dlfcn.h>

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "ipublisher_listener.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "dinput_softbus_define.h"
#include "distributed_input_inject.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DInputSourceListener::DInputSourceListener(DistributedInputSourceManager *manager)
{
    sourceManagerObj_ = manager;
    DHLOGI("DInputSourceListener init.");
}

DInputSourceListener::~DInputSourceListener()
{
    sourceManagerObj_ = nullptr;
    DHLOGI("DInputSourceListener destory.");
}

void DInputSourceListener::OnResponseRegisterDistributedHardware(
    const std::string deviceId, const std::string dhId, bool result)
{
    DHLOGI("OnResponseRegisterDistributedHardware called, deviceId: %{public}s, "
        "result: %{public}s.", GetAnonyString(deviceId).c_str(), result ? "success" : "failed");
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("OnResponseRegisterDistributedHardware sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        sourceManagerObj_->RunRegisterCallback(deviceId, dhId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL);
        DHLOGE("OnResponseRegisterDistributedHardware GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_HWID] = dhId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_RIGISTER_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponsePrepareRemoteInput(const std::string deviceId,
    bool result, const std::string &object)
{
    DHLOGI("OnResponsePrepareRemoteInput called, deviceId: %{public}s, result: %{public}s.",
        GetAnonyString(deviceId).c_str(), result ? "success" : "failed");

    if (sourceManagerObj_ == nullptr) {
        DHLOGE("OnResponsePrepareRemoteInput sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        sourceManagerObj_->RunPrepareCallback(deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL, object);
        DHLOGE("OnResponsePrepareRemoteInput GetCallbackEventHandler is null.");
        return;
    }
    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    tmpJson[INPUT_SOURCEMANAGER_KEY_WHITELIST] = object;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_PREPARE_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseUnprepareRemoteInput(const std::string deviceId, bool result)
{
    DHLOGI("OnResponseUnprepareRemoteInput called, deviceId: %{public}s, "
        "result: %{public}s.", GetAnonyString(deviceId).c_str(), result ? "success" : "failed");

    if (sourceManagerObj_ == nullptr) {
        DHLOGE("OnResponseUnprepareRemoteInput sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        sourceManagerObj_->RunUnprepareCallback(deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL);
        DHLOGE("OnResponseUnprepareRemoteInput GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_UNPREPARE_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseRelayPrepareRemoteInput(int32_t toSrcSessionId,
    const std::string &deviceId, bool result, const std::string &object)
{
    DHLOGI("OnResponseRelayPrepareRemoteInput deviceId: %{public}s, result: %{public}d.",
        GetAnonyString(deviceId).c_str(), result);
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    tmpJson[INPUT_SOURCEMANAGER_KEY_WHITELIST] = object;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SESSIONID] = toSrcSessionId;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_TO_ORIGIN, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseRelayUnprepareRemoteInput(int32_t toSrcSessionId,
    const std::string &deviceId, bool result)
{
    DHLOGI("OnResponseRelayUnprepareRemoteInput deviceId: %{public}s, result: %{public}d.",
        GetAnonyString(deviceId).c_str(), result);
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SESSIONID] = toSrcSessionId;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_TO_ORIGIN, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseStartRemoteInput(
    const std::string deviceId, const uint32_t inputTypes, bool result)
{
    DHLOGI("OnResponseStartRemoteInput called, deviceId: %{public}s, inputTypes: %{public}d, result: %{public}s.",
        GetAnonyString(deviceId).c_str(), inputTypes, result ? "success" : "failed");

    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        sourceManagerObj_->RunStartCallback(deviceId, inputTypes,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL);
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }
    if (result) {
        sourceManagerObj_->SetDeviceMapValue(deviceId, DINPUT_SOURCE_SWITCH_ON);
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_ITP] = inputTypes;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_START_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseStopRemoteInput(const std::string deviceId, const uint32_t inputTypes, bool result)
{
    DHLOGI("OnResponseStopRemoteInput called, deviceId: %{public}s, inputTypes: %{public}d, result: %{public}s.",
        GetAnonyString(deviceId).c_str(), inputTypes, result ? "true" : "failed");

    if (sourceManagerObj_ == nullptr) {
        DHLOGE("OnResponseStopRemoteInput sourceManagerObj_ is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("OnResponseStopRemoteInput GetCallbackEventHandler is null.");
        sourceManagerObj_->RunStopCallback(deviceId, inputTypes,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL);
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_ITP] = inputTypes;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_STOP_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseStartRemoteInputDhid(
    const std::string deviceId, const std::string &dhids, bool result)
{
    DHLOGI("OnResponseStartRemoteInputDhid called, deviceId: %{public}s, result: %{public}s.",
        GetAnonyString(deviceId).c_str(), result ? "success" : "failed");

    if (sourceManagerObj_ == nullptr) {
        DHLOGE("OnResponseStartRemoteInputDhid sourceManagerObj_ is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("OnResponseStartRemoteInputDhid GetCallbackEventHandler is null.");
        sourceManagerObj_->RunStartDhidCallback(deviceId, dhids,
                                                ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL);
        return;
    }
    if (result) {
        sourceManagerObj_->SetDeviceMapValue(deviceId, DINPUT_SOURCE_SWITCH_ON);
    }

    std::vector<std::string> devDhIds;
    SplitStringToVector(dhids, INPUT_STRING_SPLIT_POINT, devDhIds);

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DHID] = dhids;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_START_DHID_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseStopRemoteInputDhid(
    const std::string deviceId, const std::string &dhids, bool result)
{
    DHLOGI("OnResponseStopRemoteInputDhid called, deviceId: %{public}s, result: %{public}s.",
        GetAnonyString(deviceId).c_str(), result ? "success" : "failed");

    if (sourceManagerObj_ == nullptr) {
        DHLOGE("OnResponseStopRemoteInputDhid sourceManagerObj_ is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("OnResponseStopRemoteInputDhid GetCallbackEventHandler is null.");
        sourceManagerObj_->RunStopDhidCallback(deviceId, dhids,
                                               ERR_DH_INPUT_SERVER_SOURCE_MANAGERGET_CALLBACK_HANDLER_FAIL);
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DHID] = dhids;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = result;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_STOP_DHID_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnResponseKeyState(const std::string deviceId,
    const std::string &dhid, const uint32_t type, const uint32_t code, const uint32_t value)
{
    DHLOGI("OnResponseKeyState called, deviceId: %{public}s, dhid: %{public}s.", GetAnonyString(deviceId).c_str(),
        GetAnonyString(dhid).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        sourceManagerObj_->RunKeyStateCallback(deviceId, dhid, type, code, value);
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = deviceId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DHID] = dhid;
    tmpJson[INPUT_SOURCEMANAGER_KEY_TYPE] = type;
    tmpJson[INPUT_SOURCEMANAGER_KEY_CODE] = code;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = value;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_KEY_STATE_MSG, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}


void DInputSourceListener::OnResponseKeyStateBatch(const std::string deviceId, const std::string &event)
{
    DHLOGI("OnResponseKeyStateBatch events, deviceId: %{public}s.", GetAnonyString(deviceId).c_str());
    OnReceivedEventRemoteInput(deviceId, event);
}

void DInputSourceListener::OnReceivedEventRemoteInput(const std::string deviceId, const std::string &event)
{
    nlohmann::json inputData = nlohmann::json::parse(event, nullptr, false);
    if (inputData.is_discarded()) {
        DHLOGE("inputData parse failed!");
        return;
    }

    if (!inputData.is_array()) {
        DHLOGE("inputData not vector!");
        return;
    }

    size_t jsonSize = inputData.size();
    DHLOGD("OnReceivedEventRemoteInput called, deviceId: %{public}s, json size:%{public}zu.",
        GetAnonyString(deviceId).c_str(), jsonSize);

    std::vector<RawEvent> mEventBuffer(jsonSize);
    int idx = 0;
    for (auto it = inputData.begin(); it != inputData.end(); ++it) {
        nlohmann::json oneData = (*it);
        if (!IsInt64(oneData, INPUT_KEY_WHEN) || !IsUInt32(oneData, INPUT_KEY_TYPE) ||
            !IsUInt32(oneData, INPUT_KEY_CODE) || !IsInt32(oneData, INPUT_KEY_VALUE) ||
            !IsString(oneData, INPUT_KEY_DESCRIPTOR) || !IsString(oneData, INPUT_KEY_PATH)) {
            DHLOGE("The key is invaild.");
            continue;
        }
        mEventBuffer[idx].when = oneData[INPUT_KEY_WHEN];
        mEventBuffer[idx].type = oneData[INPUT_KEY_TYPE];
        mEventBuffer[idx].code = oneData[INPUT_KEY_CODE];
        mEventBuffer[idx].value = oneData[INPUT_KEY_VALUE];
        mEventBuffer[idx].descriptor = oneData[INPUT_KEY_DESCRIPTOR];
        mEventBuffer[idx].path = oneData[INPUT_KEY_PATH];
        RecordEventLog(oneData[INPUT_KEY_WHEN], oneData[INPUT_KEY_TYPE], oneData[INPUT_KEY_CODE],
            oneData[INPUT_KEY_VALUE], oneData[INPUT_KEY_PATH]);
        ++idx;
    }

    DistributedInputInject::GetInstance().RegisterDistributedEvent(deviceId, mEventBuffer);
}

void DInputSourceListener::OnReceiveRelayPrepareResult(int32_t status,
    const std::string &srcId, const std::string &sinkId)
{
    DHLOGI("status:%{public}d, srcId: %{public}s, sinkId: %{public}s.", status, GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SRC_DEVID] = srcId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SINK_DEVID] = sinkId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = status;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_MMI, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnReceiveRelayUnprepareResult(int32_t status,
    const std::string &srcId, const std::string &sinkId)
{
    DHLOGI("status:%{public}d, srcId: %{public}s, sinkId: %{public}s.", status, GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SRC_DEVID] = srcId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SINK_DEVID] = sinkId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = status;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_MMI, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnReceiveRelayStartDhidResult(int32_t status,
    const std::string &srcId, const std::string &sinkId, const std::string &dhids)
{
    DHLOGI("status:%{public}d, srcId: %{public}s, sinkId: %{public}s.", status, GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SRC_DEVID] = srcId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SINK_DEVID] = sinkId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = status;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DHID] = dhids;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_RELAY_STARTDHID_RESULT_MMI, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnReceiveRelayStopDhidResult(int32_t status,
    const std::string &srcId, const std::string &sinkId, const std::string &dhids)
{
    DHLOGI("status:%{public}d, srcId: %{public}s, sinkId: %{public}s.", status, GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SRC_DEVID] = srcId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SINK_DEVID] = sinkId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = status;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DHID] = dhids;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_RELAY_STOPDHID_RESULT_MMI, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnReceiveRelayStartTypeResult(int32_t status,
    const std::string &srcId, const std::string &sinkId, uint32_t inputTypes)
{
    DHLOGI("status:%{public}d, srcId: %{public}s, sinkId: %{public}s.", status, GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SRC_DEVID] = srcId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SINK_DEVID] = sinkId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = status;
    tmpJson[INPUT_SOURCEMANAGER_KEY_TYPE] = inputTypes;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_RELAY_STARTTYPE_RESULT_MMI, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::OnReceiveRelayStopTypeResult(int32_t status,
    const std::string &srcId, const std::string &sinkId, uint32_t inputTypes)
{
    DHLOGI("status:%{public}d, srcId: %{public}s, sinkId: %{public}s.", status, GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("sourceManagerObj is null.");
        return;
    }
    if (sourceManagerObj_->GetCallbackEventHandler() == nullptr) {
        DHLOGE("GetCallbackEventHandler is null.");
        return;
    }

    auto jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SRC_DEVID] = srcId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_SINK_DEVID] = sinkId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_VALUE] = status;
    tmpJson[INPUT_SOURCEMANAGER_KEY_TYPE] = inputTypes;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent =
        AppExecFwk::InnerEvent::Get(DINPUT_SOURCE_MANAGER_RELAY_STOPTYPE_RESULT_MMI, jsonArrayMsg, 0);
    sourceManagerObj_->GetCallbackEventHandler()->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void DInputSourceListener::RecordEventLog(int64_t when, int32_t type, int32_t code,
    int32_t value, const std::string &path)
{
    std::string eventType = "";
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
            eventType = "other type";
            break;
    }
    DHLOGD("3.E2E-Test Source softBus receive event, EventType: %{public}s, Code: %{public}d, Value: %{public}d, "
        "Path: %{public}s, When: %{public}" PRId64 "", eventType.c_str(), code, value, path.c_str(), when);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS