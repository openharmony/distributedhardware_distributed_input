/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "dinput_sa_manager.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DInputSAManager);
const uint32_t DINPUT_CLIENT_HANDLER_MSG_DELAY_TIME = 100; // million seconds
void DInputSAManager::SystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId == DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID) {
        DInputSAManager::GetInstance().dInputSourceSAOnline.store(false);
        {
            std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
            DInputSAManager::GetInstance().dInputSourceProxy_ = nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().handlerMutex_);
            if (DInputSAManager::GetInstance().eventHandler_ != nullptr) {
                DHLOGI("SendEvent DINPUT_CLIENT_CLEAR_SOURCE_CALLBACK_REGISTER_MSG");
                AppExecFwk::InnerEvent::Pointer msgEvent =
                    AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CLEAR_SOURCE_CALLBACK_REGISTER_MSG, systemAbilityId);
                DInputSAManager::GetInstance().eventHandler_->SendEvent(msgEvent, DINPUT_CLIENT_HANDLER_MSG_DELAY_TIME,
                    AppExecFwk::EventQueue::Priority::IMMEDIATE);
            }
        }
    }

    if (systemAbilityId == DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID) {
        DInputSAManager::GetInstance().dInputSinkSAOnline.store(false);
        {
            std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
            DInputSAManager::GetInstance().dInputSinkProxy_ = nullptr;
        }
        {
            std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().handlerMutex_);
            if (DInputSAManager::GetInstance().eventHandler_ != nullptr) {
                DHLOGI("SendEvent DINPUT_CLIENT_CLEAR_SINK_CALLBACK_REGISTER_MSG");
                AppExecFwk::InnerEvent::Pointer msgEvent =
                    AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CLEAR_SINK_CALLBACK_REGISTER_MSG, systemAbilityId);
                DInputSAManager::GetInstance().eventHandler_->SendEvent(msgEvent, DINPUT_CLIENT_HANDLER_MSG_DELAY_TIME,
                    AppExecFwk::EventQueue::Priority::IMMEDIATE);
            }
        }
    }
    DHLOGI("sa %{public}d is removed.", systemAbilityId);
}

void DInputSAManager::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    if (systemAbilityId == DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID) {
        DInputSAManager::GetInstance().dInputSourceSAOnline.store(true);
        std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().handlerMutex_);
        if (DInputSAManager::GetInstance().eventHandler_ != nullptr) {
            DHLOGI("SendEvent DINPUT_CLIENT_CHECK_SOURCE_CALLBACK_REGISTER_MSG");
            AppExecFwk::InnerEvent::Pointer msgEvent =
                AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CHECK_SOURCE_CALLBACK_REGISTER_MSG, systemAbilityId);
            DInputSAManager::GetInstance().eventHandler_->SendEvent(msgEvent,
                DINPUT_CLIENT_HANDLER_MSG_DELAY_TIME, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
    }

    if (systemAbilityId == DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID) {
        DInputSAManager::GetInstance().dInputSinkSAOnline.store(true);
        std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().handlerMutex_);
        if (DInputSAManager::GetInstance().eventHandler_ != nullptr) {
            DHLOGI("SendEvent DINPUT_CLIENT_CHECK_SINK_CALLBACK_REGISTER_MSG");
            AppExecFwk::InnerEvent::Pointer msgEvent =
                AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CHECK_SINK_CALLBACK_REGISTER_MSG, systemAbilityId);
            DInputSAManager::GetInstance().eventHandler_->SendEvent(msgEvent,
                DINPUT_CLIENT_HANDLER_MSG_DELAY_TIME, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
    }
    DHLOGI("sa %{public}d is added.", systemAbilityId);
}

void DInputSAManager::Init()
{
    saListenerCallback = new(std::nothrow) SystemAbilityListener();
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();

    if (!systemAbilityManager) {
        DHLOGE("get system ability manager failed.");
        return;
    }

    if (!isSubscribeSrcSAChangeListener.load()) {
        DHLOGI("try subscribe source sa change listener, saId:%{public}d", DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID);
        int32_t ret = systemAbilityManager->SubscribeSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID,
            saListenerCallback);
        if (ret != DH_SUCCESS) {
            DHLOGE("subscribe source sa change failed: %{public}d", ret);
            return;
        }
        isSubscribeSrcSAChangeListener.store(true);
    }

    if (!isSubscribeSinkSAChangeListener.load()) {
        DHLOGI("try subscribe sink sa change listener, saId:%{public}d", DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID);
        int32_t ret = systemAbilityManager->SubscribeSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID,
            saListenerCallback);
        if (ret != DH_SUCCESS) {
            DHLOGE("subscribe sink sa change failed: %{public}d", ret);
            return;
        }
        isSubscribeSinkSAChangeListener.store(true);
    }
}

void DInputSAManager::RegisterEventHandler(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    std::lock_guard<std::mutex> lock(handlerMutex_);
    eventHandler_ = handler;
}

bool DInputSAManager::GetDInputSourceProxy()
{
    if (!isSubscribeSrcSAChangeListener.load()) {
        std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
        if (!isSubscribeSrcSAChangeListener.load()) {
            sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (!systemAbilityManager) {
                DHLOGE("get system ability manager failed.");
                return false;
            }

            DHLOGI("try subscribe source sa change listener, saId:%{public}d", DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID);
            int32_t ret = systemAbilityManager->SubscribeSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID,
                saListenerCallback);
            if (ret != DH_SUCCESS) {
                DHLOGE("subscribe source sa change failed: %{public}d", ret);
                return false;
            }
            isSubscribeSrcSAChangeListener.store(true);
        }
    }

    if (dInputSourceSAOnline.load() && !dInputSourceProxy_) {
        std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
        if (dInputSourceProxy_ != nullptr) {
            DHLOGI("dinput source proxy has already got.");
            return true;
        }
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            DHLOGE("get system ability manager failed.");
            return false;
        }

        DHLOGI("try get sa: %{public}d", DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID);
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
            DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID);
        if (!remoteObject) {
            return false;
        }

        dInputSourceProxy_ = iface_cast<IDistributedSourceInput>(remoteObject);

        if ((!dInputSourceProxy_) || (!dInputSourceProxy_->AsObject())) {
            return false;
        }
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return dInputSourceProxy_ != nullptr;
}

