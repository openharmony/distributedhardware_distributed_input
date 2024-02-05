/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "distributed_input_source_manager.h"
#include "distributed_input_sink_manager.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DInputSAManager);

void DInputSAManager::SystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{}

void DInputSAManager::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{}

void DInputSAManager::Init()
{}

void DInputSAManager::RegisterEventHandler(std::shared_ptr<AppExecFwk::EventHandler> handler)
{
    eventHandler_ = handler;
}

bool DInputSAManager::GetDInputSourceProxy()
{
    int32_t saId = 4809;
    bool runOnCreate = false;

    if (dInputSourceProxy_ == nullptr) {
        dInputSourceProxy_ = new DistributedInputSourceManager(saId, runOnCreate);
    }
    return dInputSourceProxy_ != nullptr;
}

bool DInputSAManager::HasDInputSourceProxy()
{
    return dInputSourceProxy_ != nullptr;
}

bool DInputSAManager::SetDInputSourceProxy(const sptr<IRemoteObject> &remoteObject)
{
    dInputSourceProxy_ = iface_cast<IDistributedSourceInput>(remoteObject);

    if ((!dInputSourceProxy_) || (!dInputSourceProxy_->AsObject())) {
        DHLOGE("Failed to get dinput source proxy.");
        return false;
    }
    return true;
}

bool DInputSAManager::GetDInputSinkProxy()
{
    int32_t saId = 4810;
    bool runOnCreate = false;

    if (dInputSinkProxy_ == nullptr) {
        dInputSinkProxy_ = new DistributedInputSinkManager(saId, runOnCreate);
    }

    return dInputSinkProxy_ != nullptr;
}

bool DInputSAManager::HasDInputSinkProxy()
{
    return dInputSinkProxy_ != nullptr;
}

bool DInputSAManager::SetDInputSinkProxy(const sptr<IRemoteObject> &remoteObject)
{
    dInputSinkProxy_ = iface_cast<IDistributedSinkInput>(remoteObject);

    if ((!dInputSinkProxy_) || (!dInputSinkProxy_->AsObject())) {
        DHLOGE("Failed to get dinput sink proxy.");
        return false;
    }
    return true;
}

int32_t DInputSAManager::RestoreRegisterListenerAndCallback()
{
    return DH_SUCCESS;
}

void DInputSAManager::AddSimEventListenerToCache(sptr<ISimulationEventListener> listener)
{}

void DInputSAManager::RemoveSimEventListenerFromCache(sptr<ISimulationEventListener> listener)
{}

void DInputSAManager::AddSessionStateCbToCache(const sptr<ISessionStateCallback> callback)
{}

void DInputSAManager::RemoveSessionStateCbFromCache()
{}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS