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

#ifndef DISTRIBUTED_INPUT_SINK_HANDLER_H
#define DISTRIBUTED_INPUT_SINK_HANDLER_H

#include <condition_variable>
#include <mutex>
#include <string>

#include "idistributed_hardware_sink.h"
#include "iservice_registry.h"
#include "single_instance.h"
#include "system_ability_definition.h"
#include "system_ability_load_callback_stub.h"

#include "distributed_input_client.h"
#include "i_distributed_sink_input.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSinkHandler : public IDistributedHardwareSink {
DECLARE_SINGLE_INSTANCE_BASE(DistributedInputSinkHandler);
public:
    int32_t InitSink(const std::string &params) override;
    int32_t ReleaseSink() override;
    int32_t SubscribeLocalHardware(const std::string &dhId, const std::string &params) override;
    int32_t UnsubscribeLocalHardware(const std::string &dhId) override;
    void OnRemoteSinkSvrDied(const wptr<IRemoteObject> &remote);
    void FinishStartSA(const std::string &params, const sptr<IRemoteObject> &remoteObject);
    int32_t RegisterPrivacyResources(std::shared_ptr<PrivacyResourcesListener> listener) override;
    int32_t PauseDistributedHardware(const std::string &networkId) override;
    int32_t ResumeDistributedHardware(const std::string &networkId) override;
    int32_t StopDistributedHardware(const std::string &networkId) override;

public:
    class SALoadSinkCb : public OHOS::SystemAbilityLoadCallbackStub {
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
    class DInputSinkSvrRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };
    DistributedInputSinkHandler();
    ~DistributedInputSinkHandler();
    OHOS::sptr<OHOS::ISystemAbilityLoadCallback> sysSinkCallback = nullptr;

    sptr<IDistributedSinkInput> dInputSinkProxy_ = nullptr;
    sptr<DInputSinkSvrRecipient> sinkSvrRecipient_ = nullptr;
    std::mutex proxyMutex_;
    std::condition_variable proxyConVar_;
};

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) IDistributedHardwareSink *GetSinkHardwareHandler();
#ifdef __cplusplus
}
#endif
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_SINK_HANDLER_H
