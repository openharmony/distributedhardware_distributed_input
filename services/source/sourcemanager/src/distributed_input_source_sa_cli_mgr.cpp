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

#include "distributed_input_source_sa_cli_mgr.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "dinput_log.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DInputSourceSACliMgr);
DInputSourceSACliMgr::DInputSourceSACliMgr()
{
    DHLOGI("Ctor DInputSourceSACliMgr");
    remoteCliDeathRcv = new RemoteCliDeathRecipient();

    auto dinputMgrRunner = AppExecFwk::EventRunner::Create("DInputMgrHandler");
    if (!dinputMgrRunner) {
        DHLOGE("DInputSourceSACliMgr, dinputMgrRunner is null!");
        return;
    }
    dinputMgrHandler_ = std::make_shared<AppExecFwk::EventHandler>(dinputMgrRunner);
    if (!dinputMgrHandler_) {
        DHLOGE("DInputSourceSACliMgr, dinputMgrHandler_ is null!");
    }
}

DInputSourceSACliMgr::~DInputSourceSACliMgr()
{
    DHLOGI("Dtor DInputSourceSACliMgr");
}

void DInputSourceSACliMgr::RemoteCliDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("RemoteCliDeathRecipient::OnRemoteDied received died notify!");
    sptr<IRemoteObject> diedRemote = remote.promote();
    if (diedRemote == nullptr) {
        DHLOGE("RemoteCliDeathRecipient::OnRemoteDied promote failedy!");
        return;
    }

    DInputSourceSACliMgr::GetInstance().ProcRemoteCliDied(diedRemote);
}

sptr<IDistributedSourceInput> DInputSourceSACliMgr::GetRemoteCli(const std::string &deviceId)
{
    if (deviceId.empty()) {
        DHLOGE("DInputSourceSACliMgr::GetRemoteCli deviceId is empty");
        return nullptr;
    }
    DHLOGI("DInputSourceSACliMgr::GetRemoteCli remote deviceid is %{public}s", GetAnonyString(deviceId).c_str());
    auto remoteCli = GetRemoteCliFromCache(deviceId);
    if (remoteCli != nullptr) {
        DHLOGD("VirtualHardwareManager::GetRemoteCli get from cache!");
        return remoteCli;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("GetRemoteCli failed to connect to systemAbilityMgr!");
        return nullptr;
    }

    auto object = samgr->CheckSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID, deviceId);
    if (object == nullptr) {
        DHLOGE("GetRemoteCli failed get remote Cli %{public}s", GetAnonyString(deviceId).c_str());
        return nullptr;
    }

    AddRemoteCli(deviceId, object);
    return iface_cast<IDistributedSourceInput>(object);
}

sptr<IDistributedSourceInput> DInputSourceSACliMgr::GetRemoteCliFromCache(const std::string &devId)
{
    std::lock_guard<std::mutex> lock(remoteCliLock);
    auto iter = remoteCliMap.find(devId);
    if (iter != remoteCliMap.end()) {
        return iter->second;
    }
    return nullptr;
}

void DInputSourceSACliMgr::AddRemoteCli(const std::string &devId, sptr<IRemoteObject> object)
{
    if (devId.empty() || (object == nullptr)) {
        DHLOGW("DInputSourceSACliMgr::AddRemoteCli param error! devId=%{public}s", GetAnonyString(devId).c_str());
        return;
    }

    DHLOGI("DInputSourceSACliMgr::AddRemoteCli devId=%{public}s", GetAnonyString(devId).c_str());
    object->AddDeathRecipient(remoteCliDeathRcv);

    std::lock_guard<std::mutex> lock(remoteCliLock);
    auto item = remoteCliMap.find(devId);
    if ((item != remoteCliMap.end()) && item->second != nullptr) {
        item->second->AsObject()->RemoveDeathRecipient(remoteCliDeathRcv);
    }
    remoteCliMap[devId] = iface_cast<IDistributedSourceInput>(object);
}

void DInputSourceSACliMgr::DeleteRemoteCli(const std::string &devId)
{
    DHLOGI("DInputSourceSACliMgr::DeleteRemoteCli devId=%{public}s", GetAnonyString(devId).c_str());
    std::lock_guard<std::mutex> lock(remoteCliLock);
    auto item = remoteCliMap.find(devId);
    if (item == remoteCliMap.end()) {
        DHLOGI("DInputSourceSACliMgr::DeleteRemoteCli not found device");
        return;
    }

    if (item->second != nullptr) {
        item->second->AsObject()->RemoveDeathRecipient(remoteCliDeathRcv);
    }
    remoteCliMap.erase(item);
}

void DInputSourceSACliMgr::DeleteRemoteCli(const sptr<IRemoteObject> remote)
{
    std::lock_guard<std::mutex> lock(remoteCliLock);
    auto iter = std::find_if(remoteCliMap.begin(), remoteCliMap.end(), [&](
        const std::pair<std::string, sptr<IDistributedSourceInput>> &item)->bool {
            return item.second->AsObject() == remote;
        });
    if (iter == remoteCliMap.end()) {
        DHLOGI("VirtualHardwareManager::DeleteRemoteCli not found remote object");
        return;
    }

    DHLOGI("VirtualHardwareManager::DeleteRemoteCli remote.devId=%{public}s", GetAnonyString(iter->first).c_str());
    if (iter->second != nullptr) {
        iter->second->AsObject()->RemoveDeathRecipient(remoteCliDeathRcv);
    }
    remoteCliMap.erase(iter);
}

void DInputSourceSACliMgr::ProcRemoteCliDied(const sptr<IRemoteObject> &remote)
{
    auto remoteCliDiedProc = [this, remote]() {
        DeleteRemoteCli(remote);
    };
    if (!(dinputMgrHandler_ && dinputMgrHandler_->PostTask(remoteCliDiedProc))) {
        DHLOGE("DInputSourceSACliMgr::OnRemoteDied PostTask fail");
    }
}
}
}
}