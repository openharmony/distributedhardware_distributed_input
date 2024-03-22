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

#include "start_stop_d_inputs_call_back_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
StartStopDInputsCallbackProxy::StartStopDInputsCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IStartStopDInputsCallback>(object)
{
}

StartStopDInputsCallbackProxy::~StartStopDInputsCallbackProxy() {}

void StartStopDInputsCallbackProxy::OnResultDhids(const std::string &devId, const int32_t &status)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("StartStopDInputsCallbackProxy write token valid failed");
        return;
    }
    if (!data.WriteString(devId)) {
        DHLOGE("StartStopDInputsCallbackProxy write devId valid failed");
        return;
    }
    if (!data.WriteInt32(status)) {
        DHLOGE("StartStopDInputsCallbackProxy write status valid failed");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IStartStopDInputsCallback::Message::RESULT_STRING),
                                      data, reply, option);
    if (ret != 0) {
        DHLOGE("OnResultDhids error, ret = %{public}d", ret);
        return;
    }
    DHLOGI("OnResultDhids success.");
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
