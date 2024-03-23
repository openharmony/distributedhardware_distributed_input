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

#include "dinput_source_manager_event_handler.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "distributed_hardware_fwk_kit.h"
#include "ipublisher_listener.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "dinput_softbus_define.h"
#include "distributed_input_source_transport.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DInputSourceManagerEventHandler::DInputSourceManagerEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, DistributedInputSourceManager *manager)
    : AppExecFwk::EventHandler(runner)
{
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RIGISTER_MSG] = &DInputSourceManagerEventHandler::NotifyRegisterCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_UNRIGISTER_MSG] = &DInputSourceManagerEventHandler::NotifyUnregisterCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_PREPARE_MSG] = &DInputSourceManagerEventHandler::NotifyPrepareCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_UNPREPARE_MSG] = &DInputSourceManagerEventHandler::NotifyUnprepareCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_START_MSG] = &DInputSourceManagerEventHandler::NotifyStartCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_STOP_MSG] = &DInputSourceManagerEventHandler::NotifyStopCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_START_DHID_MSG] = &DInputSourceManagerEventHandler::NotifyStartDhidCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_STOP_DHID_MSG] = &DInputSourceManagerEventHandler::NotifyStopDhidCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_KEY_STATE_MSG] = &DInputSourceManagerEventHandler::NotifyKeyStateCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_STARTSERVER_MSG] = &DInputSourceManagerEventHandler::NotifyStartServerCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_TO_ORIGIN] =
        &DInputSourceManagerEventHandler::NotifyRelayPrepareRemoteInput;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_TO_ORIGIN] =
        &DInputSourceManagerEventHandler::NotifyRelayUnprepareRemoteInput;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_MMI] =
        &DInputSourceManagerEventHandler::NotifyRelayPrepareCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_MMI] =
        &DInputSourceManagerEventHandler::NotifyRelayUnprepareCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_STARTDHID_RESULT_MMI] =
        &DInputSourceManagerEventHandler::NotifyRelayStartDhidCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_STOPDHID_RESULT_MMI] =
        &DInputSourceManagerEventHandler::NotifyRelayStopDhidCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_STARTTYPE_RESULT_MMI] =
        &DInputSourceManagerEventHandler::NotifyRelayStartTypeCallback;
    eventFuncMap_[DINPUT_SOURCE_MANAGER_RELAY_STOPTYPE_RESULT_MMI] =
        &DInputSourceManagerEventHandler::NotifyRelayStopTypeCallback;

    sourceManagerObj_ = manager;
}

DInputSourceManagerEventHandler::~DInputSourceManagerEventHandler()
{
    eventFuncMap_.clear();
    sourceManagerObj_ = nullptr;
}

void DInputSourceManagerEventHandler::NotifyRegisterCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_HWID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    std::string dhId = innerMsg[INPUT_SOURCEMANAGER_KEY_HWID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];

    DistributedInputSourceManager::InputDeviceId inputDeviceId {deviceId, dhId};
    std::vector<DistributedInputSourceManager::InputDeviceId> tmpInputDevId = sourceManagerObj_->GetInputDeviceId();
    // Find out if the dh exists
    auto devIt  = std::find(tmpInputDevId.begin(), tmpInputDevId.end(), inputDeviceId);
    if (devIt != tmpInputDevId.end()) {
        if (result == false) {
            sourceManagerObj_->RemoveInputDeviceId(deviceId, dhId);
        }
    } else {
        DHLOGW("ProcessEvent DINPUT_SOURCE_MANAGER_RIGISTER_MSG the, devId: %{public}s, dhId: %{public}s is bad data.",
            GetAnonyString(deviceId).c_str(), GetAnonyString(dhId).c_str());
    }

    sourceManagerObj_->RunRegisterCallback(deviceId, dhId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyUnregisterCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_HWID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    std::string dhId = innerMsg[INPUT_SOURCEMANAGER_KEY_HWID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    if (result) {
        sourceManagerObj_->SetDeviceMapValue(deviceId, DINPUT_SOURCE_SWITCH_OFF);
    }
    sourceManagerObj_->RunUnregisterCallback(deviceId, dhId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyPrepareCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_WHITELIST)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    std::string object = innerMsg[INPUT_SOURCEMANAGER_KEY_WHITELIST];

    sourceManagerObj_->RunPrepareCallback(deviceId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_MSG_IS_BAD, object);
}

void DInputSourceManagerEventHandler::NotifyUnprepareCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    if (result) {
        sourceManagerObj_->SetDeviceMapValue(deviceId, DINPUT_SOURCE_SWITCH_OFF);
    }
    sourceManagerObj_->RunUnprepareCallback(deviceId,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyStartCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_ITP) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    uint32_t inputTypes = innerMsg[INPUT_SOURCEMANAGER_KEY_ITP];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    DHLOGI("Start DInput Recv Callback ret: %{public}s, devId: %{public}s, inputTypes: %{public}d",
        result ? "true" : "false", GetAnonyString(deviceId).c_str(), inputTypes);
    if (result) {
        sourceManagerObj_->SetInputTypesMap(
            deviceId, sourceManagerObj_->GetInputTypesMap(deviceId) | inputTypes);
    }
    sourceManagerObj_->SetStartTransFlag((result && (sourceManagerObj_->GetInputTypesMap(deviceId) > 0)) ?
        DInputServerType::SOURCE_SERVER_TYPE : DInputServerType::NULL_SERVER_TYPE);
    sourceManagerObj_->RunStartCallback(deviceId, inputTypes,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyStopCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_ITP) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    uint32_t inputTypes = innerMsg[INPUT_SOURCEMANAGER_KEY_ITP];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];

    DHLOGI("Stop DInput Recv Callback ret: %{public}d, devId: %{public}s, inputTypes: %{public}d",
        result, GetAnonyString(deviceId).c_str(), inputTypes);
    if (result && (sourceManagerObj_->GetInputTypesMap(deviceId) & inputTypes)) {
        sourceManagerObj_->SetInputTypesMap(
            deviceId, sourceManagerObj_->GetInputTypesMap(deviceId) -
            (sourceManagerObj_->GetInputTypesMap(deviceId) & inputTypes));
    }

    if (sourceManagerObj_->GetInputTypesMap(deviceId) == 0) {
        sourceManagerObj_->SetDeviceMapValue(deviceId, DINPUT_SOURCE_SWITCH_OFF);
    }

    // DeviceMap_ all sink device switch is off,call isstart's callback
    bool isAllDevSwitchOff = sourceManagerObj_->GetDeviceMapAllDevSwitchOff();
    if (isAllDevSwitchOff) {
        DHLOGI("All Dev Switch Off");
        sourceManagerObj_->SetStartTransFlag(DInputServerType::NULL_SERVER_TYPE);
    }
    sourceManagerObj_->RunStopCallback(deviceId, inputTypes,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyStartDhidCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DHID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    std::string dhidStr = innerMsg[INPUT_SOURCEMANAGER_KEY_DHID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];

    sourceManagerObj_->RunStartDhidCallback(deviceId, dhidStr,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyStopDhidCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DHID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    std::string dhidStr = innerMsg[INPUT_SOURCEMANAGER_KEY_DHID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];

    sourceManagerObj_->RunStopDhidCallback(deviceId, dhidStr,
        result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_MSG_IS_BAD);
}

void DInputSourceManagerEventHandler::NotifyKeyStateCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DHID) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_TYPE) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_CODE) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    std::string dhid = innerMsg[INPUT_SOURCEMANAGER_KEY_DHID];
    uint32_t keyType = innerMsg[INPUT_SOURCEMANAGER_KEY_TYPE];
    uint32_t keyCode = innerMsg[INPUT_SOURCEMANAGER_KEY_CODE];
    uint32_t keyValue = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];

    sourceManagerObj_->RunKeyStateCallback(deviceId, dhid, keyType, keyCode, keyValue);
}

void DInputSourceManagerEventHandler::NotifyStartServerCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t serType = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    DInputServerType startTransFlag = DInputServerType(serType);
    sourceManagerObj_->SetStartTransFlag(startTransFlag);
}


void DInputSourceManagerEventHandler::NotifyRelayPrepareCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SRC_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SINK_DEVID)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t status = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];
    std::string srcId = innerMsg[INPUT_SOURCEMANAGER_KEY_SRC_DEVID];
    std::string sinkId = innerMsg[INPUT_SOURCEMANAGER_KEY_SINK_DEVID];

    sourceManagerObj_->RunRelayPrepareCallback(srcId, sinkId, status);
}

void DInputSourceManagerEventHandler::NotifyRelayUnprepareCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SRC_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SINK_DEVID)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t status = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];
    std::string srcId = innerMsg[INPUT_SOURCEMANAGER_KEY_SRC_DEVID];
    std::string sinkId = innerMsg[INPUT_SOURCEMANAGER_KEY_SINK_DEVID];

    sourceManagerObj_->RunRelayUnprepareCallback(srcId, sinkId, status);
}

void DInputSourceManagerEventHandler::NotifyRelayPrepareRemoteInput(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_WHITELIST) ||
        !IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_SESSIONID)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    std::string object = innerMsg[INPUT_SOURCEMANAGER_KEY_WHITELIST];
    int32_t toSrcSessionId = innerMsg[INPUT_SOURCEMANAGER_KEY_SESSIONID];
    DHLOGI("Device whitelist object: %{public}s", object.c_str());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        return;
    }

    // notify to origin sourcesa result.
    int32_t ret = DistributedInputSourceTransport::GetInstance().NotifyOriginPrepareResult(toSrcSessionId,
        localNetworkId, deviceId, result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_MSG_IS_BAD);
    if (ret != DH_SUCCESS) {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_TO_ORIGIN notify is fail.");
        return;
    }
    sourceManagerObj_->RunWhiteListCallback(deviceId, object);
}

void DInputSourceManagerEventHandler::NotifyRelayUnprepareRemoteInput(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DEVID) ||
        !IsBoolean(innerMsg, INPUT_SOURCEMANAGER_KEY_RESULT) ||
        !IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_SESSIONID)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    std::string deviceId = innerMsg[INPUT_SOURCEMANAGER_KEY_DEVID];
    bool result = innerMsg[INPUT_SOURCEMANAGER_KEY_RESULT];
    int32_t toSrcSessionId = innerMsg[INPUT_SOURCEMANAGER_KEY_SESSIONID];
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        return;
    }

    // notify to origin sourcesa result.
    int32_t ret = DistributedInputSourceTransport::GetInstance().NotifyOriginUnprepareResult(toSrcSessionId,
        localNetworkId, deviceId, result ? DH_SUCCESS : ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_MSG_IS_BAD);
    if (ret != DH_SUCCESS) {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_TO_ORIGIN notify is fail.");
        return;
    }
}

void DInputSourceManagerEventHandler::NotifyRelayStartDhidCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SRC_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SINK_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DHID)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t status = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];
    std::string srcId = innerMsg[INPUT_SOURCEMANAGER_KEY_SRC_DEVID];
    std::string sinkId = innerMsg[INPUT_SOURCEMANAGER_KEY_SINK_DEVID];
    std::string dhids = innerMsg[INPUT_SOURCEMANAGER_KEY_DHID];

    sourceManagerObj_->RunRelayStartDhidCallback(srcId, sinkId, status, dhids);
}

void DInputSourceManagerEventHandler::NotifyRelayStopDhidCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SRC_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SINK_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_DHID)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t status = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];
    std::string srcId = innerMsg[INPUT_SOURCEMANAGER_KEY_SRC_DEVID];
    std::string sinkId = innerMsg[INPUT_SOURCEMANAGER_KEY_SINK_DEVID];
    std::string dhids = innerMsg[INPUT_SOURCEMANAGER_KEY_DHID];

    sourceManagerObj_->RunRelayStopDhidCallback(srcId, sinkId, status, dhids);
}

void DInputSourceManagerEventHandler::NotifyRelayStartTypeCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SRC_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SINK_DEVID) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_TYPE)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t status = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];
    std::string srcId = innerMsg[INPUT_SOURCEMANAGER_KEY_SRC_DEVID];
    std::string sinkId = innerMsg[INPUT_SOURCEMANAGER_KEY_SINK_DEVID];
    uint32_t inputTypes = innerMsg[INPUT_SOURCEMANAGER_KEY_TYPE];

    sourceManagerObj_->RunRelayStartTypeCallback(srcId, sinkId, status, inputTypes);
}

void DInputSourceManagerEventHandler::NotifyRelayStopTypeCallback(const AppExecFwk::InnerEvent::Pointer &event)
{
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    nlohmann::json::iterator it = dataMsg->begin();
    nlohmann::json innerMsg = *it;
    if (!IsInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_VALUE) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SRC_DEVID) ||
        !IsString(innerMsg, INPUT_SOURCEMANAGER_KEY_SINK_DEVID) ||
        !IsUInt32(innerMsg, INPUT_SOURCEMANAGER_KEY_TYPE)) {
        DHLOGE("The key is invaild.");
        return ;
    }
    int32_t status = innerMsg[INPUT_SOURCEMANAGER_KEY_VALUE];
    std::string srcId = innerMsg[INPUT_SOURCEMANAGER_KEY_SRC_DEVID];
    std::string sinkId = innerMsg[INPUT_SOURCEMANAGER_KEY_SINK_DEVID];
    uint32_t inputTypes = innerMsg[INPUT_SOURCEMANAGER_KEY_TYPE];

    sourceManagerObj_->RunRelayStopTypeCallback(srcId, sinkId, status, inputTypes);
}

void DInputSourceManagerEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    auto iter = eventFuncMap_.find(event->GetInnerEventId());
    if (iter == eventFuncMap_.end()) {
        DHLOGE("Event Id %{public}d is undefined.", event->GetInnerEventId());
        return;
    }
    SourceEventFunc &func = iter->second;
    (this->*func)(event);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS