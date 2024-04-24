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

#include "distributed_input_source_manager.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "system_ability_definition.h"
#include "string_ex.h"
#include <unistd.h>

#include "distributed_hardware_fwk_kit.h"
#include "ipublisher_listener.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_hitrace.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "distributed_input_client.h"
#include "distributed_input_inject.h"
#include "distributed_input_source_proxy.h"
#include "distributed_input_source_transport.h"
#include "distributed_input_transport_base.h"
#include "dinput_softbus_define.h"
#include "xcollie/watchdog.h"
#include "hisysevent_util.h"
#include "hidumper.h"
#include "input_check_param.h"
#include "white_list_util.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    constexpr int32_t RAND_NUM_MIN = 0;
    constexpr int32_t RAND_NUM_MAX = 20;
    constexpr int32_t US_PER_MS = 1000;
}
REGISTER_SYSTEM_ABILITY_BY_ID(DistributedInputSourceManager, DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID, true);

DistributedInputSourceManager::DistributedInputSourceManager(int32_t saId, bool runOnCreate)
    : SystemAbility(saId, runOnCreate)
{
    DHLOGI("DistributedInputSourceManager ctor!");
}

DistributedInputSourceManager::~DistributedInputSourceManager()
{
    DHLOGI("DistributedInputSourceManager dtor!");
    startDScreenListener_ = nullptr;
    stopDScreenListener_ = nullptr;
}

DistributedInputSourceManager::DInputSrcMgrListener::DInputSrcMgrListener(DistributedInputSourceManager *manager)
{
    sourceManagerObj_ = manager;
    DHLOGI("DInputSrcMgrListener init.");
}

DistributedInputSourceManager::DInputSrcMgrListener::~DInputSrcMgrListener()
{
    sourceManagerObj_ = nullptr;
    DHLOGI("DInputSrcMgrListener destory.");
}

void DistributedInputSourceManager::DInputSrcMgrListener::ResetSrcMgrResStatus()
{
    DHLOGI("DInputSrcMgrListener ResetSrcMgrResStatus.");
    if (sourceManagerObj_ == nullptr) {
        DHLOGE("ResetSrcMgrResStatus sourceManagerObj is null.");
        return;
    }
    sourceManagerObj_->ClearResourcesStatus();
}

void DistributedInputSourceManager::OnStart()
{
    if (serviceRunningState_ == ServiceSourceRunningState::STATE_RUNNING) {
        DHLOGI("dinput Manager Service has already started.");
        return;
    }
    DHLOGI("dinput Manager Service started.");
    if (!InitAuto()) {
        DHLOGI("failed to init service.");
        return;
    }
    serviceRunningState_ = ServiceSourceRunningState::STATE_RUNNING;
    runner_->Run();

    /*
	 * Publish service maybe failed, so we need call this function at the last,
     * so it can't affect the TDD test program.
     */
    bool ret = Publish(this);
    if (!ret) {
        return;
    }
    DHLOGI("DistributedInputSourceManager start success.");
}

bool DistributedInputSourceManager::InitAuto()
{
    runner_ = AppExecFwk::EventRunner::Create(true);
    if (runner_ == nullptr) {
        return false;
    }
    handler_ = std::make_shared<DistributedInputSourceEventHandler>(runner_);

    DHLOGI("init success");
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    callBackHandler_ = std::make_shared<DInputSourceManagerEventHandler>(runner, this);

    if (OHOS::HiviewDFX::Watchdog::GetInstance().AddThread("dinputsourcewatchdog", callBackHandler_,
        WATCHDOG_INTERVAL_TIME_MS)) {
        DHLOGE("HiviewDFX::Watchdog::GetInstance().AddThread() Failed.");
    }
    return true;
}

void DistributedInputSourceManager::OnStop()
{
    DHLOGI("stop service");
    runner_.reset();
    handler_.reset();
    serviceRunningState_ = ServiceSourceRunningState::STATE_NOT_START;
}

int32_t DistributedInputSourceManager::Init()
{
    DHLOGI("enter");
    isStartTrans_ = DInputServerType::NULL_SERVER_TYPE;

    // transport init session
    int32_t ret = DistributedInputSourceTransport::GetInstance().Init();
    if (ret != DH_SUCCESS) {
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_INIT_FAIL;
    }

    statuslistener_ = std::make_shared<DInputSourceListener>(this);
    DistributedInputSourceTransport::GetInstance().RegisterSourceRespCallback(statuslistener_);
    srcMgrListener_ = std::make_shared<DInputSrcMgrListener>(this);
    DistributedInputTransportBase::GetInstance().RegisterSourceManagerCallback(srcMgrListener_);

    serviceRunningState_ = ServiceSourceRunningState::STATE_RUNNING;
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit == nullptr) {
        DHLOGE("dhFwkKit obtain fail!");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_INIT_FAIL;
    }
    startDScreenListener_ = new(std::nothrow) StartDScreenListener;
    stopDScreenListener_ = new(std::nothrow) StopDScreenListener;
    deviceOfflineListener_ = new(std::nothrow) DeviceOfflineListener(this);
    dhFwkKit->RegisterPublisherListener(DHTopic::TOPIC_START_DSCREEN, startDScreenListener_);
    dhFwkKit->RegisterPublisherListener(DHTopic::TOPIC_STOP_DSCREEN, stopDScreenListener_);
    dhFwkKit->RegisterPublisherListener(DHTopic::TOPIC_DEV_OFFLINE, deviceOfflineListener_);

    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::Release()
{
    DHLOGI("Release source manager.");
    for (auto iter = inputDevice_.begin(); iter != inputDevice_.end(); ++iter) {
        std::string devId = iter->devId;
        std::string dhId = iter->dhId;
        DHLOGI("Release devId: %{public}s, dhId: %{public}s.", GetAnonyString(devId).c_str(),
            GetAnonyString(dhId).c_str());
        int32_t ret = DistributedInputInject::GetInstance().UnregisterDistributedHardware(devId, dhId);
        if (ret != DH_SUCCESS) {
            DHLOGE("DinputSourceManager Release called, remove node fail.");
        }
    }
    DistributedInputSourceTransport::GetInstance().Release();
    inputDevice_.clear();
    DeviceMap_.clear();
    InputTypesMap_.clear();

    std::shared_ptr<nlohmann::json> jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = static_cast<int32_t>(DInputServerType::NULL_SERVER_TYPE);
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_STARTSERVER_MSG, jsonArrayMsg, 0);
    if (callBackHandler_ != nullptr) {
        DHLOGI("Sourcemanager send event success.");
        callBackHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
    serviceRunningState_ = ServiceSourceRunningState::STATE_NOT_START;
    UnregisterDHFwkPublisher();

    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_EXIT, "dinput source sa exit success.");
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        DHLOGE("Failed to get SystemAbilityManager.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_RELEASE_FAIL;
    }
    int32_t ret = systemAbilityMgr->UnloadSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID);
    if (ret != DH_SUCCESS) {
        DHLOGE("Failed to UnloadSystemAbility service! errcode: %{public}d.", ret);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_RELEASE_FAIL;
    }
    DHLOGI("Source unloadSystemAbility successfully.");
    return DH_SUCCESS;
}

void DistributedInputSourceManager::UnregisterDHFwkPublisher()
{
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr && startDScreenListener_ != nullptr) {
        DHLOGI("UnPublish StartDScreenListener");
        dhFwkKit->UnregisterPublisherListener(DHTopic::TOPIC_START_DSCREEN, startDScreenListener_);
    }
    if (dhFwkKit != nullptr && stopDScreenListener_ != nullptr) {
        DHLOGI("UnPublish StopDScreenListener");
        dhFwkKit->UnregisterPublisherListener(DHTopic::TOPIC_STOP_DSCREEN, stopDScreenListener_);
    }
    if (dhFwkKit != nullptr && deviceOfflineListener_ != nullptr) {
        DHLOGI("UnPublish DeviceOfflineListener");
        dhFwkKit->UnregisterPublisherListener(DHTopic::TOPIC_DEV_OFFLINE, deviceOfflineListener_);
    }
    if (dhFwkKit != nullptr) {
        DHLOGD("Disable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, DISABLE_LOW_LATENCY.dump());
    }
}

bool DistributedInputSourceManager::CheckRegisterParam(const std::string &devId, const std::string &dhId,
    const std::string &parameters, sptr<IRegisterDInputCallback> callback)
{
    if (devId.empty() || devId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam devId is empty or devId size too long.");
        return false;
    }
    if (dhId.empty() || dhId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam dhId is empty or dhId size too long.");
        return false;
    }
    if (parameters.empty()) {
        DHLOGE("CheckParam parameters is empty.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

int32_t DistributedInputSourceManager::RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::string &parameters, sptr<IRegisterDInputCallback> callback)
{
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_REGISTER, devId, dhId, "dinput register call.");
    if (!CheckRegisterParam(devId, dhId, parameters, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_REGISTER_FAIL, devId, dhId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL, "Dinputregister failed callback is nullptr.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL;
    }
    DHLOGI("RegisterDistributedHardware called, deviceId: %{public}s, dhId: %{public}s, parameters: %{public}s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), SetAnonyId(parameters).c_str());
    int32_t randNumber = GetRandomInt32(RAND_NUM_MIN, RAND_NUM_MAX);
    usleep(randNumber * US_PER_MS);

    std::lock_guard<std::mutex> lock(regDisHardwareMutex_);
    DInputClientRegistInfo info {devId, dhId, callback};
    regCallbacks_.push_back(info);
    InputDeviceId inputDeviceId {devId, dhId, GetNodeDesc(parameters)};

    // 1.Find out if the dh exists
    auto it  = std::find(inputDevice_.begin(), inputDevice_.end(), inputDeviceId);
    if (it != inputDevice_.end()) {
        callback->OnResult(devId, dhId, DH_SUCCESS);
        return DH_SUCCESS;
    }

    // 2.create input node
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedHardware(devId, dhId, parameters);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_REGISTER_FAIL, devId, dhId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL, "Dinputregister failed in create input node.");
        DHLOGE("RegisterDistributedHardware called, create node fail.");

        for (auto iter = regCallbacks_.begin(); iter != regCallbacks_.end(); ++iter) {
            if (iter->devId == devId && iter->dhId == dhId) {
                iter->callback->OnResult(iter->devId, iter->dhId, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL);
                regCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL;
    }

    // 3.save device
    DHLOGI("inputDevice push deviceId: %{public}s, dhId: %{public}s", GetAnonyString(inputDeviceId.devId).c_str(),
        GetAnonyString(inputDeviceId.dhId).c_str());
    inputDevice_.push_back(inputDeviceId);

    // 4.notify source distributedfwk register hardware success
    callback->OnResult(devId, dhId, DH_SUCCESS);

    // 5. Notify node mgr to scan vir dev node info
    DistributedInputInject::GetInstance().NotifyNodeMgrScanVirNode(devId, dhId);
    return DH_SUCCESS;
}

void DistributedInputSourceManager::handleStartServerCallback(const std::string &devId)
{
    bool isFindDevice = false;
    for (auto iter = inputDevice_.begin(); iter != inputDevice_.end(); ++iter) {
        if (devId == iter->devId) {
            isFindDevice = true;
            break;
        }
    }
    if (!isFindDevice) {
        DeviceMap_[devId] = DINPUT_SOURCE_SWITCH_OFF;
        // DeviceMap_ all sink device switch is off,call isstart's callback
        bool isAllDevSwitchOff = true;
        for (auto it = DeviceMap_.begin(); it != DeviceMap_.end(); ++it) {
            if (it->second == DINPUT_SOURCE_SWITCH_ON) {
                isAllDevSwitchOff = false;
                break;
            }
        }
        if (isAllDevSwitchOff) {
            std::shared_ptr<nlohmann::json> jsonArrayMsg = std::make_shared<nlohmann::json>();
            nlohmann::json tmpJson;
            tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = static_cast<int32_t>(DInputServerType::NULL_SERVER_TYPE);
            jsonArrayMsg->push_back(tmpJson);
            AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
                DINPUT_SOURCE_MANAGER_STARTSERVER_MSG, jsonArrayMsg, 0);

            if (callBackHandler_ == nullptr) {
                DHLOGE("handleStartServerCallback callBackHandler_ is null.");
                return;
            }
            callBackHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
    }
}

int32_t DistributedInputSourceManager::UnregCallbackNotify(const std::string &devId, const std::string &dhId)
{
    for (auto iter = unregCallbacks_.begin(); iter != unregCallbacks_.end(); ++iter) {
        if (iter->devId == devId && iter->dhId == dhId) {
            iter->callback->OnResult(iter->devId, iter->dhId, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL);
            unregCallbacks_.erase(iter);
            return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
        }
    }
    return DH_SUCCESS;
}

bool DistributedInputSourceManager::CheckUnregisterParam(const std::string &devId, const std::string &dhId,
    sptr<IUnregisterDInputCallback> callback)
{
    if (devId.empty() || devId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam devId is empty or devId size too long.");
        return false;
    }
    if (dhId.empty() || dhId.size() > DEV_ID_LENGTH_MAX) {
        DHLOGE("CheckParam dhId is empty or dhId size too long.");
        return false;
    }
    if (callback == nullptr) {
        DHLOGE("CheckParam callback is null.");
        return false;
    }
    return true;
}

int32_t DistributedInputSourceManager::CheckDeviceIsExists(const std::string &devId, const std::string &dhId,
    const InputDeviceId &inputDeviceId, std::vector<InputDeviceId>::iterator &it)
{
    for (; it != inputDevice_.end(); ++it) {
        if (it->devId == inputDeviceId.devId && it->dhId == inputDeviceId.dhId) {
            break;
        }
    }

    if (it == inputDevice_.end()) {
        DHLOGE("CheckDevice called, deviceId: %{public}s is not exist.", GetAnonyString(devId).c_str());
        if (UnregCallbackNotify(devId, dhId) != DH_SUCCESS) {
            return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::DeleteInputDeviceNodeInfo(const std::string &devId, const std::string &dhId,
    const std::vector<InputDeviceId>::iterator &it)
{
    int32_t ret = DistributedInputInject::GetInstance().UnregisterDistributedHardware(devId, dhId);
    if (ret != DH_SUCCESS) {
        DHLOGE("RemoveInputNode called, remove node fail.");
        if (UnregCallbackNotify(devId, dhId) != DH_SUCCESS) {
            return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REMOVE_INPUT_NODE_FAIL;
    }

    inputDevice_.erase(it);
    std::shared_ptr<nlohmann::json> jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_SOURCEMANAGER_KEY_DEVID] = devId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_HWID] = dhId;
    tmpJson[INPUT_SOURCEMANAGER_KEY_RESULT] = true;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_SOURCE_MANAGER_UNRIGISTER_MSG, jsonArrayMsg, 0);

    if (callBackHandler_ == nullptr) {
        DHLOGE("UnregisterDistributedHardware callBackHandler_ is null.");
        if (UnregCallbackNotify(devId, dhId) != DH_SUCCESS) {
            return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_DELETE_DEVICE_FAIL;
    }
    callBackHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
    sptr<IUnregisterDInputCallback> callback)
{
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_UNREGISTER, devId, dhId, "dinput unregister call");
    if (!CheckUnregisterParam(devId, dhId, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_UNREGISTER_FAIL, devId, dhId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL, "dinput unregister failed in callback is nullptr");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
    }
    DHLOGI("Unregister called, deviceId: %{public}s,  dhId: %{public}s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(regDisHardwareMutex_);
    DInputClientUnregistInfo info {devId, dhId, callback};
    unregCallbacks_.push_back(info);

    InputDeviceId inputDeviceId {devId, dhId};
    auto it = inputDevice_.begin();
    if (CheckDeviceIsExists(devId, dhId, inputDeviceId, it) != DH_SUCCESS) {
        DHLOGE("Unregister deviceId: %{public}s is not exist.", GetAnonyString(devId).c_str());
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_UNREGISTER_FAIL, devId, dhId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL, "dinput unregister failed in deviceId is not exist");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
    }

    if (DeleteInputDeviceNodeInfo(devId, dhId, it) != DH_SUCCESS) {
        DHLOGE("Unregister deviceId: %{public}s, delete device node failed", GetAnonyString(devId).c_str());
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_UNREGISTER_FAIL, devId, dhId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL, "dinput unregister failed in delete input node");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL;
    }

    HiDumper::GetInstance().DeleteNodeInfo(devId, dhId);

    // isstart callback
    handleStartServerCallback(devId);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::PrepareRemoteInput(
    const std::string &deviceId, sptr<IPrepareDInputCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_PREPARE_START, DINPUT_PREPARE_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_PREPARE, deviceId, "Dinput prepare call.");
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, "Dinput prepare param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_PREPARE_START, DINPUT_PREPARE_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }

    DHLOGI("Prepare called, deviceId: %{public}s", GetAnonyString(deviceId).c_str());
    int32_t ret = DistributedInputSourceTransport::GetInstance().OpenInputSoftbus(deviceId, false);
    if (ret != DH_SUCCESS) {
        DHLOGE("Open softbus session fail, ret: %{public}d", ret);
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, "Dinput prepare failed in open softbus");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_PREPARE_START, DINPUT_PREPARE_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }

    DInputClientPrepareInfo info {deviceId, callback};
    AddPrepareCallbacks(info);

    ret = DistributedInputSourceTransport::GetInstance().PrepareRemoteInput(deviceId);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, "Dinput prepare failed in transport prepare");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_PREPARE_START, DINPUT_PREPARE_TASK);
        DHLOGE("Can not send message by softbus, prepare fail, ret: %{public}d", ret);
        info.preCallback->OnResult(deviceId, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL);
        RemovePrepareCallbacks(info);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::UnprepareRemoteInput(
    const std::string &deviceId, sptr<IUnprepareDInputCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_UNPREPARE_START, DINPUT_UNPREPARE_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_UNPREPARE, deviceId, "Dinput unprepare call.");
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, "Dinput unprepare param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_UNPREPARE_START, DINPUT_UNPREPARE_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL;
    }
    DHLOGI("Unprepare called, deviceId: %{public}s", GetAnonyString(deviceId).c_str());
    DInputClientUnprepareInfo info {deviceId, callback};
    AddUnPrepareCallbacks(info);
    int32_t ret = DistributedInputSourceTransport::GetInstance().UnprepareRemoteInput(deviceId);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, unprepare fail, ret: %{public}d", ret);
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, "Dinput unprepare failed in transport unprepare.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_UNPREPARE_START, DINPUT_UNPREPARE_TASK);
        info.unpreCallback->OnResult(deviceId, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL);
        RemoveUnPrepareCallbacks(info);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StartRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_START_USE, deviceId, "Dinput start use call.");
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, inputTypes, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }

    DHLOGI("Start called, deviceId: %{public}s, inputTypes: %{public}d", GetAnonyString(deviceId).c_str(), inputTypes);
    for (auto iter : staCallbacks_) {
        if (iter.devId == deviceId && iter.inputTypes == inputTypes) {
            callback->OnResult(deviceId, inputTypes, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL);
            HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
                ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start use failed in already started.");
            FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
            return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
        }
    }

    DInputClientStartInfo info {deviceId, inputTypes, callback};
    staCallbacks_.push_back(info);
    DeviceMap_[deviceId] = DINPUT_SOURCE_SWITCH_OFF;
    int32_t ret = DistributedInputSourceTransport::GetInstance().StartRemoteInput(deviceId, inputTypes);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start use failed in transport start");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        DHLOGE("Start fail.");
        for (auto iter = staCallbacks_.begin(); iter != staCallbacks_.end(); ++iter) {
            if (iter->devId == deviceId && iter->inputTypes == inputTypes) {
                iter->callback->OnResult(iter->devId, iter->inputTypes, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL);
                staCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StopRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_STOP_USE, deviceId, "Dinput stop use call");
    if (!DInputCheckParam::GetInstance().CheckParam(deviceId, inputTypes, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }

    DHLOGI("Stop called, deviceId: %{public}s, inputTypes: %{public}d", GetAnonyString(deviceId).c_str(), inputTypes);
    for (auto iter : stpCallbacks_) {
        if (iter.devId == deviceId && iter.inputTypes == inputTypes) {
            callback->OnResult(deviceId, inputTypes, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL);
            HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
                ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop use failed in already stoped.");
            FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
            return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
        }
    }

    DInputClientStopInfo info {deviceId, inputTypes, callback};
    stpCallbacks_.push_back(info);
    int32_t ret = DistributedInputSourceTransport::GetInstance().StopRemoteInput(deviceId, inputTypes);
    if (ret != DH_SUCCESS) {
        DHLOGE("Stop fail.");
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, deviceId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop use failed in transport stop.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        for (auto iter = stpCallbacks_.begin(); iter != stpCallbacks_.end(); ++iter) {
            if (iter->devId == deviceId && iter->inputTypes == inputTypes) {
                iter->callback->OnResult(iter->devId, iter->inputTypes, ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL);
                stpCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_START_USE, sinkId, "Dinput start use call.");
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, inputTypes, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }

    DHLOGI("StartRemoteInput called, srcId: %{public}s, sinkId: %{public}s, inputTypes: %{public}d",
        GetAnonyString(srcId).c_str(), GetAnonyString(sinkId).c_str(), inputTypes);
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start use failed in get local networkId error.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    if (srcId != localNetworkId) {
        return RelayStartRemoteInputByType(srcId, sinkId, inputTypes, callback);
    }

    DInputClientStartInfo info {sinkId, inputTypes, callback};
    staCallbacks_.push_back(info);
    DeviceMap_[sinkId] = DINPUT_SOURCE_SWITCH_OFF; // when sink device start success,set DINPUT_SOURCE_SWITCH_ON
    int32_t ret = DistributedInputSourceTransport::GetInstance().StartRemoteInput(sinkId, inputTypes);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start use failed in transport start.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        DHLOGE("StartRemoteInput called, start fail.");
        for (auto it = staCallbacks_.begin(); it != staCallbacks_.end(); ++it) {
            if (it->devId == sinkId && it->inputTypes == inputTypes) {
                staCallbacks_.erase(it);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_STOP_USE, sinkId, "Dinput stop use call.");
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, inputTypes, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    DHLOGI("StopRemoteInput called, srcId: %{public}s, sinkId: %{public}s, inputTypes: %{public}d",
        GetAnonyString(srcId).c_str(), GetAnonyString(sinkId).c_str(), inputTypes);
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop use failed in get networkId.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    if (srcId != localNetworkId) {
        return RelayStopRemoteInputByType(srcId, sinkId, inputTypes, callback);
    }

    DInputClientStopInfo info {sinkId, inputTypes, callback};
    stpCallbacks_.push_back(info);
    int32_t ret = DistributedInputSourceTransport::GetInstance().StopRemoteInput(sinkId, inputTypes);
    if (ret != DH_SUCCESS) {
        DHLOGE("StopRemoteInput called, stop fail.");
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop use failed in transport stop.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        for (auto it = stpCallbacks_.begin(); it != stpCallbacks_.end(); ++it) {
            if (it->devId == sinkId && it->inputTypes == inputTypes) {
                stpCallbacks_.erase(it);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RelayStartRemoteInputByType(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    std::lock_guard<std::mutex> lock(startStopMutex_);
    DInputClientStartTypeInfo info(srcId, sinkId, inputTypes, callback);
    relayStaTypeCallbacks_.push_back(info);

    int32_t ret = DistributedInputSourceTransport::GetInstance().SendRelayStartTypeRequest(srcId, sinkId, inputTypes);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        for (auto iter = relayStaTypeCallbacks_.begin(); iter != relayStaTypeCallbacks_.end(); ++iter) {
            if (iter->srcId == srcId && iter->sinkId == sinkId && iter->inputTypes == inputTypes) {
                relayStaTypeCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RelayStopRemoteInputByType(
    const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    std::lock_guard<std::mutex> lock(startStopMutex_);
    DInputClientStopTypeInfo info(srcId, sinkId, inputTypes, callback);
    relayStpTypeCallbacks_.push_back(info);

    int32_t ret = DistributedInputSourceTransport::GetInstance().SendRelayStopTypeRequest(srcId, sinkId, inputTypes);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        for (auto iter = relayStpTypeCallbacks_.begin(); iter != relayStpTypeCallbacks_.end(); ++iter) {
            if (iter->srcId == srcId && iter->sinkId == sinkId && iter->inputTypes == inputTypes) {
                relayStpTypeCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }

    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IPrepareDInputCallback> callback)
{
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, "Dinput prepare param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    DHLOGI("Dinput prepare, srcId: %{public}s, sinkId: %{public}s", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    if (srcId != localNetworkId) {
        return RelayPrepareRemoteInput(srcId, sinkId, callback);
    }
    // current device is source device
    int32_t ret = DistributedInputSourceTransport::GetInstance().OpenInputSoftbus(sinkId, false);
    if (ret != DH_SUCCESS) {
        DHLOGE("Open softbus session fail ret=%{public}d.", ret);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    DInputClientPrepareInfo info {sinkId, callback};
    AddPrepareCallbacks(info);

    ret = DistributedInputSourceTransport::GetInstance().PrepareRemoteInput(sinkId);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, prepare fail, ret: %{public}d", ret);
        RemovePrepareCallbacks(info);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IUnprepareDInputCallback> callback)
{
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, "Dinput unprepare param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL;
    }
    DHLOGI("Dinput unprepare, srcId: %{public}s, sinkId: %{public}s", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL;
    }
    if (srcId != localNetworkId) {
        return RelayUnprepareRemoteInput(srcId, sinkId, callback);
    }

    // current device is source device
    DInputClientUnprepareInfo info {sinkId, callback};
    AddUnPrepareCallbacks(info);
    int32_t ret = DistributedInputSourceTransport::GetInstance().UnprepareRemoteInput(sinkId);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, unprepare fail, ret: %{public}d", ret);
        RemoveUnPrepareCallbacks(info);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL;
    }
    return DH_SUCCESS;
}

bool DistributedInputSourceManager::IsStringDataSame(const std::vector<std::string> &oldDhIds,
    std::vector<std::string> newDhIds)
{
    if (oldDhIds.size() != newDhIds.size()) {
        DHLOGI("Size is not same, return false.");
        return false;
    }
    bool isSame = true;
    for (auto oDhid : oldDhIds) {
        auto it = find(newDhIds.begin(), newDhIds.end(), oDhid);
        if (it == newDhIds.end()) {
            isSame = false;
            break;
        }
    }
    DHLOGI("IsSame: %{public}d.", isSame);
    return isSame;
}

int32_t DistributedInputSourceManager::StartRemoteInput(const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_START_USE, sinkId, "dinput start use call");
    if (!DInputCheckParam::GetInstance().CheckParam(sinkId, dhIds, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    DHLOGI("Dinput start, sinkId: %{public}s, vector.string.size: %{public}zu", GetAnonyString(sinkId).c_str(),
        dhIds.size());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "dinput start use failed in get networkId");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }

    // current device is source device
    DInputClientStartDhidInfo info {localNetworkId, sinkId, dhIds, callback};
    staStringCallbacks_.push_back(info);
    DeviceMap_[sinkId] = DINPUT_SOURCE_SWITCH_OFF; // when sink device start success,set DINPUT_SOURCE_SWITCH_ON
    int32_t ret = DistributedInputSourceTransport::GetInstance().StartRemoteInput(sinkId, dhIds);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "dinput start use failed in transport start");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        DHLOGE("StartRemoteInput start fail.");
        for (auto iter = staStringCallbacks_.begin(); iter != staStringCallbacks_.end(); ++iter) {
            if (iter->sinkId == sinkId && IsStringDataSame(iter->dhIds, dhIds)) {
                staStringCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_STOP_USE, sinkId, "dinput stop use call");
    if (!DInputCheckParam::GetInstance().CheckParam(sinkId, dhIds, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    DHLOGI("Dinput stop, sinkId: %{public}s, vector.string.size: %{public}zu", GetAnonyString(sinkId).c_str(),
        dhIds.size());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "dinput stop use failed in get networkId");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }

    DInputClientStopDhidInfo info {localNetworkId, sinkId, dhIds, callback};
    stpStringCallbacks_.push_back(info);
    int32_t ret = DistributedInputSourceTransport::GetInstance().StopRemoteInput(sinkId, dhIds);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "dinput stop use failed in transport stop");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        DHLOGE("StopRemoteInput stop fail.");
        for (auto iter = stpStringCallbacks_.begin(); iter != stpStringCallbacks_.end(); ++iter) {
            if (iter->sinkId == sinkId && IsStringDataSame(iter->dhIds, dhIds)) {
                stpStringCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_START_USE, sinkId, "Dinput start use call.");
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, dhIds, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    DHLOGI("Dinput start, srcId: %{public}s, sinkId: %{public}s", GetAnonyString(srcId).c_str(),
        GetAnonyString(sinkId).c_str());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start use failed in get networkId.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    if (srcId != localNetworkId) {
        return RelayStartRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    }

    DInputClientStartDhidInfo info {srcId, sinkId, dhIds, callback};
    staStringCallbacks_.push_back(info);
    DeviceMap_[sinkId] = DINPUT_SOURCE_SWITCH_OFF;
    int32_t ret = DistributedInputSourceTransport::GetInstance().StartRemoteInput(sinkId, dhIds);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, "Dinput start use failed in transport start.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
        DHLOGE("StartRemoteInput start fail.");
        for (auto iter = staStringCallbacks_.begin(); iter != staStringCallbacks_.end(); ++iter) {
            if (iter->srcId == srcId && iter->sinkId == sinkId && IsStringDataSame(iter->dhIds, dhIds)) {
                staStringCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
    HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_STOP_USE, sinkId, "Dinput stop use call.");
    if (!DInputCheckParam::GetInstance().CheckParam(srcId, sinkId, dhIds, callback)) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop param is failed.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    DHLOGI("Dinput stop, srcId: %{public}s, sinkId: %{public}s, vector.string.size: %{public}zu",
        GetAnonyString(srcId).c_str(), GetAnonyString(sinkId).c_str(), dhIds.size());
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop use failed in get networkId.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    if (srcId != localNetworkId) {
        return RelayStopRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    }

    DInputClientStopDhidInfo info {srcId, sinkId, dhIds, callback};
    stpStringCallbacks_.push_back(info);
    int32_t ret = DistributedInputSourceTransport::GetInstance().StopRemoteInput(sinkId, dhIds);
    if (ret != DH_SUCCESS) {
        HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_OPT_FAIL, sinkId,
            ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, "Dinput stop use failed in transport stop.");
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
        DHLOGE("StopRemoteInput stop fail.");
        for (auto iter = stpStringCallbacks_.begin(); iter != stpStringCallbacks_.end(); ++iter) {
            if (iter->srcId == srcId && iter->sinkId == sinkId && IsStringDataSame(iter->dhIds, dhIds)) {
                stpStringCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RegisterAddWhiteListCallback(sptr<IAddWhiteListInfosCallback> callback)
{
    DHLOGI("RegisterAddWhiteListCallback called.");
    if (callback == nullptr) {
        DHLOGE("RegisterAddWhiteListCallback callback is null.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REG_CALLBACK_ERR;
    }
    std::lock_guard<std::mutex> lock(whiteListMutex_);
    addWhiteListCallbacks_.insert(callback);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RegisterDelWhiteListCallback(sptr<IDelWhiteListInfosCallback> callback)
{
    DHLOGI("RegisterDelWhiteListCallback called.");
    if (callback == nullptr) {
        DHLOGE("RegisterDelWhiteListCallback callback is null.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REG_CALLBACK_ERR;
    }
    std::lock_guard<std::mutex> lock(whiteListMutex_);
    delWhiteListCallbacks_.insert(callback);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RegisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    DHLOGI("RegisterSimulationEventListener called.");
    if (listener == nullptr) {
        DHLOGE("RegisterSimulationEventListener callback is null.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_SIMULATION_EVENT_CALLBACK_ERR;
    }
    std::lock_guard<std::mutex> lock(simEventMutex_);
    this->simulationEventCallbacks_.insert(listener);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    DHLOGI("UnregisterSimulationEventListener called.");
    if (listener == nullptr) {
        DHLOGE("UnregisterSimulationEventListener callback is null.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_SIMULATION_EVENT_CALLBACK_ERR;
    }
    std::lock_guard<std::mutex> lock(simEventMutex_);
    this->simulationEventCallbacks_.erase(listener);
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RegisterSessionStateCb(sptr<ISessionStateCallback> callback)
{
    if (callback == nullptr) {
        DHLOGE("RegisterSessionStateCb callback is null.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_SESSION_STATE_CB_IS_NULL;
    }
    DistributedInputTransportBase::GetInstance().RegisterSessionStateCb(callback);
    DistributedInputInject::GetInstance().RegisterInjectEventCb(callback);
    return DH_SUCCESS;
}
int32_t DistributedInputSourceManager::UnregisterSessionStateCb()
{
    DistributedInputTransportBase::GetInstance().UnregisterSessionStateCb();
    DistributedInputInject::GetInstance().UnregisterInjectEventCb();
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RelayPrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IPrepareDInputCallback> callback)
{
    int32_t ret = DistributedInputSourceTransport::GetInstance().OpenInputSoftbus(srcId, true);
    if (ret != DH_SUCCESS) {
        DHLOGE("Open softbus session fail.");
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    DInputClientRelayPrepareInfo info(srcId, sinkId, callback);
    AddRelayPrepareCallbacks(info);

    ret = DistributedInputSourceTransport::GetInstance().SendRelayPrepareRequest(srcId, sinkId);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, prepare fail, ret: %{public}d", ret);
        RemoveRelayPrepareCallbacks(info);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RelayUnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
    sptr<IUnprepareDInputCallback> callback)
{
    DInputClientRelayUnprepareInfo info(srcId, sinkId, callback);
    AddRelayUnPrepareCallbacks(info);

    int32_t ret = DistributedInputSourceTransport::GetInstance().SendRelayUnprepareRequest(srcId, sinkId);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, prepare fail, ret: %{public}d", ret);
        RemoveRelayUnPrepareCallbacks(info);
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RelayStartRemoteInputByDhid(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    std::lock_guard<std::mutex> lock(startStopMutex_);
    DInputClientStartDhidInfo info{srcId, sinkId, dhIds, callback};
    relayStaDhidCallbacks_.push_back(info);

    int32_t ret = DistributedInputSourceTransport::GetInstance().SendRelayStartDhidRequest(srcId, sinkId, dhIds);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, start fail.");
        for (auto iter = relayStaDhidCallbacks_.begin(); iter != relayStaDhidCallbacks_.end(); ++iter) {
            if (iter->srcId == srcId && iter->sinkId == sinkId && IsStringDataSame(iter->dhIds, dhIds)) {
                relayStaDhidCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL;
    }
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::RelayStopRemoteInputByDhid(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    std::lock_guard<std::mutex> lock(startStopMutex_);
    DInputClientStopDhidInfo info{srcId, sinkId, dhIds, callback};
    relayStpDhidCallbacks_.push_back(info);

    int32_t ret = DistributedInputSourceTransport::GetInstance().SendRelayStopDhidRequest(srcId, sinkId, dhIds);
    if (ret != DH_SUCCESS) {
        DHLOGE("Can not send message by softbus, stop fail.");
        for (auto iter = relayStpDhidCallbacks_.begin(); iter != relayStpDhidCallbacks_.end(); ++iter) {
            if (iter->srcId == srcId && iter->sinkId == sinkId && IsStringDataSame(iter->dhIds, dhIds)) {
                relayStpDhidCallbacks_.erase(iter);
                return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
            }
        }
        return ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL;
    }
    return DH_SUCCESS;
}

void DistributedInputSourceManager::RunRegisterCallback(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    std::lock_guard<std::mutex> lock(regDisHardwareMutex_);
    for (auto iter = regCallbacks_.begin(); iter != regCallbacks_.end(); ++iter) {
        if (iter->devId == devId && iter->dhId == dhId) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RIGISTER_MSG");
            iter->callback->OnResult(devId, dhId, status);
            regCallbacks_.erase(iter);
            return;
        }
    }
    DHLOGE("ProcessEvent registerCallback is null.");
}

void DistributedInputSourceManager::RunUnregisterCallback(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    std::lock_guard<std::mutex> lock(regDisHardwareMutex_);
    for (auto iter = unregCallbacks_.begin(); iter != unregCallbacks_.end(); ++iter) {
        if (iter->devId == devId && iter->dhId == dhId) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_UNRIGISTER_MSG");
            iter->callback->OnResult(devId, dhId, status);
            unregCallbacks_.erase(iter);
            return;
        }
    }
    DHLOGE("ProcessEvent unregisterCallback is null.");
}

void DistributedInputSourceManager::RunPrepareCallback(
    const std::string &devId, const int32_t &status, const std::string &object)
{
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_PREPARE_START, DINPUT_PREPARE_TASK);
    std::lock_guard<std::mutex> lock(prepareMutex_);
    for (auto iter = preCallbacks_.begin(); iter != preCallbacks_.end(); ++iter) {
        if (iter->devId == devId) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_PREPARE_MSG");
            iter->preCallback->OnResult(devId, status);
            preCallbacks_.erase(iter);
            RunWhiteListCallback(devId, object);
            return;
        }
    }
    DHLOGE("ProcessEvent parepareCallback is null.");
}

void DistributedInputSourceManager::RunWhiteListCallback(const std::string &devId, const std::string &object)
{
    std::lock_guard<std::mutex> lock(whiteListMutex_);
    if (addWhiteListCallbacks_.size() == 0) {
        DHLOGE("addWhiteListCallbacks_ is empty.");
        return;
    }
    for (const auto &it : addWhiteListCallbacks_) {
        it->OnResult(devId, object);
    }
}

void DistributedInputSourceManager::RunRelayPrepareCallback(const std::string &srcId, const std::string &sinkId,
    const int32_t status)
{
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_PREPARE_START, DINPUT_PREPARE_TASK);
    std::lock_guard<std::mutex> lock(prepareMutex_);
    bool isCbRun = false;
    sptr<IPrepareDInputCallback> cb = nullptr;
    for (auto iter = relayPreCallbacks_.begin(); iter != relayPreCallbacks_.end(); ++iter) {
        if (iter->srcId == srcId && iter->sinkId == sinkId) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_MMI");
            cb = iter->preCallback;
            relayPreCallbacks_.erase(iter);
            isCbRun = true;
            break;
        }
    }
    if (isCbRun && cb != nullptr) {
        cb->OnResult(sinkId, status);
    } else {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_PREPARE_RESULT_MMI relayPreCallbacks_ is null.");
    }
}

void DistributedInputSourceManager::RunRelayUnprepareCallback(const std::string &srcId, const std::string &sinkId,
    const int32_t status)
{
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_UNPREPARE_START, DINPUT_UNPREPARE_TASK);
    std::lock_guard<std::mutex> lock(prepareMutex_);
    bool isCbRun = false;
    sptr<IUnprepareDInputCallback> cb = nullptr;
    for (auto iter = relayUnpreCallbacks_.begin(); iter != relayUnpreCallbacks_.end(); ++iter) {
        if (iter->srcId == srcId && iter->sinkId == sinkId) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_MMI");
            cb = iter->unpreCallback;
            relayUnpreCallbacks_.erase(iter);
            isCbRun = true;
            break;
        }
    }
    if (isCbRun && cb != nullptr) {
        cb->OnResult(sinkId, status);
    } else {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_UNPREPARE_RESULT_MMI relayUnpreCallbacks_ is null.");
    }
}

void DistributedInputSourceManager::RunUnprepareCallback(const std::string &devId, const int32_t &status)
{
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_UNPREPARE_START, DINPUT_UNPREPARE_TASK);
    std::lock_guard<std::mutex> prepareLock(prepareMutex_);
    for (auto iter = unpreCallbacks_.begin(); iter != unpreCallbacks_.end(); ++iter) {
        if (iter->devId == devId) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_UNPREPARE_MSG");
            iter->unpreCallback->OnResult(devId, status);
            unpreCallbacks_.erase(iter);
            std::lock_guard<std::mutex> whiteListLock(whiteListMutex_);
            if (delWhiteListCallbacks_.size() == 0) {
                DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_UNPREPARE_MSG delWhiteListCallback is null.");
                return;
            }
            for (const auto &it : delWhiteListCallbacks_) {
                it->OnResult(devId);
            }
            return;
        }
    }
    DHLOGE("ProcessEvent unparepareCallback is null.");
}

void DistributedInputSourceManager::RunStartCallback(
    const std::string &devId, const uint32_t &inputTypes, const int32_t &status)
{
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
    for (auto iter = staCallbacks_.begin(); iter != staCallbacks_.end(); ++iter) {
        if (iter->devId == devId && iter->inputTypes == inputTypes) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_START_MSG");
            iter->callback->OnResult(devId, inputTypes, status);
            staCallbacks_.erase(iter);
            break;
        }
    }
}

void DistributedInputSourceManager::RunStopCallback(
    const std::string &devId, const uint32_t &inputTypes, const int32_t &status)
{
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
    for (auto iter = stpCallbacks_.begin(); iter != stpCallbacks_.end(); ++iter) {
        if (iter->devId == devId && iter->inputTypes == inputTypes) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_STOP_MSG");
            iter->callback->OnResult(devId, inputTypes, status);
            stpCallbacks_.erase(iter);
            break;
        }
    }
}

void DistributedInputSourceManager::RunStartDhidCallback(const std::string &sinkId, const std::string &dhIds,
    const int32_t &status)
{
    std::vector<std::string> dhidsVec;
    SplitStringToVector(dhIds, INPUT_STRING_SPLIT_POINT, dhidsVec);
    DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_START_DHID_MSG dhIds:%{public}s, vec-size:%{public}zu",
        GetAnonyString(dhIds).c_str(), dhidsVec.size());
    std::string localNetWorkId = GetLocalNetworkId();
    if (localNetWorkId.empty()) {
        return;
    }

    for (auto iter = staStringCallbacks_.begin(); iter != staStringCallbacks_.end(); ++iter) {
        if (iter->sinkId != sinkId || !IsStringDataSame(iter->dhIds, dhidsVec)) {
            continue;
        }
        iter->callback->OnResultDhids(sinkId, status);
        staStringCallbacks_.erase(iter);
        break;
    }
}

void DistributedInputSourceManager::RunStopDhidCallback(const std::string &sinkId, const std::string &dhIds,
    const int32_t &status)
{
    std::vector<std::string> dhidsVec;
    SplitStringToVector(dhIds, INPUT_STRING_SPLIT_POINT, dhidsVec);
    std::string localNetworkId = GetLocalNetworkId();
    if (localNetworkId.empty()) {
        return;
    }

    for (auto iter = stpStringCallbacks_.begin();
        iter != stpStringCallbacks_.end(); ++iter) {
        if (iter->sinkId != sinkId || !IsStringDataSame(iter->dhIds, dhidsVec)) {
            continue;
        }
        iter->callback->OnResultDhids(sinkId, status);
        stpStringCallbacks_.erase(iter);
        break;
    }
}

void DistributedInputSourceManager::RunRelayStartDhidCallback(const std::string &srcId, const std::string &sinkId,
    const int32_t status, const std::string &dhids)
{
    std::vector<std::string> dhidsVec;
    SplitStringToVector(dhids, INPUT_STRING_SPLIT_POINT, dhidsVec);
    DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STARTDHID_RESULT_MMI dhIds:%{public}s, vec-size:%{public}zu",
        dhids.c_str(), dhidsVec.size());
    bool isCbRun = false;
    sptr<IStartStopDInputsCallback> cb = nullptr;
    for (auto iter = relayStaDhidCallbacks_.begin(); iter != relayStaDhidCallbacks_.end(); ++iter) {
        if (iter->srcId != srcId || iter->sinkId != sinkId || !IsStringDataSame(iter->dhIds, dhidsVec)) {
            continue;
        }
        DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STARTDHID_RESULT_MMI call OnResultDhids");
        cb = iter->callback;
        relayStaDhidCallbacks_.erase(iter);
        isCbRun = true;
        break;
    }
    if (isCbRun && cb != nullptr) {
        cb->OnResultDhids(sinkId, status);
    } else {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STARTDHID_RESULT_MMI relayStaDhidCallbacks_ is null.");
    }
}

void DistributedInputSourceManager::RunRelayStopDhidCallback(const std::string &srcId, const std::string &sinkId,
    const int32_t status, const std::string &dhids)
{
    std::vector<std::string> dhidsVec;
    SplitStringToVector(dhids, INPUT_STRING_SPLIT_POINT, dhidsVec);
    bool isCbRun = false;
    sptr<IStartStopDInputsCallback> cb = nullptr;
    for (auto iter = relayStpDhidCallbacks_.begin(); iter != relayStpDhidCallbacks_.end(); ++iter) {
        if (iter->srcId != srcId || iter->sinkId != sinkId || !IsStringDataSame(iter->dhIds, dhidsVec)) {
            continue;
        }
        DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STOPDHID_RESULT_MMI call OnResultDhids");
        cb = iter->callback;
        relayStpDhidCallbacks_.erase(iter);
        isCbRun = true;
        break;
    }
    if (isCbRun && cb != nullptr) {
        cb->OnResultDhids(sinkId, status);
    } else {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STOPDHID_RESULT_MMI relayStpDhidCallbacks_ is null.");
    }
}

void DistributedInputSourceManager::RunRelayStartTypeCallback(const std::string &srcId, const std::string &sinkId,
    const int32_t status, uint32_t inputTypes)
{
    bool isCbRun = false;
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_START_START, DINPUT_START_TASK);
    for (std::vector<DInputClientStartTypeInfo>::iterator iter =
        relayStaTypeCallbacks_.begin(); iter != relayStaTypeCallbacks_.end(); ++iter) {
        if (iter->srcId == srcId && iter->sinkId == sinkId && iter->inputTypes == inputTypes) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STARTTYPE_RESULT_MMI");
            iter->callback->OnResult(sinkId, inputTypes, status);
            relayStaTypeCallbacks_.erase(iter);
            isCbRun = true;
            break;
        }
    }

    if (!isCbRun) {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STARTTYPE_RESULT_MMI relayStaTypeCallbacks_ is null.");
    }
}

void DistributedInputSourceManager::RunRelayStopTypeCallback(const std::string &srcId, const std::string &sinkId,
    const int32_t status, uint32_t inputTypes)
{
    bool isCbRun = false;
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_STOP_START, DINPUT_STOP_TASK);
    for (std::vector<DInputClientStopTypeInfo>::iterator iter =
        relayStpTypeCallbacks_.begin(); iter != relayStpTypeCallbacks_.end(); ++iter) {
        if (iter->srcId == srcId && iter->sinkId == sinkId && iter->inputTypes == inputTypes) {
            DHLOGI("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STOPTYPE_RESULT_MMI");
            iter->callback->OnResult(sinkId, inputTypes, status);
            relayStpTypeCallbacks_.erase(iter);
            isCbRun = true;
            break;
        }
    }

    if (!isCbRun) {
        DHLOGE("ProcessEvent DINPUT_SOURCE_MANAGER_RELAY_STOPTYPE_RESULT_MMI relayStpTypeCallbacks_ is null.");
    }
}
void DistributedInputSourceManager::RunKeyStateCallback(const std::string &sinkId, const std::string &dhId,
    const uint32_t type, const uint32_t code, const uint32_t value)
{
    // 1.notify multiinput
    std::lock_guard<std::mutex> lock(simEventMutex_);
    for (const auto &cb : simulationEventCallbacks_) {
        cb->OnSimulationEvent(type, code, value);
    }

    DHLOGI("ProcessEvent notify multimodal OnSimulationEvent success.");
    // 2.if return success, write to virtulnode
    RawEvent mEventBuffer;
    mEventBuffer.type = type;
    mEventBuffer.code = code;
    mEventBuffer.value = value;
    mEventBuffer.descriptor = dhId;
    std::vector<RawEvent> eventBuffers = {mEventBuffer};
    DistributedInputInject::GetInstance().RegisterDistributedEvent(sinkId, eventBuffers);
    return;
}

DInputServerType DistributedInputSourceManager::GetStartTransFlag()
{
    return isStartTrans_;
}

void DistributedInputSourceManager::SetStartTransFlag(const DInputServerType flag)
{
    DHLOGI("Set Source isStartTrans_ %{public}d", static_cast<int32_t>(flag));
    isStartTrans_ = flag;
}

std::vector<DistributedInputSourceManager::InputDeviceId> DistributedInputSourceManager::GetInputDeviceId()
{
    return inputDevice_;
}

void DistributedInputSourceManager::RemoveInputDeviceId(const std::string deviceId, const std::string dhId)
{
    InputDeviceId inputDeviceId {deviceId, dhId};

    auto it  = std::find(inputDevice_.begin(), inputDevice_.end(), inputDeviceId);
    if (it == inputDevice_.end()) {
        return;
    }

    // delete device
    DHLOGI("inputDevice erase deviceId: %{public}s, dhId: %{public}s", GetAnonyString(it->devId).c_str(),
        GetAnonyString(it->dhId).c_str());
    inputDevice_.erase(it);
}

bool DistributedInputSourceManager::GetDeviceMapAllDevSwitchOff()
{
    bool isAllDevSwitchOff = true;
    for (auto it = DeviceMap_.begin(); it != DeviceMap_.end(); ++it) {
        if (it->second == DINPUT_SOURCE_SWITCH_ON) {
            isAllDevSwitchOff = false;
            break;
        }
    }
    return isAllDevSwitchOff;
}

void DistributedInputSourceManager::SetDeviceMapValue(const std::string deviceId, int32_t value)
{
    DeviceMap_[deviceId] = value;
}

uint32_t DistributedInputSourceManager::GetInputTypesMap(const std::string deviceId)
{
    auto key = InputTypesMap_.find(deviceId);
    if (key != InputTypesMap_.end()) {
        return InputTypesMap_[deviceId];
    }
    return static_cast<uint32_t>(DInputDeviceType::NONE);
}

uint32_t DistributedInputSourceManager::GetAllInputTypesMap()
{
    uint32_t rInputTypes = static_cast<uint32_t>(DInputDeviceType::NONE);
    for (auto iter = InputTypesMap_.begin(); iter != InputTypesMap_.end(); ++iter) {
        rInputTypes |= iter->second;
    }
    return rInputTypes;
}

void DistributedInputSourceManager::ClearResourcesStatus()
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    preCallbacks_.clear();
    unpreCallbacks_.clear();
    relayPreCallbacks_.clear();
    relayUnpreCallbacks_.clear();
}

void DistributedInputSourceManager::SetInputTypesMap(const std::string deviceId, uint32_t value)
{
    if (value == static_cast<uint32_t>(DInputDeviceType::NONE)) {
        auto key = InputTypesMap_.find(deviceId);
        if (key != InputTypesMap_.end()) {
            InputTypesMap_.erase(key);
            return;
        }
    }
    InputTypesMap_[deviceId] = value;
}

void DistributedInputSourceManager::StartDScreenListener::OnMessage(const DHTopic topic, const std::string &message)
{
    DHLOGI("StartDScreenListener OnMessage!");
    if (topic != DHTopic::TOPIC_START_DSCREEN) {
        DHLOGE("this topic is wrong, %{public}d", static_cast<uint32_t>(topic));
        return;
    }
    if (message.size() > SCREEN_MSG_MAX) {
        DHLOGE("StartDScreenListener message size too long.");
        return;
    }
    std::string sinkDevId = "";
    SrcScreenInfo srcScreenInfo = {};
    int32_t parseRes = ParseMessage(message, sinkDevId, srcScreenInfo);
    if (parseRes != DH_SUCCESS) {
        DHLOGE("Parse message failed!");
        return;
    }

    std::string srcDevId = GetLocalNetworkId();
    std::string virtualTouchScreenDHId = DistributedInputInject::GetInstance().GenerateVirtualTouchScreenDHId(
        srcScreenInfo.sourceWinId, srcScreenInfo.sourceWinWidth, srcScreenInfo.sourceWinHeight);
    int32_t createNodeRes = DistributedInputInject::GetInstance().CreateVirtualTouchScreenNode(srcDevId,
        virtualTouchScreenDHId, srcScreenInfo.sourceWinId, srcScreenInfo.sourceWinWidth,
        srcScreenInfo.sourceWinHeight);
    if (createNodeRes != DH_SUCCESS) {
        DHLOGE("Create virtual touch screen Node failed!");
        return;
    }

    int32_t cacheRes = UpdateSrcScreenInfoCache(srcScreenInfo);
    if (cacheRes != DH_SUCCESS) {
        DHLOGE("Update SrcScreenInfo cache failed!");
        return;
    }

    int32_t rpcRes = DistributedInputClient::GetInstance().NotifyStartDScreen(sinkDevId, srcDevId,
        srcScreenInfo.sourceWinId);
    if (rpcRes != DH_SUCCESS) {
        DHLOGE("Rpc invoke failed!");
        return;
    }

    sptr<IRemoteObject> dScreenSrcSA =
        DInputContext::GetInstance().GetRemoteObject(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID);
    sptr<DScreenSourceSvrRecipient> dScreenSrcDeathRecipient(new(std::nothrow) DScreenSourceSvrRecipient(srcDevId,
        sinkDevId, srcScreenInfo.sourceWinId));
    dScreenSrcSA->AddDeathRecipient(dScreenSrcDeathRecipient);
    DInputContext::GetInstance().AddRemoteObject(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, dScreenSrcSA);
}

int32_t DistributedInputSourceManager::StartDScreenListener::ParseMessage(const std::string &message,
    std::string &sinkDevId, SrcScreenInfo &srcScreenInfo)
{
    nlohmann::json jsonObj = nlohmann::json::parse(message, nullptr, false);
    if (jsonObj.is_discarded()) {
        DHLOGE("jsonObj parse failed!");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    if (!IsString(jsonObj, SINK_DEVICE_ID)) {
        DHLOGE("devId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkDevId = jsonObj[SINK_DEVICE_ID].get<std::string>();
    if (!IsUInt64(jsonObj, SOURCE_WINDOW_ID)) {
        DHLOGE("sourceWinId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    srcScreenInfo.sourceWinId = jsonObj[SOURCE_WINDOW_ID].get<uint64_t>();
    if (!IsUInt32(jsonObj, SOURCE_WINDOW_WIDTH)) {
        DHLOGE("sourceWinWidth key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    srcScreenInfo.sourceWinWidth = jsonObj[SOURCE_WINDOW_WIDTH].get<std::uint32_t>();
    if (!IsUInt32(jsonObj, SOURCE_WINDOW_HEIGHT)) {
        DHLOGE("sourceWinHeight key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    srcScreenInfo.sourceWinHeight = jsonObj[SOURCE_WINDOW_HEIGHT].get<std::uint32_t>();
    return DH_SUCCESS;
}

int32_t DistributedInputSourceManager::StartDScreenListener::UpdateSrcScreenInfoCache(const SrcScreenInfo &tmpInfo)
{
    std::string srcDevId = GetLocalNetworkId();
    std::string srcScreenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(srcDevId, tmpInfo.sourceWinId);
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(srcScreenInfoKey);
    srcScreenInfo.devId = srcDevId;
    srcScreenInfo.sessionId = DistributedInputSourceTransport::GetInstance().GetCurrentSessionId();
    srcScreenInfo.uuid = GetUUIDBySoftBus(srcDevId);
    srcScreenInfo.sourceWinId = tmpInfo.sourceWinId;
    srcScreenInfo.sourceWinWidth = tmpInfo.sourceWinWidth;
    srcScreenInfo.sourceWinHeight = tmpInfo.sourceWinHeight;
    srcScreenInfo.sourcePhyId = DistributedInputInject::GetInstance().GenerateVirtualTouchScreenDHId(
        srcScreenInfo.sourceWinId, srcScreenInfo.sourceWinWidth, srcScreenInfo.sourceWinHeight);
    int32_t virtualScreenFd = DistributedInputInject::GetInstance().GetVirtualTouchScreenFd();
    if (virtualScreenFd < 0) {
        DHLOGE("virtualScreenFd is invalid");
        return ERR_DH_INPUT_SERVER_SOURCE_VIRTUAL_SCREEN_NODE_IS_INVALID;
    }
    srcScreenInfo.sourcePhyFd = static_cast<uint32_t>(virtualScreenFd);
    srcScreenInfo.sourcePhyWidth = tmpInfo.sourceWinWidth;
    srcScreenInfo.sourcePhyHeight = tmpInfo.sourceWinHeight;
    DHLOGI("StartDScreenListener UpdateSrcScreenInfo the data: devId: %{public}s, sourceWinId: %{public}" PRIu64 ", "
        "sourceWinWidth: %{public}d, sourceWinHeight: %{public}d, sourcePhyId: %{public}s, sourcePhyFd: %{public}d, "
        "sourcePhyWidth: %{public}d, sourcePhyHeight: %{public}d",
        GetAnonyString(srcScreenInfo.devId).c_str(), srcScreenInfo.sourceWinId, srcScreenInfo.sourceWinWidth,
        srcScreenInfo.sourceWinHeight, GetAnonyString(srcScreenInfo.sourcePhyId).c_str(), srcScreenInfo.sourcePhyFd,
        srcScreenInfo.sourcePhyWidth, srcScreenInfo.sourcePhyHeight);
    return DInputContext::GetInstance().UpdateSrcScreenInfo(srcScreenInfoKey, srcScreenInfo);
}

void DistributedInputSourceManager::StopDScreenListener::OnMessage(const DHTopic topic, const std::string &message)
{
    DHLOGI("StopDScreenListener OnMessage!");
    if (topic != DHTopic::TOPIC_STOP_DSCREEN) {
        DHLOGE("this topic is wrong, %{public}u", static_cast<uint32_t>(topic));
        return;
    }
    std::string sinkDevId = "";
    uint64_t sourceWinId = 0;
    int32_t parseRes = ParseMessage(message, sinkDevId, sourceWinId);
    if (parseRes != DH_SUCCESS) {
        DHLOGE("Message parse failed!");
        return;
    }

    std::string sourceDevId = GetLocalNetworkId();
    std::string screenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(sourceDevId, sourceWinId);
    DHLOGI("screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(screenInfoKey);

    int32_t removeNodeRes = DistributedInputInject::GetInstance().RemoveVirtualTouchScreenNode(
        sourceDevId, srcScreenInfo.sourcePhyId);
    if (removeNodeRes != DH_SUCCESS) {
        DHLOGE("Remove virtual touch screen node failed!");
        return;
    }

    int32_t removeCacheRes = DInputContext::GetInstance().RemoveSrcScreenInfo(screenInfoKey);
    if (removeCacheRes != DH_SUCCESS) {
        DHLOGE("Remove src cache failed!");
        return;
    }

    int32_t rpcRes = DistributedInputClient::GetInstance().NotifyStopDScreen(sinkDevId, screenInfoKey);
    if (rpcRes != DH_SUCCESS) {
        DHLOGE("Rpc invoke failed!");
        return;
    }
}

int32_t DistributedInputSourceManager::StopDScreenListener::ParseMessage(const std::string &message,
    std::string &sinkDevId, uint64_t &sourceWinId)
{
    nlohmann::json jsonObj = nlohmann::json::parse(message, nullptr, false);
    if (jsonObj.is_discarded()) {
        DHLOGE("jsonObj parse failed!");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    if (!IsString(jsonObj, SINK_DEVICE_ID)) {
        DHLOGE("sourceWinId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sinkDevId = jsonObj[SINK_DEVICE_ID].get<std::string>();
    if (!IsUInt64(jsonObj, SOURCE_WINDOW_ID)) {
        DHLOGE("sourceWinId key is invalid");
        return ERR_DH_INPUT_JSON_PARSE_FAIL;
    }
    sourceWinId = jsonObj[SOURCE_WINDOW_ID].get<uint64_t>();
    return DH_SUCCESS;
}

DistributedInputSourceManager::DeviceOfflineListener::DeviceOfflineListener(
    DistributedInputSourceManager* srcManagerContext)
{
    DHLOGI("DeviceOfflineListener ctor!");
    sourceManagerContext_ = srcManagerContext;
}

void DistributedInputSourceManager::DeviceOfflineListener::OnMessage(const DHTopic topic, const std::string &message)
{
    DHLOGI("DeviceOfflineListener OnMessage!");
    if (topic != DHTopic::TOPIC_DEV_OFFLINE) {
        DHLOGE("this topic is wrong, %{public}u", static_cast<uint32_t>(topic));
        return;
    }
    if (message.empty()) {
        DHLOGE("this message is empty");
        return;
    }
}

DistributedInputSourceManager::DScreenSourceSvrRecipient::DScreenSourceSvrRecipient(const std::string &srcDevId,
    const std::string &sinkDevId, const uint64_t srcWinId)
{
    DHLOGI("DScreenStatusListener ctor!");
    this->srcDevId_ = srcDevId;
    this->sinkDevId_ = sinkDevId;
    this->srcWinId_ = srcWinId;
}

void DistributedInputSourceManager::DScreenSourceSvrRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("DScreenStatusListener OnRemoveSystemAbility");
    sptr<IRemoteObject> remoteObject = remote.promote();
    if (!remoteObject) {
        DHLOGE("OnRemoteDied remote promoted failed");
        return;
    }
    std::string screenInfoKey = DInputContext::GetInstance().GetScreenInfoKey(srcDevId_, srcWinId_);
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(screenInfoKey);
    std::string sourceDevId = GetLocalNetworkId();
    int32_t removeNodeRes = DistributedInputInject::GetInstance().RemoveVirtualTouchScreenNode(
        sourceDevId, srcScreenInfo.sourcePhyId);
    if (removeNodeRes != DH_SUCCESS) {
        DHLOGE("Remove virtual touch screen node failed!");
        return;
    }

    int32_t removeCacheRes = DInputContext::GetInstance().RemoveSrcScreenInfo(screenInfoKey);
    if (removeCacheRes != DH_SUCCESS) {
        DHLOGE("Remove src cache failed!");
        return;
    }

    int32_t rpcRes = DistributedInputClient::GetInstance().NotifyStopDScreen(sinkDevId_, screenInfoKey);
    if (rpcRes != DH_SUCCESS) {
        DHLOGE("Rpc invoke failed!");
        return;
    }
    DInputContext::GetInstance().RemoveRemoteObject(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID);
}

int32_t DistributedInputSourceManager::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    DHLOGI("DistributedInputSourceManager Dump.");
    std::vector<std::string> argsStr(args.size());
    std::transform(args.begin(), args.end(), argsStr.begin(), [](const auto &item) {return Str16ToStr8(item);});
    std::string result("");
    if (!HiDumper::GetInstance().HiDump(argsStr, result)) {
        DHLOGI("Hidump error.");
        return ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL;
    }

    int ret = dprintf(fd, "%{public}s\n", result.c_str());
    if (ret < 0) {
        DHLOGE("dprintf error.");
        return ERR_DH_INPUT_HIDUMP_DPRINTF_FAIL;
    }
    return DH_SUCCESS;
}

void DistributedInputSourceManager::AddPrepareCallbacks(const DInputClientPrepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    preCallbacks_.insert(info);
}

void DistributedInputSourceManager::RemovePrepareCallbacks(const DInputClientPrepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    preCallbacks_.erase(info);
}

void DistributedInputSourceManager::AddUnPrepareCallbacks(const DInputClientUnprepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    unpreCallbacks_.insert(info);
}

void DistributedInputSourceManager::RemoveUnPrepareCallbacks(const DInputClientUnprepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    unpreCallbacks_.erase(info);
}

void DistributedInputSourceManager::AddRelayPrepareCallbacks(const DInputClientRelayPrepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    relayPreCallbacks_.insert(info);
}

void DistributedInputSourceManager::RemoveRelayPrepareCallbacks(const DInputClientRelayPrepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    relayPreCallbacks_.erase(info);
}

void DistributedInputSourceManager::AddRelayUnPrepareCallbacks(const DInputClientRelayUnprepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    relayUnpreCallbacks_.insert(info);
}

void DistributedInputSourceManager::RemoveRelayUnPrepareCallbacks(const DInputClientRelayUnprepareInfo &info)
{
    std::lock_guard<std::mutex> lock(prepareMutex_);
    relayUnpreCallbacks_.erase(info);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
