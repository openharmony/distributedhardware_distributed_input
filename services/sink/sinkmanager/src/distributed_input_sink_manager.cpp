/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "distributed_input_sink_manager.h"

#include <algorithm>
#include <fcntl.h>
#include <pthread.h>
#include <thread>
#include <linux/input.h>

#include "dinput_softbus_define.h"
#include "distributed_hardware_fwk_kit.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "screen_manager.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "distributed_input_collector.h"
#include "distributed_input_sink_switch.h"
#include "distributed_input_sink_transport.h"

#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "hidumper.h"
#include "hisysevent_util.h"
#include "white_list_util.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
REGISTER_SYSTEM_ABILITY_BY_ID(DistributedInputSinkManager, DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID, true);

DistributedInputSinkManager::DistributedInputSinkManager(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate)
{
    DHLOGI("DistributedInputSinkManager ctor!");
    inputTypes_ = DInputDeviceType::NONE;
}

DistributedInputSinkManager::~DistributedInputSinkManager()
{
    DHLOGI("DistributedInputSinkManager dtor!");
    projectWindowListener_ = nullptr;
}

DistributedInputSinkManager::DInputSinkListener::DInputSinkListener(DistributedInputSinkManager *manager)
{
    sinkManagerObj_ = manager;
    sinkManagerObj_->SetInputTypes(static_cast<uint32_t>(DInputDeviceType::NONE));
    DHLOGI("DInputSinkListener init.");
}

DistributedInputSinkManager::DInputSinkListener::~DInputSinkListener()
{
    sinkManagerObj_->SetInputTypes(static_cast<uint32_t>(DInputDeviceType::NONE));
    sinkManagerObj_ = nullptr;
    DHLOGI("DInputSinkListener destory.");
}

void DistributedInputSinkManager::QueryLocalWhiteList(nlohmann::json &jsonStr)
{
    TYPE_WHITE_LIST_VEC vecFilter;
    std::string localNetworkId = GetLocalDeviceInfo().networkId;
    if (!localNetworkId.empty()) {
        WhiteListUtil::GetInstance().GetWhiteList(localNetworkId, vecFilter);
    } else {
        DHLOGE("Query local network id from softbus failed");
        jsonStr[DINPUT_SOFTBUS_KEY_WHITE_LIST] = "";
        return;
    }

    if (vecFilter.empty() || vecFilter[0].empty() || vecFilter[0][0].empty()) {
        DHLOGE("White list is null.");
        jsonStr[DINPUT_SOFTBUS_KEY_WHITE_LIST] = "";
        return;
    }
    nlohmann::json filterMsg(vecFilter);
    std::string object = filterMsg.dump();
    jsonStr[DINPUT_SOFTBUS_KEY_WHITE_LIST] = object;
}

void DistributedInputSinkManager::DInputSinkListener::OnPrepareRemoteInput(
    const int32_t& sessionId, const std::string &deviceId)
{
    DHLOGI("OnPrepareRemoteInput called, sessionId: %d, devId: %s", sessionId, GetAnonyString(deviceId).c_str());
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ONPREPARE;
    std::string smsg = "";
    int32_t ret = DistributedInputCollector::GetInstance().Init(
        DistributedInputSinkTransport::GetInstance().GetEventHandler());
    if (ret != DH_SUCCESS) {
        DHLOGE("DInputSinkListener init InputCollector error.");
        jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = false;
        jsonStr[DINPUT_SOFTBUS_KEY_WHITE_LIST] = "";
        smsg = jsonStr.dump();
        DistributedInputSinkTransport::GetInstance().RespPrepareRemoteInput(sessionId, smsg);
        return;
    }

    DistributedInputSinkSwitch::GetInstance().AddSession(sessionId);
    sinkManagerObj_->QueryLocalWhiteList(jsonStr);
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespPrepareRemoteInput(sessionId, smsg);
}

void DistributedInputSinkManager::DInputSinkListener::OnUnprepareRemoteInput(const int32_t& sessionId)
{
    DHLOGI("OnUnprepareRemoteInput called, sessionId: %d", sessionId);
    OnStopRemoteInput(sessionId, static_cast<uint32_t>(DInputDeviceType::ALL));
    DistributedInputSinkSwitch::GetInstance().RemoveSession(sessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ONUNPREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    std::string smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespUnprepareRemoteInput(sessionId, smsg);
}

void DistributedInputSinkManager::DInputSinkListener::OnRelayPrepareRemoteInput(const int32_t &toSrcSessionId,
    const int32_t &toSinkSessionId, const std::string &deviceId)
{
    DHLOGI("OnRelayPrepareRemoteInput called, toSinkSessionId: %d, devId: %s", toSinkSessionId,
        GetAnonyString(deviceId).c_str());
    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_PREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = toSrcSessionId;
    std::string smsg = "";
    int ret = DistributedInputCollector::GetInstance().Init(
        DistributedInputSinkTransport::GetInstance().GetEventHandler());
    if (ret != DH_SUCCESS) {
        DHLOGE("DInputSinkListener init InputCollector error.");
        jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = false;
        jsonStr[DINPUT_SOFTBUS_KEY_WHITE_LIST] = "";
        smsg = jsonStr.dump();
        DistributedInputSinkTransport::GetInstance().RespPrepareRemoteInput(toSinkSessionId, smsg);
        return;
    }

    DistributedInputSinkSwitch::GetInstance().AddSession(toSinkSessionId);
    sinkManagerObj_->QueryLocalWhiteList(jsonStr);
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespPrepareRemoteInput(toSinkSessionId, smsg);
}

void DistributedInputSinkManager::DInputSinkListener::OnRelayUnprepareRemoteInput(const int32_t &toSrcSessionId,
    const int32_t &toSinkSessionId, const std::string &deviceId)
{
    DHLOGI("OnRelayUnprepareRemoteInput called, toSinkSessionId: %d, devId: %s", toSinkSessionId,
        GetAnonyString(deviceId).c_str());
    DistributedInputCollector::GetInstance().SetSharingTypes(false, static_cast<uint32_t>(DInputDeviceType::ALL));
    DistributedInputSinkSwitch::GetInstance().StopAllSwitch();
    DistributedInputSinkSwitch::GetInstance().RemoveSession(toSinkSessionId);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_UNPREPARE;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = toSrcSessionId;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    std::string smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespUnprepareRemoteInput(toSinkSessionId, smsg);
}

void DistributedInputSinkManager::DInputSinkListener::OnStartRemoteInput(
    const int32_t& sessionId, const uint32_t& inputTypes)
{
    DHLOGI("OnStartRemoteInput called, sessionId: %d, inputTypes: %u.", sessionId, inputTypes);
    // set new session
    int32_t startRes = DistributedInputSinkSwitch::GetInstance().StartSwitch(sessionId);

    sinkManagerObj_->SetStartTransFlag((startRes == DH_SUCCESS) ? DInputServerType::SINK_SERVER_TYPE
        : DInputServerType::NULL_SERVER_TYPE);

    bool result = (startRes == DH_SUCCESS);
    nlohmann::json jsonStrSta;
    jsonStrSta[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ONSTART;
    jsonStrSta[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    jsonStrSta[DINPUT_SOFTBUS_KEY_RESP_VALUE] = result;
    std::string smsg = jsonStrSta.dump();
    DistributedInputSinkTransport::GetInstance().RespStartRemoteInput(sessionId, smsg);

    if (startRes == DH_SUCCESS) {
        sinkManagerObj_->SetInputTypes(sinkManagerObj_->GetInputTypes() | inputTypes);
        AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingTypes(true,
            sinkManagerObj_->GetInputTypes());
        sinkManagerObj_->StoreStartDhids(sessionId, affDhIds.sharingDhIds);
        DistributedInputCollector::GetInstance().ReportDhIdSharingState(affDhIds);
    }

    bool isMouse = (sinkManagerObj_->GetInputTypes() & static_cast<uint32_t>(DInputDeviceType::MOUSE)) != 0;
    if (isMouse) {
        std::map<int32_t, std::string> deviceInfos;
        DistributedInputCollector::GetInstance().GetDeviceInfoByType(static_cast<uint32_t>(DInputDeviceType::MOUSE),
            deviceInfos);
        for (auto deviceInfo : deviceInfos) {
            DHLOGI("deviceInfo dhId, %s", GetAnonyString(deviceInfo.second).c_str());
            std::vector<std::string> vecStr;
            StringSplitToVector(deviceInfo.second, INPUT_STRING_SPLIT_POINT, vecStr);
            DInputState::GetInstance().RecordDhids(vecStr, DhidState::THROUGH_OUT, sessionId);
        }
    }
}

void DistributedInputSinkManager::DInputSinkListener::OnStopRemoteInput(
    const int32_t& sessionId, const uint32_t& inputTypes)
{
    DHLOGI("OnStopRemoteInput called, sessionId: %d, inputTypes: %d, curInputTypes: %d",
        sessionId, inputTypes, sinkManagerObj_->GetInputTypes());

    sinkManagerObj_->SetInputTypes(sinkManagerObj_->GetInputTypes() -
        (sinkManagerObj_->GetInputTypes() & inputTypes));
    AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingTypes(false, inputTypes);
    std::vector<std::string> stopIndeedDhIds;
    sinkManagerObj_->DeleteStopDhids(sessionId, affDhIds.noSharingDhIds, stopIndeedDhIds);
    AffectDhIds stopIndeedOnes;
    stopIndeedOnes.noSharingDhIds = stopIndeedDhIds;
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(stopIndeedOnes);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ONSTOP;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    std::string smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespStopRemoteInput(sessionId, smsg);

    bool isAllClosed = DistributedInputCollector::GetInstance().IsAllDevicesStoped();
    if (isAllClosed) {
        DistributedInputSinkSwitch::GetInstance().StopAllSwitch();
        if (DistributedInputSinkSwitch::GetInstance().GetSwitchOpenedSession() ==
            ERR_DH_INPUT_SERVER_SINK_GET_OPEN_SESSION_FAIL) {
            DHLOGI("all session is stop.");
            sinkManagerObj_->SetStartTransFlag(DInputServerType::NULL_SERVER_TYPE);
        }
    }
}

void DistributedInputSinkManager::DInputSinkListener::OnStartRemoteInputDhid(const int32_t &sessionId,
    const std::string &strDhids)
{
    DHLOGI("OnStartRemoteInputDhid called, sessionId: %d", sessionId);
    // set new session
    int32_t startRes = DistributedInputSinkSwitch::GetInstance().StartSwitch(sessionId);

    bool result = (startRes == DH_SUCCESS);
    nlohmann::json jsonStrSta;
    jsonStrSta[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_DHID_ONSTART;
    jsonStrSta[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = strDhids;
    jsonStrSta[DINPUT_SOFTBUS_KEY_RESP_VALUE] = result;
    std::string smsg = jsonStrSta.dump();
    DistributedInputSinkTransport::GetInstance().RespStartRemoteInput(sessionId, smsg);

    if (startRes != DH_SUCCESS) {
        DHLOGE("StartSwitch error.");
        return;
    }

    std::vector<std::string> vecStr;
    StringSplitToVector(strDhids, INPUT_STRING_SPLIT_POINT, vecStr);
    DInputState::GetInstance().RecordDhids(vecStr, DhidState::THROUGH_OUT, sessionId);
    AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingDhIds(true, vecStr);
    sinkManagerObj_->StoreStartDhids(sessionId, affDhIds.sharingDhIds);
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(affDhIds);
}

void DistributedInputSinkManager::DInputSinkListener::OnStopRemoteInputDhid(const int32_t &sessionId,
    const std::string &strDhids)
{
    DHLOGI("OnStopRemoteInputDhid called, sessionId: %d", sessionId);
    std::vector<std::string> stopIndeedDhIds;
    std::vector<std::string> stopOnCmdDhIds;
    StringSplitToVector(strDhids, INPUT_STRING_SPLIT_POINT, stopOnCmdDhIds);
    sinkManagerObj_->DeleteStopDhids(sessionId, stopOnCmdDhIds, stopIndeedDhIds);
    (void)DistributedInputCollector::GetInstance().SetSharingDhIds(false, stopIndeedDhIds);
    AffectDhIds stopIndeedOnes;
    stopIndeedOnes.noSharingDhIds = stopIndeedDhIds;
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(stopIndeedOnes);

    DInputState::GetInstance().RecordDhids(stopOnCmdDhIds, DhidState::THROUGH_IN, -1);

    if (DistributedInputCollector::GetInstance().IsAllDevicesStoped()) {
        DHLOGE("All dhid stop sharing, sessionId: %d is closed.", sessionId);
        DistributedInputSinkSwitch::GetInstance().StopSwitch(sessionId);
    }

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_DHID_ONSTOP;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = strDhids;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    std::string smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespStopRemoteInput(sessionId, smsg);

    bool isAllClosed = DistributedInputCollector::GetInstance().IsAllDevicesStoped();
    if (isAllClosed) {
        DistributedInputSinkSwitch::GetInstance().StopAllSwitch();
        sinkManagerObj_->SetInputTypes(static_cast<uint32_t>(DInputDeviceType::NONE));
        if (DistributedInputSinkSwitch::GetInstance().GetSwitchOpenedSession() ==
            ERR_DH_INPUT_SERVER_SINK_GET_OPEN_SESSION_FAIL) {
            DHLOGI("all session is stop.");
            sinkManagerObj_->SetStartTransFlag(DInputServerType::NULL_SERVER_TYPE);
        }
    }
}

void DistributedInputSinkManager::DInputSinkListener::OnRelayStartDhidRemoteInput(const int32_t &toSrcSessionId,
    const int32_t &toSinkSessionId, const std::string &deviceId, const std::string &strDhids)
{
    DHLOGI("onRelayStartDhidRemoteInput called, toSinkSessionId: %d", toSinkSessionId);
    // set new session
    int32_t startRes = DistributedInputSinkSwitch::GetInstance().StartSwitch(toSinkSessionId);
    bool result = (startRes == DH_SUCCESS);
    nlohmann::json jsonStrSta;
    jsonStrSta[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_STARTDHID;
    jsonStrSta[DINPUT_SOFTBUS_KEY_SESSION_ID] = toSrcSessionId;
    jsonStrSta[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = strDhids;
    jsonStrSta[DINPUT_SOFTBUS_KEY_RESP_VALUE] = result;
    std::string smsg = jsonStrSta.dump();
    DistributedInputSinkTransport::GetInstance().RespStartRemoteInput(toSinkSessionId, smsg);

    if (startRes != DH_SUCCESS) {
        DHLOGE("StartSwitch error.");
        return;
    }

    std::vector<std::string> vecStr;
    StringSplitToVector(strDhids, INPUT_STRING_SPLIT_POINT, vecStr);
    DInputState::GetInstance().RecordDhids(vecStr, DhidState::THROUGH_OUT, toSinkSessionId);
    AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingDhIds(true, vecStr);
    sinkManagerObj_->StoreStartDhids(toSinkSessionId, affDhIds.sharingDhIds);
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(affDhIds);
}

void DistributedInputSinkManager::DInputSinkListener::OnRelayStopDhidRemoteInput(const int32_t &toSrcSessionId,
    const int32_t &toSinkSessionId, const std::string &deviceId, const std::string &strDhids)
{
    DHLOGI("onRelayStopDhidRemoteInput called, toSinkSessionId: %d", toSinkSessionId);
    std::vector<std::string> stopIndeedDhIds;
    std::vector<std::string> stopOnCmdDhIds;
    StringSplitToVector(strDhids, INPUT_STRING_SPLIT_POINT, stopOnCmdDhIds);
    sinkManagerObj_->DeleteStopDhids(toSinkSessionId, stopOnCmdDhIds, stopIndeedDhIds);
    AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingDhIds(false, stopIndeedDhIds);
    AffectDhIds stopIndeedOnes;
    stopIndeedOnes.noSharingDhIds = stopIndeedDhIds;
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(stopIndeedOnes);

    DInputState::GetInstance().RecordDhids(stopOnCmdDhIds, DhidState::THROUGH_IN, -1);

    if (DistributedInputCollector::GetInstance().IsAllDevicesStoped()) {
        DHLOGE("All dhid stop sharing, sessionId: %d is closed.", toSinkSessionId);
        DistributedInputSinkSwitch::GetInstance().StopSwitch(toSinkSessionId);
    }

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_STOPDHID;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = toSrcSessionId;
    jsonStr[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = strDhids;
    std::string smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespStopRemoteInput(toSinkSessionId, smsg);

    bool isAllClosed = DistributedInputCollector::GetInstance().IsAllDevicesStoped();
    if (!isAllClosed) {
        DHLOGE("Not all devices are stopped.");
        return;
    }
    DistributedInputSinkSwitch::GetInstance().StopAllSwitch();
    sinkManagerObj_->SetInputTypes(static_cast<uint32_t>(DInputDeviceType::NONE));
    if (DistributedInputSinkSwitch::GetInstance().GetSwitchOpenedSession() ==
        ERR_DH_INPUT_SERVER_SINK_GET_OPEN_SESSION_FAIL) {
        DHLOGI("All session is stop.");
        sinkManagerObj_->SetStartTransFlag(DInputServerType::NULL_SERVER_TYPE);
    }
}

void DistributedInputSinkManager::DInputSinkListener::OnRelayStartTypeRemoteInput(const int32_t &toSrcSessionId,
    const int32_t &toSinkSessionId, const std::string &deviceId, uint32_t inputTypes)
{
    DHLOGI("OnRelayStartTypeRemoteInput called, toSinkSessionId: %d", toSinkSessionId);
    // set new session
    int32_t startRes = DistributedInputSinkSwitch::GetInstance().StartSwitch(toSinkSessionId);

    sinkManagerObj_->SetStartTransFlag((startRes == DH_SUCCESS) ? DInputServerType::SINK_SERVER_TYPE
        : DInputServerType::NULL_SERVER_TYPE);
    bool result = (startRes == DH_SUCCESS);
    nlohmann::json jsonStrSta;
    jsonStrSta[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_STARTTYPE;
    jsonStrSta[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    jsonStrSta[DINPUT_SOFTBUS_KEY_RESP_VALUE] = result;
    jsonStrSta[DINPUT_SOFTBUS_KEY_SESSION_ID] = toSrcSessionId;
    std::string smsg = jsonStrSta.dump();
    DistributedInputSinkTransport::GetInstance().RespStartRemoteInput(toSinkSessionId, smsg);

    if (startRes != DH_SUCCESS) {
        DHLOGE("OnRelayStartTypeRemoteInput startSwitch error.");
        return;
    }

    if (sinkManagerObj_->GetEventHandler() == nullptr) {
        DHLOGE("eventhandler is null.");
        return;
    }

    sinkManagerObj_->SetInputTypes(sinkManagerObj_->GetInputTypes() | inputTypes);
    AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingTypes(true,
        sinkManagerObj_->GetInputTypes());
    sinkManagerObj_->StoreStartDhids(toSinkSessionId, affDhIds.sharingDhIds);
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(affDhIds);

    bool isMouse = (sinkManagerObj_->GetInputTypes() & static_cast<uint32_t>(DInputDeviceType::MOUSE)) != 0;
    if (!isMouse) {
        DHLOGE("No mouse type.");
        return;
    }
    std::map<int32_t, std::string> deviceInfos;
    DistributedInputCollector::GetInstance().GetDeviceInfoByType(static_cast<uint32_t>(DInputDeviceType::MOUSE),
        deviceInfos);
    for (auto deviceInfo : deviceInfos) {
        DHLOGI("deviceInfo dhId, %s", GetAnonyString(deviceInfo.second).c_str());
        std::vector<std::string> vecStr;
        StringSplitToVector(deviceInfo.second, INPUT_STRING_SPLIT_POINT, vecStr);
        DInputState::GetInstance().RecordDhids(vecStr, DhidState::THROUGH_OUT, toSinkSessionId);
    }
}

void DistributedInputSinkManager::DInputSinkListener::OnRelayStopTypeRemoteInput(const int32_t &toSrcSessionId,
    const int32_t &toSinkSessionId, const std::string &deviceId, uint32_t inputTypes)
{
    DHLOGI("OnRelayStopTypeRemoteInput called, sessionId: %d, inputTypes: %d, curInputTypes: %d",
        toSinkSessionId, inputTypes, sinkManagerObj_->GetInputTypes());

    sinkManagerObj_->SetInputTypes(sinkManagerObj_->GetInputTypes() -
        (sinkManagerObj_->GetInputTypes() & inputTypes));
    AffectDhIds affDhIds = DistributedInputCollector::GetInstance().SetSharingTypes(false, inputTypes);
    std::vector<std::string> stopIndeedDhIds;
    sinkManagerObj_->DeleteStopDhids(toSinkSessionId, affDhIds.noSharingDhIds, stopIndeedDhIds);
    AffectDhIds stopIndeedOnes;
    stopIndeedOnes.noSharingDhIds = stopIndeedDhIds;
    DistributedInputCollector::GetInstance().ReportDhIdSharingState(stopIndeedOnes);

    nlohmann::json jsonStr;
    jsonStr[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_STOPTYPE;
    jsonStr[DINPUT_SOFTBUS_KEY_INPUT_TYPE] = inputTypes;
    jsonStr[DINPUT_SOFTBUS_KEY_RESP_VALUE] = true;
    jsonStr[DINPUT_SOFTBUS_KEY_SESSION_ID] = toSrcSessionId;
    std::string smsg = jsonStr.dump();
    DistributedInputSinkTransport::GetInstance().RespStopRemoteInput(toSinkSessionId, smsg);

    bool isAllClosed = DistributedInputCollector::GetInstance().IsAllDevicesStoped();
    if (!isAllClosed) {
        DHLOGE("Not all devices are stopped.");
        return;
    }
    DistributedInputSinkSwitch::GetInstance().StopAllSwitch();
    if (DistributedInputSinkSwitch::GetInstance().GetSwitchOpenedSession() ==
        ERR_DH_INPUT_SERVER_SINK_GET_OPEN_SESSION_FAIL) {
        DHLOGI("All session is stop.");
        sinkManagerObj_->SetStartTransFlag(DInputServerType::NULL_SERVER_TYPE);
    }
}

bool DistributedInputSinkManager::IsStopDhidOnCmdStillNeed(int32_t sessionId, const std::string &stopDhId)
{
    for (auto sessionDhid : sharingDhIdsMap_) {
        if (sessionDhid.first == sessionId) {
            DHLOGW("IsStopDhidOnCmdStillNeed sessionId=%d is self, ignore.", sessionId);
            continue;
        }
        for (auto dhid : sessionDhid.second) {
            if (stopDhId == dhid) {
                DHLOGI("IsStopDhidOnCmdStillNeed stopDhId=%s is find in session: %d", GetAnonyString(stopDhId).c_str(),
                    sessionDhid.first);
                return true;
            }
        }
    }
    DHLOGW("IsStopDhidOnCmdStillNeed stopDhId=%s is not find.", GetAnonyString(stopDhId).c_str());
    return false;
}

void DistributedInputSinkManager::DeleteStopDhids(int32_t sessionId, const std::vector<std::string> stopDhIds,
    std::vector<std::string> &stopIndeedDhIds)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (sharingDhIdsMap_.count(sessionId) <= 0) {
        DHLOGE("DeleteStopDhids sessionId: %d is not exist.", sessionId);
        return;
    }
    DHLOGI("DeleteStopDhids sessionId=%d before has dhid.size=%d, delDhIds.size=%d.", sessionId,
        sharingDhIdsMap_[sessionId].size(), stopDhIds.size());
    for (auto stopDhId : stopDhIds) {
        sharingDhIdsMap_[sessionId].erase(stopDhId);
    }
    if (sharingDhIdsMap_[sessionId].size() == 0) {
        sharingDhIdsMap_.erase(sessionId);
        DHLOGI("DeleteStopDhids sessionId=%d is delete.", sessionId);
    } else {
        DHLOGI("DeleteStopDhids sessionId=%d after has dhid.size=%d.", sessionId, sharingDhIdsMap_[sessionId].size());
    }
    // find which dhid can be stop
    for (auto tmp : stopDhIds) {
        bool isFind = IsStopDhidOnCmdStillNeed(sessionId, tmp);
        if (!isFind) {
            stopIndeedDhIds.push_back(tmp);
            sharingDhIds_.erase(tmp);
        }
    }
}

void DistributedInputSinkManager::StoreStartDhids(int32_t sessionId, const std::vector<std::string> &dhIds)
{
    std::set<std::string> tmpDhids;
    std::lock_guard<std::mutex> lock(mutex_);
    if (sharingDhIdsMap_.count(sessionId) > 0) {
        tmpDhids = sharingDhIdsMap_[sessionId];
    }
    DHLOGI("StoreStartDhids start tmpDhids.size=%d, add dhIds.size=%d.", tmpDhids.size(), dhIds.size());
    for (auto iter : dhIds) {
        tmpDhids.insert(iter);
        sharingDhIds_.insert(iter);
    }
    sharingDhIdsMap_[sessionId] = tmpDhids;
    DHLOGI("StoreStartDhids end tmpDhids.size=%d", tmpDhids.size());
}

void DistributedInputSinkManager::OnStart()
{
    if (serviceRunningState_ == ServiceSinkRunningState::STATE_RUNNING) {
        DHLOGI("dinput Manager Service has already started.");
        return;
    }
    DHLOGI("dinput Manager Service started.");
    if (!InitAuto()) {
        DHLOGI("failed to init service.");
        return;
    }
    serviceRunningState_ = ServiceSinkRunningState::STATE_RUNNING;
    runner_->Run();

    /*
	 * Publish service maybe failed, so we need call this function at the last,
     * so it can't affect the TDD test program.
     */
    bool ret = Publish(this);
    if (!ret) {
        return;
    }

    DHLOGI("DistributedInputSinkManager  start success.");
}

bool DistributedInputSinkManager::InitAuto()
{
    runner_ = AppExecFwk::EventRunner::Create(true);
    if (runner_ == nullptr) {
        return false;
    }

    handler_ = std::make_shared<DistributedInputSinkEventHandler>(runner_);
    DHLOGI("init success");
    return true;
}

void DistributedInputSinkManager::OnStop()
{
    DHLOGI("stop service");
    runner_.reset();
    handler_.reset();
    serviceRunningState_ = ServiceSinkRunningState::STATE_NOT_START;
}

/*
 * get event handler
 *
 * @return event handler object.
 */
std::shared_ptr<DistributedInputSinkEventHandler> DistributedInputSinkManager::GetEventHandler()
{
    return handler_;
}

int32_t DistributedInputSinkManager::Init()
{
    DHLOGI("enter");
    isStartTrans_ = DInputServerType::NULL_SERVER_TYPE;
    // transport init session
    int32_t ret = DistributedInputSinkTransport::GetInstance().Init();
    if (ret != DH_SUCCESS) {
        return ERR_DH_INPUT_SERVER_SINK_MANAGER_INIT_FAIL;
    }

    ret = DInputState::GetInstance().Init();
    if (ret != DH_SUCCESS) {
        DHLOGE("DInputState init fail!");
        return ERR_DH_INPUT_SERVER_SINK_MANAGER_INIT_FAIL;
    }

    statuslistener_ = std::make_shared<DInputSinkListener>(this);
    DistributedInputSinkTransport::GetInstance().RegistSinkRespCallback(statuslistener_);

    serviceRunningState_ = ServiceSinkRunningState::STATE_RUNNING;

    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit == nullptr) {
        DHLOGE("dhFwkKit obtain fail!");
        return ERR_DH_INPUT_SERVER_SINK_MANAGER_INIT_FAIL;
    }
    projectWindowListener_ = new ProjectWindowListener(this);
    dhFwkKit->RegisterPublisherListener(DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO, projectWindowListener_);
    return DH_SUCCESS;
}

int32_t DistributedInputSinkManager::Release()
{
    DHLOGI("Release sink Manager.");
    DistributedInputSinkSwitch::GetInstance().StopAllSwitch();
    DistributedInputSinkTransport::GetInstance().CloseAllSession();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        sharingDhIds_.clear();
        sharingDhIdsMap_.clear();
    }

    // notify callback servertype
    SetStartTransFlag(DInputServerType::NULL_SERVER_TYPE);
    // Release input collect resource
    DistributedInputCollector::GetInstance().Release();

    serviceRunningState_ = ServiceSinkRunningState::STATE_NOT_START;
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr && projectWindowListener_ != nullptr) {
        DHLOGI("UnPublish ProjectWindowListener");
        dhFwkKit->UnregisterPublisherListener(DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO, projectWindowListener_);
    }
    if (dhFwkKit != nullptr) {
        DHLOGD("Disable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, DISABLE_LOW_LATENCY.dump());
    }

    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_EXIT, "dinput sink sa exit success.");
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        DHLOGE("Failed to get SystemAbilityManager.");
        return ERR_DH_INPUT_SERVER_SINK_MANAGER_RELEASE_FAIL;
    }
    int32_t ret = systemAbilityMgr->UnloadSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID);
    if (ret != DH_SUCCESS) {
        DHLOGE("Failed to UnloadSystemAbility service! errcode: %d.", ret);
        return ERR_DH_INPUT_SERVER_SINK_MANAGER_RELEASE_FAIL;
    }
    DHLOGI("Sink unloadSystemAbility successfully.");
    return DH_SUCCESS;
}

int32_t DistributedInputSinkManager::RegisterGetSinkScreenInfosCallback(
    sptr<IGetSinkScreenInfosCallback> callback)
{
    DHLOGI("start");
    if (callback != nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        getSinkScreenInfosCallbacks_.insert(callback);
    }
    return DH_SUCCESS;
}

uint32_t DistributedInputSinkManager::GetSinkScreenInfosCbackSize()
{
    return getSinkScreenInfosCallbacks_.size();
}

DInputServerType DistributedInputSinkManager::GetStartTransFlag()
{
    return isStartTrans_;
}
void DistributedInputSinkManager::SetStartTransFlag(const DInputServerType flag)
{
    DHLOGI("Set Sink isStartTrans_ %d", static_cast<int32_t>(flag));
    isStartTrans_ = flag;
}

uint32_t DistributedInputSinkManager::GetInputTypes()
{
    return static_cast<uint32_t>(inputTypes_);
}

void DistributedInputSinkManager::SetInputTypes(const uint32_t& inputTypes)
{
    inputTypes_ = static_cast<DInputDeviceType>(inputTypes);
}

DistributedInputSinkManager::ProjectWindowListener::ProjectWindowListener(DistributedInputSinkManager *manager)
{
    DHLOGI("ProjectWindowListener ctor!");
    std::lock_guard<std::mutex> lock(handleScreenMutex_);
    sinkManagerObj_ = manager;
    if (screen_ == nullptr) {
        std::vector<sptr<Rosen::Screen>> screens;
        Rosen::ScreenManager::GetInstance().GetAllScreens(screens);
        if (screens.size() > 0) {
            screen_ = screens[SCREEN_ID_DEFAULT];
        }
    }
}

DistributedInputSinkManager::ProjectWindowListener::~ProjectWindowListener()
{
    DHLOGI("ProjectWindowListener dtor!");
    std::lock_guard<std::mutex> lock(handleScreenMutex_);
    sinkManagerObj_ = nullptr;
    screen_ = nullptr;
}

void DistributedInputSinkManager::ProjectWindowListener::OnMessage(const DHTopic topic, const std::string& message)
{
    DHLOGI("ProjectWindowListener OnMessage!");
    if (topic != DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO) {
        DHLOGE("this topic is wrong, %d", static_cast<uint32_t>(topic));
        return;
    }
    std::string srcDeviceId = "";
    uint64_t srcWinId = 0;
    SinkScreenInfo sinkScreenInfo = {};
    int32_t parseRes = ParseMessage(message, srcDeviceId, srcWinId, sinkScreenInfo);
    if (parseRes != DH_SUCCESS) {
        DHLOGE("message parse failed!");
        return;
    }
    int32_t saveRes = UpdateSinkScreenInfoCache(srcDeviceId, srcWinId, sinkScreenInfo);
    if (saveRes != DH_SUCCESS) {
        DHLOGE("Save sink screen info failed!");
        return;
    }
    sptr<IRemoteObject> dScreenSinkSA = DInputContext::GetInstance().GetRemoteObject(
        DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID);
    sptr<DScreenSinkSvrRecipient> dScreenSinkDeathRecipient = new(std::nothrow) DScreenSinkSvrRecipient(srcDeviceId,
        srcWinId);
    dScreenSinkSA->AddDeathRecipient(dScreenSinkDeathRecipient);
    DInputContext::GetInstance().AddRemoteObject(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID, dScreenSinkSA);
}

int32_t DistributedInputSinkManager::ProjectWindowListener::ParseMessage(const std::string& message,
    std::string& srcDeviceId, uint64_t& srcWinId, SinkScreenInfo& sinkScreenInfo)
{
    nlohmann::json jsonObj = nlohmann::json::parse(message, nullptr, false);
    if (jsonObj.is_discarded()) {
        DHLOGE("jsonObj parse failed!");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    if (!IsString(jsonObj, SOURCE_DEVICE_ID)) {
        DHLOGE("sourceDevId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    srcDeviceId = jsonObj[SOURCE_DEVICE_ID].get<std::string>();
    if (!IsUInt64(jsonObj, SOURCE_WINDOW_ID)) {
        DHLOGE("sourceWinId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    srcWinId = jsonObj[SOURCE_WINDOW_ID].get<uint64_t>();
    if (!IsUInt64(jsonObj, SINK_SHOW_WINDOW_ID)) {
        DHLOGE("sinkWinId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkScreenInfo.sinkShowWinId = jsonObj[SINK_SHOW_WINDOW_ID].get<uint64_t>();
    if (!IsUInt32(jsonObj, SINK_PROJECT_SHOW_WIDTH)) {
        DHLOGE("sourceWinHeight key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkScreenInfo.sinkProjShowWidth = jsonObj[SINK_PROJECT_SHOW_WIDTH].get<std::uint32_t>();
    if (!IsUInt32(jsonObj, SINK_PROJECT_SHOW_HEIGHT)) {
        DHLOGE("sourceWinHeight key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkScreenInfo.sinkProjShowHeight = jsonObj[SINK_PROJECT_SHOW_HEIGHT].get<std::uint32_t>();
    if (!IsUInt32(jsonObj, SINK_WINDOW_SHOW_X)) {
        DHLOGE("sourceWinHeight key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkScreenInfo.sinkWinShowX = jsonObj[SINK_WINDOW_SHOW_X].get<std::uint32_t>();
    if (!IsUInt32(jsonObj, SINK_WINDOW_SHOW_Y)) {
        DHLOGE("sourceWinHeight key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkScreenInfo.sinkWinShowY = jsonObj[SINK_WINDOW_SHOW_Y].get<std::uint32_t>();
    return DH_SUCCESS;
}

int32_t DistributedInputSinkManager::ProjectWindowListener::UpdateSinkScreenInfoCache(const std::string& srcDevId,
    const uint64_t srcWinId, const SinkScreenInfo& sinkScreenInfoTmp)
{
    std::string srcScreenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(srcDevId, srcWinId);
    SinkScreenInfo sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(srcScreenInfoKey);
    sinkScreenInfo.sinkShowWinId = sinkScreenInfoTmp.sinkShowWinId;
    sinkScreenInfo.sinkProjShowWidth = sinkScreenInfoTmp.sinkProjShowWidth;
    sinkScreenInfo.sinkProjShowHeight = sinkScreenInfoTmp.sinkProjShowHeight;
    sinkScreenInfo.sinkWinShowX = sinkScreenInfoTmp.sinkWinShowX;
    sinkScreenInfo.sinkWinShowY = sinkScreenInfoTmp.sinkWinShowY;
    sinkScreenInfo.sinkShowWidth = GetScreenWidth();
    sinkScreenInfo.sinkShowHeight = GetScreenHeight();
    LocalAbsInfo info = DInputContext::GetInstance().GetLocalTouchScreenInfo().localAbsInfo;
    sinkScreenInfo.sinkPhyWidth = static_cast<uint32_t>(info.absMtPositionXMax + 1);
    sinkScreenInfo.sinkPhyHeight = static_cast<uint32_t>(info.absMtPositionYMax + 1);
    DHLOGI("sinkShowWinId: %d, sinkProjShowWidth: %d, sinkProjShowHeight: %d, sinkWinShowX: %d, sinkWinShowY: %d,"
        "sinkShowWidth: %d, sinkShowHeight: %d, sinkPhyWidth: %d, sinkPhyHeight: %d", sinkScreenInfo.sinkShowWinId,
        sinkScreenInfo.sinkProjShowWidth, sinkScreenInfo.sinkProjShowHeight, sinkScreenInfo.sinkWinShowX,
        sinkScreenInfo.sinkWinShowY, sinkScreenInfo.sinkShowWidth, sinkScreenInfo.sinkShowHeight,
        sinkScreenInfo.sinkPhyWidth, sinkScreenInfo.sinkPhyHeight);
    int32_t ret = DInputContext::GetInstance().UpdateSinkScreenInfo(srcScreenInfoKey, sinkScreenInfo);
    std::lock_guard<std::mutex> lock(sinkManagerObj_->mutex_);
    if ((ret == DH_SUCCESS) && (sinkManagerObj_->GetSinkScreenInfosCbackSize() > 0)) {
        sinkManagerObj_->CallBackScreenInfoChange();
    }
    return ret;
}

uint32_t DistributedInputSinkManager::ProjectWindowListener::GetScreenWidth()
{
    std::lock_guard<std::mutex> lock(handleScreenMutex_);
    if (screen_ == nullptr) {
        DHLOGE("screen is nullptr!");
        return DEFAULT_VALUE;
    }
    return screen_->GetWidth();
}

uint32_t DistributedInputSinkManager::ProjectWindowListener::GetScreenHeight()
{
    std::lock_guard<std::mutex> lock(handleScreenMutex_);
    if (screen_ == nullptr) {
        DHLOGE("screen is nullptr!");
        return DEFAULT_VALUE;
    }
    return screen_->GetHeight();
}

DistributedInputSinkManager::DScreenSinkSvrRecipient::DScreenSinkSvrRecipient(const std::string& srcDevId,
    const uint64_t srcWinId)
{
    DHLOGI("DScreenStatusListener ctor!");
    this->srcDevId_ = srcDevId;
    this->srcWinId_ = srcWinId;
}

DistributedInputSinkManager::DScreenSinkSvrRecipient::~DScreenSinkSvrRecipient()
{
    DHLOGI("DScreenStatusListener dtor!");
}

void DistributedInputSinkManager::DScreenSinkSvrRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("DScreenSinkSvrRecipient OnRemoteDied");
    sptr<IRemoteObject> remoteObject = remote.promote();
    if (!remoteObject) {
        DHLOGE("OnRemoteDied remote promoted failed");
        return;
    }
    std::string srcScreenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(srcDevId_, srcWinId_);
    DInputContext::GetInstance().RemoveSinkScreenInfo(srcScreenInfoKey);
    DInputContext::GetInstance().RemoveRemoteObject(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID);
}

int32_t DistributedInputSinkManager::NotifyStartDScreen(const SrcScreenInfo& srcScreenInfo)
{
    DHLOGI("NotifyStartDScreen start!");

    CleanExceptionalInfo(srcScreenInfo);

    std::string screenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(srcScreenInfo.devId,
        srcScreenInfo.sourceWinId);
    SinkScreenInfo sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(screenInfoKey);
    sinkScreenInfo.srcScreenInfo = srcScreenInfo;
    DHLOGI("OnRemoteRequest the data: devId: %s, sourceWinId: %d, sourceWinWidth: %d, sourceWinHeight: %d,"
        "sourcePhyId: %s, sourcePhyFd: %d, sourcePhyWidth: %d, sourcePhyHeight: %d",
        GetAnonyString(srcScreenInfo.devId).c_str(), srcScreenInfo.sourceWinId, srcScreenInfo.sourceWinWidth,
        srcScreenInfo.sourceWinHeight, GetAnonyString(srcScreenInfo.sourcePhyId).c_str(), srcScreenInfo.sourcePhyFd,
        srcScreenInfo.sourcePhyWidth, srcScreenInfo.sourcePhyHeight);
    int32_t ret = DInputContext::GetInstance().UpdateSinkScreenInfo(screenInfoKey, sinkScreenInfo);
    std::lock_guard<std::mutex> lock(mutex_);
    if ((ret == DH_SUCCESS) && (getSinkScreenInfosCallbacks_.size() > 0)) {
        CallBackScreenInfoChange();
    }
    return ret;
}

void DistributedInputSinkManager::CallBackScreenInfoChange()
{
    DHLOGI("start!");
    std::vector<std::vector<uint32_t>> transInfos;
    auto sinkInfos = DInputContext::GetInstance().GetAllSinkScreenInfo();
    std::vector<uint32_t> info;
    for (const auto& [id, sinkInfo] : sinkInfos) {
        info.clear();
        info.emplace_back(sinkInfo.transformInfo.sinkWinPhyX);
        info.emplace_back(sinkInfo.transformInfo.sinkWinPhyY);
        info.emplace_back(sinkInfo.transformInfo.sinkProjPhyWidth);
        info.emplace_back(sinkInfo.transformInfo.sinkProjPhyHeight);
        transInfos.emplace_back(info);
    }
    nlohmann::json screenMsg(transInfos);
    std::string str = screenMsg.dump();
    for (const auto& iter : getSinkScreenInfosCallbacks_) {
        iter->OnResult(str);
    }
}

void DistributedInputSinkManager::CleanExceptionalInfo(const SrcScreenInfo& srcScreenInfo)
{
    DHLOGI("CleanExceptionalInfo start!");
    std::string uuid = srcScreenInfo.uuid;
    int32_t sessionId = srcScreenInfo.sessionId;
    auto sinkInfos = DInputContext::GetInstance().GetAllSinkScreenInfo();

    for (const auto& [id, sinkInfo] : sinkInfos) {
        auto srcInfo = sinkInfo.srcScreenInfo;
        if ((std::strcmp(srcInfo.uuid.c_str(), uuid.c_str()) == 0) && (srcInfo.sessionId != sessionId)) {
            DInputContext::GetInstance().RemoveSinkScreenInfo(id);
            DHLOGI("CleanExceptionalInfo screenInfoKey: %s, sessionId: %d", id.c_str(), sessionId);
        }
    }
}

int32_t DistributedInputSinkManager::NotifyStopDScreen(const std::string& srcScreenInfoKey)
{
    DHLOGI("NotifyStopDScreen start, srcScreenInfoKey: %s", GetAnonyString(srcScreenInfoKey).c_str());
    if (srcScreenInfoKey.empty()) {
        DHLOGE("srcScreenInfoKey is empty, srcScreenInfoKey: %s", GetAnonyString(srcScreenInfoKey).c_str());
        return ERR_DH_INPUT_SERVER_SINK_SCREEN_INFO_IS_EMPTY;
    }
    return DInputContext::GetInstance().RemoveSinkScreenInfo(srcScreenInfoKey);
}

int32_t DistributedInputSinkManager::RegisterSharingDhIdListener(sptr<ISharingDhIdListener> sharingDhIdListener)
{
    DHLOGI("RegisterSharingDhIdListener");
    DistributedInputCollector::GetInstance().RegisterSharingDhIdListener(sharingDhIdListener);
    return DH_SUCCESS;
}

int32_t DistributedInputSinkManager::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    DHLOGI("DistributedInputSinkManager Dump.");
    std::vector<std::string> argsStr(args.size());
    std::transform(args.begin(), args.end(), argsStr.begin(), [](const auto &item) {return Str16ToStr8(item);});
    std::string result("");
    if (!HiDumper::GetInstance().HiDump(argsStr, result)) {
        DHLOGI("Hidump error.");
        return ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL;
    }

    int32_t ret = dprintf(fd, "%s\n", result.c_str());
    if (ret < 0) {
        DHLOGE("dprintf error.");
        return ERR_DH_INPUT_HIDUMP_DPRINTF_FAIL;
    }
    return DH_SUCCESS;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
