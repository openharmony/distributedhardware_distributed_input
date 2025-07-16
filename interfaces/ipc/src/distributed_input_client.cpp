/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "distributed_input_client.h"

#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "system_ability_definition.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "distributed_input_source_proxy.h"
#include "input_check_param.h"
#include "softbus_bus_center.h"
#include "white_list_util.h"
#include "dinput_sa_manager.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
std::shared_ptr<DistributedInputClient> DistributedInputClient::instance = std::make_shared<DistributedInputClient>();
DistributedInputClient::DistributedInputClient() : isAddWhiteListCbReg_(false), isDelWhiteListCbReg_(false),
    isNodeMonitorCbReg_(false), isSimulationEventCbReg_(false), isSharingDhIdsReg_(false),
    isGetSinkScreenInfosCbReg_(false)
{
    DHLOGI("DistributedInputClient init start");
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    eventHandler_ = std::make_shared<DistributedInputClient::DInputClientEventHandler>(runner);
    DInputSAManager::GetInstance().RegisterEventHandler(eventHandler_);
    DInputSAManager::GetInstance().Init();
    DHLOGI("DistributedInputClient init end.");
}

DistributedInputClient &DistributedInputClient::GetInstance()
{
    return *instance.get();
}

void DistributedInputClient::RegisterDInputCb::OnResult(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    std::lock_guard<std::mutex> lock(DistributedInputClient::GetInstance().operationMutex_);
    for (std::vector<DHardWareFwkRegistInfo>::iterator iter =
        DistributedInputClient::GetInstance().dHardWareFwkRstInfos_.begin();
        iter != DistributedInputClient::GetInstance().dHardWareFwkRstInfos_.end();
        ++iter) {
        if (iter->devId == devId && iter->dhId == dhId && iter->callback != nullptr) {
            iter->callback->OnRegisterResult(devId, dhId, status, "");
            DistributedInputClient::GetInstance().dHardWareFwkRstInfos_.erase(iter);
            return;
        }
    }
}

void DistributedInputClient::UnregisterDInputCb::OnResult(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    std::lock_guard<std::mutex> lock(DistributedInputClient::GetInstance().operationMutex_);
    for (std::vector<DHardWareFwkUnRegistInfo>::iterator iter =
        DistributedInputClient::GetInstance().dHardWareFwkUnRstInfos_.begin();
        iter != DistributedInputClient::GetInstance().dHardWareFwkUnRstInfos_.end();
        ++iter) {
        if (iter->devId == devId && iter->dhId == dhId && iter->callback != nullptr) {
            iter->callback->OnUnregisterResult(devId, dhId, status, "");
            DistributedInputClient::GetInstance().dHardWareFwkUnRstInfos_.erase(iter);
            return;
        }
    }
}

void DistributedInputClient::AddWhiteListInfosCb::OnResult(const std::string &deviceId, const std::string &strJson)
{
    if (!strJson.empty()) {
        DistributedInputClient::GetInstance().AddWhiteListInfos(deviceId, strJson);
    }
}

void DistributedInputClient::DelWhiteListInfosCb::OnResult(const std::string &deviceId)
{
    DistributedInputClient::GetInstance().DelWhiteListInfos(deviceId);
}

void DistributedInputClient::GetSinkScreenInfosCb::OnResult(const std::string &strJson)
{
    if (!strJson.empty()) {
        DistributedInputClient::GetInstance().UpdateSinkScreenInfos(strJson);
    }
}

int32_t DistributedInputClient::SharingDhIdListenerCb::OnSharing(const std::string &dhId)
{
    std::lock_guard<std::mutex> lock(DistributedInputClient::GetInstance().sharingDhIdsMtx_);
    DHLOGI("Add Sharing Local dhId: %{public}s", GetAnonyString(dhId).c_str());
    DistributedInputClient::GetInstance().sharingDhIds_.insert(dhId);
    return DH_SUCCESS;
}

int32_t DistributedInputClient::SharingDhIdListenerCb::OnNoSharing(const std::string &dhId)
{
    std::lock_guard<std::mutex> lock(DistributedInputClient::GetInstance().sharingDhIdsMtx_);
    DHLOGI("Remove No Sharing Local dhId: %{public}s", GetAnonyString(dhId).c_str());
    DistributedInputClient::GetInstance().sharingDhIds_.erase(dhId);
    return DH_SUCCESS;
}

DistributedInputClient::DInputClientEventHandler::DInputClientEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner)
{
}

void DistributedInputClient::DInputClientEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        DHLOGE("Event is nullptr");
        return;
    }
    uint32_t eventId = event->GetInnerEventId();
    DHLOGI("DInputClientEventHandler ProcessEvent start eventId:%{public}d.", eventId);
    if (eventId == DINPUT_CLIENT_CHECK_SOURCE_CALLBACK_REGISTER_MSG) {
        DistributedInputClient::GetInstance().CheckSourceRegisterCallback();
        int32_t result = DInputSAManager::GetInstance().RestoreRegisterListenerAndCallback();
        if (result != DH_SUCCESS) {
            DHLOGE("source sa execute RestoreRegisterListenerAndCallback fail, result = %{public}d", result);
        }
        return;
    }

    if (eventId == DINPUT_CLIENT_CHECK_SINK_CALLBACK_REGISTER_MSG) {
        DistributedInputClient::GetInstance().CheckSinkRegisterCallback();
        return;
    }

    if (eventId == DINPUT_CLIENT_CLEAR_SOURCE_CALLBACK_REGISTER_MSG) {
        DHLOGI("Source SA exit, clear callback flag");
        DistributedInputClient::GetInstance().isAddWhiteListCbReg_.store(false);
        DistributedInputClient::GetInstance().isDelWhiteListCbReg_.store(false);
        DistributedInputClient::GetInstance().isNodeMonitorCbReg_.store(false);
        DistributedInputClient::GetInstance().isSimulationEventCbReg_.store(false);
        return;
    }

    if (eventId == DINPUT_CLIENT_CLEAR_SINK_CALLBACK_REGISTER_MSG) {
        DHLOGI("Sink SA exit, clear callback flag");
        DistributedInputClient::GetInstance().isSharingDhIdsReg_.store(false);
        return;
    }
}

void DistributedInputClient::CheckSourceRegisterCallback()
{
    DHLOGI("CheckSourceRegisterCallback called, isAddWhiteListCbReg_[%{public}d], isDelWhiteListCbReg_[%{public}d], "
        "isNodeMonitorCbReg_[%{public}d], isSimulationEventCbReg_[%{public}d]",
        isAddWhiteListCbReg_.load(), isDelWhiteListCbReg_.load(), isNodeMonitorCbReg_.load(),
        isSimulationEventCbReg_.load());

    CheckWhiteListCallback();
    CheckKeyStateCallback();
}

void DistributedInputClient::CheckSinkRegisterCallback()
{
    DHLOGI("CheckSinkRegisterCallback called, isSharingDhIdsReg_[%{public}d]", isSharingDhIdsReg_.load());
    CheckSharingDhIdsCallback();
    CheckSinkScreenInfoCallback();
}

void DistributedInputClient::CheckSharingDhIdsCallback()
{
    if (!DInputSAManager::GetInstance().GetDInputSinkProxy()) {
        DHLOGE("CheckWhiteListCallback client get source proxy fail");
        return;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    if (!isSharingDhIdsReg_.load()) {
        sptr<ISharingDhIdListener> listener(new (std::nothrow) SharingDhIdListenerCb());
        int32_t ret =
            DInputSAManager::GetInstance().dInputSinkProxy_->RegisterSharingDhIdListener(listener);
        if (ret == DH_SUCCESS) {
            isSharingDhIdsReg_.store(true);
            std::lock_guard<std::mutex> lock(operationMutex_);
            sharingDhIdListeners_.insert(listener);
        } else {
            DHLOGE("CheckSharingDhIdsCallback client RegisterSharingDhIdListener fail");
        }
    }
}

void DistributedInputClient::CheckWhiteListCallback()
{
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("CheckWhiteListCallback client get source proxy fail");
        return;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    if (!isAddWhiteListCbReg_.load()) {
        sptr<AddWhiteListInfosCb> addCallback(new (std::nothrow) AddWhiteListInfosCb());
        int32_t ret =
            DInputSAManager::GetInstance().dInputSourceProxy_->RegisterAddWhiteListCallback(addCallback);
        if (ret == DH_SUCCESS) {
            isAddWhiteListCbReg_.store(true);
            std::lock_guard<std::mutex> lock(operationMutex_);
            addWhiteListCallbacks_.insert(addCallback);
        } else {
            DHLOGE("CheckWhiteListCallback client RegisterAddWhiteListCallback fail");
        }
    }
    if (!isDelWhiteListCbReg_.load()) {
        sptr<DelWhiteListInfosCb> delCallback(new (std::nothrow) DelWhiteListInfosCb());
        int32_t ret =
            DInputSAManager::GetInstance().dInputSourceProxy_->RegisterDelWhiteListCallback(delCallback);
        if (ret == DH_SUCCESS) {
            isDelWhiteListCbReg_.store(true);
            std::lock_guard<std::mutex> lock(operationMutex_);
            delWhiteListCallbacks_.insert(delCallback);
        } else {
            DHLOGE("CheckWhiteListCallback client RegisterDelWhiteListCallback fail");
        }
    }
}

void DistributedInputClient::CheckKeyStateCallback()
{
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("CheckKeyStateCallback client get source proxy fail");
        return;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    if (!isSimulationEventCbReg_.load() && regSimulationEventListener_ != nullptr) {
        DInputSAManager::GetInstance().dInputSourceProxy_->RegisterSimulationEventListener(regSimulationEventListener_);
        isSimulationEventCbReg_.store(true);
    }
}

void DistributedInputClient::CheckSinkScreenInfoCallback()
{
    if (!DInputSAManager::GetInstance().GetDInputSinkProxy()) {
        DHLOGE("get sink proxy fail");
        return;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    if (!isGetSinkScreenInfosCbReg_.load()) {
        sptr<GetSinkScreenInfosCb> callback(new (std::nothrow) GetSinkScreenInfosCb());
        int32_t ret =
            DInputSAManager::GetInstance().dInputSinkProxy_->RegisterGetSinkScreenInfosCallback(callback);
        if (ret == DH_SUCCESS) {
            isGetSinkScreenInfosCbReg_.store(true);
            std::lock_guard<std::mutex> lock(operationMutex_);
            getSinkScreenInfosCallbacks_.insert(callback);
        } else {
            DHLOGE("RegisterAddWhiteListCallback fail");
        }
    }
}

int32_t DistributedInputClient::InitSource()
{
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->Init();
}

int32_t DistributedInputClient::InitSink()
{
    if (!DInputSAManager::GetInstance().GetDInputSinkProxy()) {
        return ERR_DH_INPUT_CLIENT_GET_SINK_PROXY_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    return DInputSAManager::GetInstance().dInputSinkProxy_->Init();
}

int32_t DistributedInputClient::ReleaseSource()
{
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }

    serverType_ = DInputServerType::NULL_SERVER_TYPE;
    inputTypes_ = DInputDeviceType::NONE;
    regNodeListener_ = nullptr;
    unregNodeListener_ = nullptr;
    regSimulationEventListener_ = nullptr;
    unregSimulationEventListener_ = nullptr;
    WhiteListUtil::GetInstance().ClearWhiteList();
    {
        std::lock_guard<std::mutex> lock(operationMutex_);
        addWhiteListCallbacks_.clear();
        delWhiteListCallbacks_.clear();
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->Release();
}

int32_t DistributedInputClient::ReleaseSink()
{
    if (!DInputSAManager::GetInstance().GetDInputSinkProxy()) {
        return ERR_DH_INPUT_CLIENT_GET_SINK_PROXY_FAIL;
    }
    serverType_ = DInputServerType::NULL_SERVER_TYPE;
    inputTypes_ = DInputDeviceType::NONE;
    {
        std::lock_guard<std::mutex> lock(operationMutex_);
        getSinkScreenInfosCallbacks_.clear();
        sharingDhIdListeners_.clear();
    }
    WhiteListUtil::GetInstance().ClearWhiteList();
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    return DInputSAManager::GetInstance().dInputSinkProxy_->Release();
}

int32_t DistributedInputClient::RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::string &parameters, const std::shared_ptr<RegisterCallback> &callback)
{
    DHLOGI("DinputRegister called, deviceId: %{public}s,  dhId: %{public}s,  parameters: %{public}s.",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), parameters.c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputRegister client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckRegisterParam(devId, dhId, parameters, callback)) {
        return ERR_DH_INPUT_CLIENT_REGISTER_FAIL;
    }
    {
        std::lock_guard<std::mutex> lock(DistributedInputClient::GetInstance().operationMutex_);
        for (auto iter : dHardWareFwkRstInfos_) {
            if (iter.devId == devId && iter.dhId == dhId) {
                return ERR_DH_INPUT_CLIENT_REGISTER_FAIL;
            }
        }
        DHardWareFwkRegistInfo info {devId, dhId, callback};
        dHardWareFwkRstInfos_.push_back(info);
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->RegisterDistributedHardware(devId, dhId, parameters,
        new(std::nothrow) RegisterDInputCb());
}

int32_t DistributedInputClient::UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::shared_ptr<UnregisterCallback> &callback)
{
    DHLOGI("DinputUnregister called, deviceId: %{public}s,  dhId: %{public}s.",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputUnregister client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckUnregisterParam(devId, dhId, callback)) {
        return ERR_DH_INPUT_CLIENT_UNREGISTER_FAIL;
    }
    {
        std::lock_guard<std::mutex> lock(DistributedInputClient::GetInstance().operationMutex_);
        for (auto iter : dHardWareFwkUnRstInfos_) {
            if (iter.devId == devId && iter.dhId == dhId) {
                return ERR_DH_INPUT_CLIENT_UNREGISTER_FAIL;
            }
        }
        DHardWareFwkUnRegistInfo info {devId, dhId, callback};
        dHardWareFwkUnRstInfos_.push_back(info);
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->UnregisterDistributedHardware(devId, dhId,
        new(std::nothrow) UnregisterDInputCb());
}

int32_t DistributedInputClient::PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> callback)
{
    DHLOGI("DinputPrepare called, deviceId: %{public}s.", GetAnonyString(deviceId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputPrepare client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, callback)) {
        return ERR_DH_INPUT_CLIENT_PREPARE_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->PrepareRemoteInput(deviceId, callback);
}

int32_t DistributedInputClient::UnprepareRemoteInput(const std::string &deviceId,
    sptr<IUnprepareDInputCallback> callback)
{
    DHLOGI("DinputUnprepare called, deviceId: %{public}s.", GetAnonyString(deviceId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputUnprepare client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, callback)) {
        return ERR_DH_INPUT_CLIENT_UNPREPARE_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->UnprepareRemoteInput(deviceId, callback);
}

int32_t DistributedInputClient::StartRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    DHLOGI("DinputStart called, deviceId: %{public}s, inputTypes: %{public}d.",
        GetAnonyString(deviceId).c_str(), inputTypes);
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStart client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, inputTypes, callback)) {
        return ERR_DH_INPUT_CLIENT_START_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StartRemoteInput(deviceId, inputTypes, callback);
}

int32_t DistributedInputClient::StopRemoteInput(const std::string &deviceId, const uint32_t &inputTypes,
    sptr<IStopDInputCallback> callback)
{
    DHLOGI("DinputStop called, deviceId: %{public}s, inputTypes: %{public}d.",
        GetAnonyString(deviceId).c_str(), inputTypes);
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStop client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, inputTypes, callback)) {
        return ERR_DH_INPUT_CLIENT_STOP_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StopRemoteInput(deviceId, inputTypes, callback);
}

int32_t DistributedInputClient::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    DHLOGI("DinputStart called, srcId: %{public}s, sinkId: %{public}s, inputTypes: %{public}d.",
        GetAnonyString(srcId).c_str(), GetAnonyString(sinkId).c_str(), inputTypes);

    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStart relay type client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, inputTypes, callback)) {
        return ERR_DH_INPUT_CLIENT_START_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StartRemoteInput(srcId, sinkId, inputTypes, callback);
}

int32_t DistributedInputClient::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    DHLOGI("DinputStop called, srcId: %{public}s, sinkId: %{public}s, inputTypes: %{public}d.",
        GetAnonyString(srcId).c_str(), GetAnonyString(sinkId).c_str(), inputTypes);
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStop relay type client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, inputTypes, callback)) {
        return ERR_DH_INPUT_CLIENT_STOP_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StopRemoteInput(srcId, sinkId, inputTypes, callback);
}

int32_t DistributedInputClient::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IPrepareDInputCallback> callback)
{
    DHLOGI("DinputPrepare called, srcId: %{public}s, sinkId: %{public}s.", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputPrepare relay proxy error, client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, callback)) {
        return ERR_DH_INPUT_CLIENT_PREPARE_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->PrepareRemoteInput(srcId, sinkId, callback);
}

int32_t DistributedInputClient::UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IUnprepareDInputCallback> callback)
{
    DHLOGI("DinputUnprepare called, srcId: %{public}s, sinkId: %{public}s.", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputUnprepare relay proxy error, client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, callback)) {
        return ERR_DH_INPUT_CLIENT_UNPREPARE_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->UnprepareRemoteInput(srcId, sinkId, callback);
}

int32_t DistributedInputClient::StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    DHLOGI("DinputStart called, sinkId: %{public}s.", GetAnonyString(sinkId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStart dhid proxy error, client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(sinkId, dhIds, callback)) {
        return ERR_DH_INPUT_CLIENT_START_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StartRemoteInput(sinkId, dhIds, callback);
}

int32_t DistributedInputClient::StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    DHLOGI("DinputStop called, sinkId: %{public}s.", GetAnonyString(sinkId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStop dhid proxy error, client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(sinkId, dhIds, callback)) {
        return ERR_DH_INPUT_CLIENT_STOP_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StopRemoteInput(sinkId, dhIds, callback);
}

int32_t DistributedInputClient::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    DHLOGI("DinputStart called, srcId: %{public}s, sinkId: %{public}s.", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStart proxy error, client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, dhIds, callback)) {
        return ERR_DH_INPUT_CLIENT_START_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StartRemoteInput(srcId, sinkId, dhIds, callback);
}

int32_t DistributedInputClient::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    DHLOGI("DinputStop called, srcId: %{public}s, sinkId: %{public}s.", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStop proxy error, client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, dhIds, callback)) {
        return ERR_DH_INPUT_CLIENT_STOP_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->StopRemoteInput(srcId, sinkId, dhIds, callback);
}

bool DistributedInputClient::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    DHLOGI("IsNeedFilterOut called, deviceId: %{public}s", GetAnonyString(deviceId).c_str());
    if (deviceId.empty() || (deviceId.size() > DEV_ID_LENGTH_MAX)) {
        DHLOGE("IsNeedFilterOut param deviceId is empty.");
        return false;
    }
    return WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
}

bool DistributedInputClient::IsTouchEventNeedFilterOut(const TouchScreenEvent &event)
{
    std::lock_guard<std::mutex> lock(operationMutex_);
    for (const auto &info : screenTransInfos_) {
        DHLOGI("sinkProjPhyWidth: %{public}d sinkProjPhyHeight: %{public}d", info.sinkProjPhyWidth,
            info.sinkProjPhyHeight);
        if ((event.absX >= info.sinkWinPhyX) && (event.absX <= (info.sinkWinPhyX + info.sinkProjPhyWidth))
            && (event.absY >= info.sinkWinPhyY)  && (event.absY <= (info.sinkWinPhyY + info.sinkProjPhyHeight))) {
            return true;
        }
    }
    return false;
}

bool DistributedInputClient::IsStartDistributedInput(const std::string &dhId)
{
    std::lock_guard<std::mutex> lock(sharingDhIdsMtx_);
    if (dhId.empty() || (dhId.size() > DH_ID_LENGTH_MAX)) {
        DHLOGE("IsStartDistributedInput param dhid is error.");
        return false;
    }
    return sharingDhIds_.find(dhId) != sharingDhIds_.end();
}

int32_t DistributedInputClient::RegisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    DHLOGI("RegisterSimulationEventListener called Simulation Event Listener Register.");
    if (listener == nullptr) {
        DHLOGE("RegisterSimulationEventListener param error");
        return ERR_DH_INPUT_CLIENT_REG_UNREG_KEY_STATE_FAIL;
    }
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("RegisterSimulationEventListener proxy error, client fail");
        isSimulationEventCbReg_.store(false);
        regSimulationEventListener_ = listener;
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    int32_t ret = DInputSAManager::GetInstance().dInputSourceProxy_->RegisterSimulationEventListener(listener);
    if (ret == DH_SUCCESS) {
        isSimulationEventCbReg_.store(true);
        DInputSAManager::GetInstance().AddSimEventListenerToCache(listener);
    } else {
        isSimulationEventCbReg_.store(false);
        regSimulationEventListener_ = listener;
        DHLOGE("RegisterSimulationEventListener Failed, ret = %{public}d", ret);
    }
    return ret;
}

int32_t DistributedInputClient::UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    DHLOGI("UnregisterSimulationEventListener called Simulation Event Listener UnRegister.");
    if (listener == nullptr) {
        DHLOGE("UnregisterSimulationEventListener param error");
        return ERR_DH_INPUT_CLIENT_REG_UNREG_KEY_STATE_FAIL;
    }
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("UnregisterSimulationEventListener proxy error, client fail");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    int32_t ret = DInputSAManager::GetInstance().dInputSourceProxy_->UnregisterSimulationEventListener(listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("UnregisterSimulationEventListener Failed, ret = %{public}d", ret);
    }
    DInputSAManager::GetInstance().RemoveSimEventListenerFromCache(listener);
    return ret;
}

bool DistributedInputClient::IsJsonData(std::string strData) const
{
    if (strData[0] != '{') {
        return false;
    }

    int num = 1;
    for (size_t i = 1; i < strData.length(); ++i) {
        if (strData[i] == '{') {
            ++num;
        } else if (strData[i] == '}') {
            --num;
        }
        if (num == 0) {
            return true;
        }
    }

    return false;
}

void DistributedInputClient::AddWhiteListInfos(const std::string &deviceId, const std::string &strJson) const
{
    nlohmann::json inputData = nlohmann::json::parse(strJson, nullptr, false);
    if (inputData.is_discarded()) {
        DHLOGE("InputData parse failed!");
        return;
    }
    if (!inputData.is_array()) {
        DHLOGE("inputData not vector!");
        return;
    }
    size_t jsonSize = inputData.size();
    DHLOGI("AddWhiteListInfosCb OnResult deviceId: %{public}s, json str: %{public}s, json size:%{public}zu.\n",
        GetAnonyString(deviceId).c_str(), GetAnonyString(strJson).c_str(), jsonSize);
    TYPE_WHITE_LIST_VEC vecWhiteList = inputData;
    WhiteListUtil::GetInstance().SyncWhiteList(deviceId, vecWhiteList);
}

void DistributedInputClient::DelWhiteListInfos(const std::string &deviceId) const
{
    WhiteListUtil::GetInstance().ClearWhiteList(deviceId);
}

void DistributedInputClient::UpdateSinkScreenInfos(const std::string &strJson)
{
    std::lock_guard<std::mutex> lock(operationMutex_);
    screenTransInfos_.clear();
    nlohmann::json inputData = nlohmann::json::parse(strJson, nullptr, false);
    if (inputData.is_discarded()) {
        DHLOGE("InputData parse failed!");
        return;
    }
    if (!inputData.is_array()) {
        DHLOGE("inputData not vector!");
        return;
    }
    size_t jsonSize = inputData.size();
    DHLOGI("OnResult json str: %{public}s, json size:%{public}zu.\n", GetAnonyString(strJson).c_str(), jsonSize);
    std::vector<std::vector<uint32_t>> transInfos = inputData;
    for (auto info : transInfos) {
        if (info.size() != SINK_SCREEN_INFO_SIZE) {
            DHLOGE("get sinkScreenInfo failed, info size is %{public}zu", info.size());
            continue;
        }
        TransformInfo tmp{info[0], info[1], info[2], info[3]};
        screenTransInfos_.emplace_back(tmp);
        DHLOGI("screenTransInfos_ size %{public}zu", screenTransInfos_.size());
    }
}

int32_t DistributedInputClient::NotifyStartDScreen(const std::string &sinkDevId, const std::string &srcDevId,
    const uint64_t srcWinId)
{
    sptr<IDistributedSinkInput> remoteDInput = GetRemoteDInput(sinkDevId);
    if (remoteDInput == nullptr || !remoteDInput->AsObject()) {
        DHLOGE("GetRemoteDInput failed, networkId = %{public}s", GetAnonyString(sinkDevId).c_str());
        return ERR_DH_INPUT_RPC_GET_REMOTE_DINPUT_FAIL;
    }
    std::string srcScreenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(srcDevId, srcWinId);
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(srcScreenInfoKey);
    DHLOGI("DinputSinkProxy the data: devId: %{public}s, sourceWinId: %{public}" PRIu64 ", sourceWinWidth: %{public}d, "
        "sourceWinHeight: %{public}d, sourcePhyId: %{public}s, sourcePhyFd: %{public}d, sourcePhyWidth: %{public}d, "
        "sourcePhyHeight: %{public}d", GetAnonyString(srcScreenInfo.devId).c_str(), srcScreenInfo.sourceWinId,
        srcScreenInfo.sourceWinWidth, srcScreenInfo.sourceWinHeight, GetAnonyString(srcScreenInfo.sourcePhyId).c_str(),
        srcScreenInfo.sourcePhyFd, srcScreenInfo.sourcePhyWidth, srcScreenInfo.sourcePhyHeight);
    auto ret = remoteDInput->NotifyStartDScreen(srcScreenInfo);
    DHLOGI("NotifyStartDScreen, retCode = %{public}d", ret);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyStartDScreen failed, errCode = %{public}d", ret);
    }
    return ret;
}

int32_t DistributedInputClient::NotifyStopDScreen(const std::string &networkId, const std::string &srcScreenInfoKey)
{
    sptr<IDistributedSinkInput> remoteDInput = GetRemoteDInput(networkId);
    if (remoteDInput == nullptr || !remoteDInput->AsObject()) {
        DHLOGE("GetRemoteDInput failed, networkId = %{public}s", GetAnonyString(networkId).c_str());
        return ERR_DH_INPUT_RPC_GET_REMOTE_DINPUT_FAIL;
    }
    auto ret = remoteDInput->NotifyStopDScreen(srcScreenInfoKey);
    DHLOGI("NotifyStopDScreen, retCode = %{public}d", ret);
    if (ret != DH_SUCCESS) {
        DHLOGE("NotifyStopDScreen failed, errCode = %{public}d", ret);
    }
    return ret;
}

sptr<IDistributedSinkInput> DistributedInputClient::GetRemoteDInput(const std::string &networkId) const
{
    DHLOGI("GetRemoteDInput start, networkId = %{public}s", GetAnonyString(networkId).c_str());
    if (networkId.empty()) {
        DHLOGE("networkId is empty");
        return nullptr;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("GetSystemAbilityManager failed");
        return nullptr;
    }
    auto object = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID, networkId);
    if (object == nullptr) {
        DHLOGE("CheckSystemAbility failed");
        return nullptr;
    }
    return iface_cast<IDistributedSinkInput>(object);
}

int32_t DistributedInputClient::RegisterSessionStateCb(sptr<ISessionStateCallback> callback)
{
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStart client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    if (callback == nullptr) {
        DHLOGE("RegisterSessionStateCb callback is null.");
        return ERR_DH_INPUT_CLIENT_REGISTER_SESSION_STATE_FAIL;
    }
    DInputSAManager::GetInstance().AddSessionStateCbToCache(callback);
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->RegisterSessionStateCb(callback);
}

int32_t DistributedInputClient::UnregisterSessionStateCb()
{
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("DinputStart client fail.");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }
    DInputSAManager::GetInstance().RemoveSessionStateCbFromCache();
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return DInputSAManager::GetInstance().dInputSourceProxy_->UnregisterSessionStateCb();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
