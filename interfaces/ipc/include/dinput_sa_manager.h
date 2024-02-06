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

#ifndef DINPUT_SA_MANAGER_H
#define DINPUT_SA_MANAGER_H

#include <atomic>
#include <mutex>
#include <set>

#include "i_distributed_source_input.h"
#include "i_distributed_sink_input.h"
#include "single_instance.h"
#include "system_ability_status_change_stub.h"

#include "idistributed_hardware_source.h"
#include "idistributed_hardware_sink.h"
#include "event_handler.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
const uint32_t DINPUT_CLIENT_CHECK_SOURCE_CALLBACK_REGISTER_MSG = 1;
const uint32_t DINPUT_CLIENT_CHECK_SINK_CALLBACK_REGISTER_MSG = 2;
const uint32_t DINPUT_CLIENT_CLEAR_SOURCE_CALLBACK_REGISTER_MSG = 3;
const uint32_t DINPUT_CLIENT_CLEAR_SINK_CALLBACK_REGISTER_MSG = 4;
class DInputSAManager {
DECLARE_SINGLE_INSTANCE_BASE(DInputSAManager);
public:
    void Init();
    bool GetDInputSourceProxy();
    bool HasDInputSourceProxy();
    bool SetDInputSourceProxy(const sptr<IRemoteObject> &remoteObject);
    bool GetDInputSinkProxy();
    bool HasDInputSinkProxy();
    bool SetDInputSinkProxy(const sptr<IRemoteObject> &remoteObject);
    void RegisterEventHandler(std::shared_ptr<AppExecFwk::EventHandler> handler);
    int32_t RestoreRegisterListenerAndCallback();
    void AddSimEventListenerToCache(sptr<ISimulationEventListener> listener);
    void RemoveSimEventListenerFromCache(sptr<ISimulationEventListener> listener);
    void AddSessionStateCbToCache(sptr<ISessionStateCallback> callback);
    void RemoveSessionStateCbFromCache();

public:
    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    };

private:
    std::mutex simEventListenerCacheMtx_;
    std::set<sptr<ISimulationEventListener>> simEventListenerCache_;
    std::mutex sessionStateCbCacheMtx_;
    std::set<sptr<ISessionStateCallback>> sessionStateCbCache_;

private:
    DInputSAManager() = default;
    ~DInputSAManager() = default;

public:
    std::atomic<bool> dInputSourceSAOnline = false;
    std::atomic<bool> dInputSinkSAOnline = false;
    std::atomic<bool> isSubscribeSrcSAChangeListener = false;
    std::atomic<bool> isSubscribeSinkSAChangeListener = false;
    std::mutex sinkMutex_;
    std::mutex sourceMutex_;
    std::mutex handlerMutex_;

    sptr<IDistributedSourceInput> dInputSourceProxy_ = nullptr;
    sptr<IDistributedSinkInput> dInputSinkProxy_ = nullptr;
    sptr<SystemAbilityListener> saListenerCallback = nullptr;

    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DINPUT_SA_MANAGER_H