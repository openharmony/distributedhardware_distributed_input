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

#ifndef DISTRIBUTED_INPUT_SOURCE_HANDLER_H
#define DISTRIBUTED_INPUT_SOURCE_HANDLER_H

#include <condition_variable>
#include <mutex>
#include <string>

#include "idistributed_hardware_source.h"
#include "iservice_registry.h"
#include "single_instance.h"
#include "system_ability_load_callback_stub.h"
#include "system_ability_definition.h"

#include "distributed_input_client.h"
#include "distributed_input_source_manager.h"
#include "i_distributed_source_input.h"
#include "load_d_input_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceHandler : public IDistributedHardwareSource {
DECLARE_SINGLE_INSTANCE_BASE(DistributedInputSourceHandler);
public:
    int32_t InitSource(const std::string &params) override;
    int32_t ReleaseSource() override;
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const EnableParam &param, std::shared_ptr<RegisterCallback> callback) override;
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        std::shared_ptr<UnregisterCallback> callback) override;
    int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId, const std::string &key,
        const std::string &value) override;
    void RegisterDistributedHardwareStateListener(std::shared_ptr<DistributedHardwareStateListener> listener) override;
    void UnregisterDistributedHardwareStateListener() override;
    void RegisterDataSyncTriggerListener(std::shared_ptr<DataSyncTriggerListener> listener) override;
    void UnregisterDataSyncTriggerListener() override;
    void FinishStartSA(const std::string &params, const sptr<IRemoteObject> &remoteObject);
    void OnRemoteSourceSvrDied(const wptr<IRemoteObject> &remote);

public:

    class SALoadSourceCb : public OHOS::SystemAbilityLoadCallbackStub {
    public:
        void OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                    const OHOS::sptr<IRemoteObject> &remoteObject) override;

        void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;

        int32_t GetSystemAbilityId() const
        {
            return currSystemAbilityId;
        }

        OHOS::sptr<IRemoteObject> GetRemoteObject() const
        {
            return currRemoteObject;
        }
    private:
        int32_t currSystemAbilityId = 0;
        OHOS::sptr<OHOS::IRemoteObject> currRemoteObject;
    };
private:
    DistributedInputSourceHandler();
    ~DistributedInputSourceHandler() = default;
    class DInputSourceSvrRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };
    OHOS::sptr<OHOS::ISystemAbilityLoadCallback> sysSourceCallback = nullptr;

    std::mutex proxyMutex_;
    std::condition_variable proxyConVar_;
    sptr<IDistributedSourceInput> dInputSourceProxy_ = nullptr;
    sptr<LoadDInputSourceCallback> dInputSourceCallback_ = nullptr;
    sptr<DInputSourceSvrRecipient> sourceSvrRecipient_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
__attribute__((visibility("default"))) IDistributedHardwareSource *GetSourceHardwareHandler();
#ifdef __cplusplus
}
#endif // __cplusplus
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_SOURCE_HANDLER_H
