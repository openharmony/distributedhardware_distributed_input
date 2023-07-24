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

#include "distributed_input_sink_handler.h"

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "hisysevent_util.h"
#include "i_distributed_sink_input.h"
#include "load_d_input_sink_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DistributedInputSinkHandler);

DistributedInputSinkHandler::~DistributedInputSinkHandler()
{
    DHLOGI("~DistributedInputSinkHandler");
}

int32_t DistributedInputSinkHandler::InitSink(const std::string &params)
{
    DHLOGD("InitSource");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (!DInputSAManager::GetInstance().HasDInputSinkProxy()) {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgr) {
            DHLOGE("Failed to get system ability mgr.");
            return ERR_DH_INPUT_SINK_HANDLER_INIT_SINK_SA_FAIL;
        }
        sptr<LoadDInputSinkCallback> loadCallback(new LoadDInputSinkCallback(params));
        HisyseventUtil::GetInstance().SysEventWriteBehavior(DINPUT_INIT,
            "dinput sink LoadSystemAbility call");
        int32_t ret = samgr->LoadSystemAbility(DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID, loadCallback);
        if (ret != ERR_OK) {
            DHLOGE("Failed to Load systemAbility, systemAbilityId:%d, ret code:%d",
                   DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID, ret);
            return ERR_DH_INPUT_SINK_HANDLER_INIT_SINK_SA_FAIL;
        }
    }

    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(INPUT_LOAD_SA_TIMEOUT_MS),
        [this]() { return (DInputSAManager::GetInstance().HasDInputSinkProxy()); });
    if (!waitStatus) {
        DHLOGE("dinput load sa timeout.");
        return ERR_DH_INPUT_SINK_HANDLER_INIT_SINK_SA_FAIL;
    }

    return DH_SUCCESS;
}

void DistributedInputSinkHandler::FinishStartSA(const std::string &params, const sptr<IRemoteObject> &remoteObject)
{
    DHLOGD("FinishStartSA");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    DInputSAManager::GetInstance().SetDInputSinkProxy(remoteObject);
    DistributedInputClient::GetInstance().InitSink();
    proxyConVar_.notify_all();
}

int32_t DistributedInputSinkHandler::ReleaseSink()
{
    return DistributedInputClient::GetInstance().ReleaseSink();
}

int32_t DistributedInputSinkHandler::SubscribeLocalHardware(const std::string &dhId, const std::string &params)
{
    return DH_SUCCESS;
}

int32_t DistributedInputSinkHandler::UnsubscribeLocalHardware(const std::string &dhId)
{
    return DH_SUCCESS;
}

void DistributedInputSinkHandler::SALoadSinkCb::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const OHOS::sptr<IRemoteObject>& remoteObject)
{
    currSystemAbilityId = systemAbilityId;
    currRemoteObject = remoteObject;
    DHLOGI("DistributedInputSinkHandler OnLoadSystemAbilitySuccess. systemAbilityId=%d", systemAbilityId);
}

void DistributedInputSinkHandler::SALoadSinkCb::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    currSystemAbilityId = systemAbilityId;
    DHLOGE("DistributedInputSinkHandler OnLoadSystemAbilityFail. systemAbilityId=%d", systemAbilityId);
}

IDistributedHardwareSink *GetSinkHardwareHandler()
{
    return &DistributedInputSinkHandler::GetInstance();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