bool DInputSAManager::HasDInputSourceProxy()
{
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    return dInputSourceProxy_ != nullptr;
}

bool DInputSAManager::SetDInputSourceProxy(const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sourceMutex_);
    dInputSourceProxy_ = iface_cast<IDistributedSourceInput>(remoteObject);

    if ((!dInputSourceProxy_) || (!dInputSourceProxy_->AsObject())) {
        DHLOGE("Failed to get dinput source proxy.");
        return false;
    }
    return true;
}

bool DInputSAManager::GetDInputSinkProxy()
{
    if (!isSubscribeSinkSAChangeListener.load()) {
        std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
        if (!isSubscribeSinkSAChangeListener.load()) {
            sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (!systemAbilityManager) {
                DHLOGE("get system ability manager failed.");
                return false;
            }

            DHLOGI("try subscribe sink sa change listener, sa id: %{public}d", DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID);
            int32_t ret = systemAbilityManager->SubscribeSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID,
                saListenerCallback);
            if (ret != DH_SUCCESS) {
                DHLOGE("subscribe sink sa change failed: %{public}d", ret);
                return false;
            }
            isSubscribeSinkSAChangeListener.store(true);
        }
    }

    if (dInputSinkSAOnline.load() && !dInputSinkProxy_) {
        std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
        if (dInputSinkProxy_ != nullptr) {
            DHLOGI("dinput sink proxy has already got.");
            return true;
        }
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            DHLOGE("get system ability manager failed.");
            return false;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
            DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID);
        if (!remoteObject) {
            return false;
        }

        dInputSinkProxy_ = iface_cast<IDistributedSinkInput>(remoteObject);

        if ((!dInputSinkProxy_) || (!dInputSinkProxy_->AsObject())) {
            return false;
        }
    }
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    return dInputSinkProxy_ != nullptr;
}

bool DInputSAManager::HasDInputSinkProxy()
{
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    return dInputSinkProxy_ != nullptr;
}

bool DInputSAManager::SetDInputSinkProxy(const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<std::mutex> lock(DInputSAManager::GetInstance().sinkMutex_);
    dInputSinkProxy_ = iface_cast<IDistributedSinkInput>(remoteObject);

    if ((!dInputSinkProxy_) || (!dInputSinkProxy_->AsObject())) {
        DHLOGE("Failed to get dinput sink proxy.");
        return false;
    }
    return true;
}

int32_t DInputSAManager::RestoreRegisterListenerAndCallback()
{
    DHLOGI("Restore RegisterPublisherListener");
    if (!DInputSAManager::GetInstance().GetDInputSourceProxy()) {
        DHLOGE("RestoreRegisterSimulationEventListener proxy error, client fail");
        return ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL;
    }

    int32_t result = DH_SUCCESS;
    {
        std::lock_guard<std::mutex> lock(simEventListenerCacheMtx_);
        for (const auto& listener : simEventListenerCache_) {
            if (listener == nullptr) {
                DHLOGE("simEventListenerCache_ is nullptr");
                continue;
            }
            int32_t ret = DInputSAManager::GetInstance().dInputSourceProxy_->RegisterSimulationEventListener(listener);
            if (ret != DH_SUCCESS) {
                result = ret;
                DHLOGE("SA execute RegisterSimulationEventListener fail, ret = %{public}d", ret);
            }
        }
    }
    {
        DHLOGI("Restore RegisterSessionStateCb");
        std::lock_guard<std::mutex> lock(sessionStateCbCacheMtx_);
        for (const auto& callback : sessionStateCbCache_) {
            if (callback == nullptr) {
                DHLOGE("sessionStateCbCache_ is nullptr");
                continue;
            }
            int32_t ret = DInputSAManager::GetInstance().dInputSourceProxy_->RegisterSessionStateCb(callback);
            if (ret != DH_SUCCESS) {
                result = ret;
                DHLOGE("SA execute RegisterSessionStateCb fail, ret = %{public}d", ret);
            }
        }
    }
    return result;
}

void DInputSAManager::AddSimEventListenerToCache(sptr<ISimulationEventListener> listener)
{
    std::lock_guard<std::mutex> simEventListenerLock(simEventListenerCacheMtx_);
    if (listener != nullptr) {
        simEventListenerCache_.insert(listener);
    }
}

void DInputSAManager::RemoveSimEventListenerFromCache(sptr<ISimulationEventListener> listener)
{
    std::lock_guard<std::mutex> simEventListenerLock(simEventListenerCacheMtx_);
    if (listener != nullptr) {
        simEventListenerCache_.erase(listener);
    }
}

void DInputSAManager::AddSessionStateCbToCache(sptr<ISessionStateCallback> callback)
{
    std::lock_guard<std::mutex> sessionStateCbLock(sessionStateCbCacheMtx_);
    if (callback != nullptr) {
        sessionStateCbCache_.insert(callback);
    }
}

void DInputSAManager::RemoveSessionStateCbFromCache()
{
    std::lock_guard<std::mutex> sessionStateCbLock(sessionStateCbCacheMtx_);
    sessionStateCbCache_.clear();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS