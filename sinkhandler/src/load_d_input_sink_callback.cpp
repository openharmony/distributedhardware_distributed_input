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

#include "load_d_input_sink_callback.h"

#include "dinput_log.h"
#include "distributed_input_sink_handler.h"
#include "hisysevent_util.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
    LoadDInputSinkCallback::LoadDInputSinkCallback(const std::string &params) : params_(params) {}

void LoadDInputSinkCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject)
{
    DHLOGI("load dinput SA success, systemAbilityId:%{public}d, remoteObject result:%{public}s",
        systemAbilityId, (remoteObject != nullptr) ? "true" : "false");
    if (remoteObject == nullptr) {
        DHLOGE("remoteObject is nullptr");
        return;
    }
    DistributedInputSinkHandler::GetInstance().FinishStartSA(params_, remoteObject);
}

void LoadDInputSinkCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HisyseventUtil::GetInstance().SysEventWriteFault(DINPUT_INIT_FAIL,
        "dinput sink LoadSystemAbility call failed.");
    DHLOGE("load dinput SA failed, systemAbilityId:%{public}d", systemAbilityId);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
