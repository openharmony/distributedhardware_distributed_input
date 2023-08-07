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

#include "prepare_d_input_call_back_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
PrepareDInputCallbackProxy::PrepareDInputCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IPrepareDInputCallback>(object)
{
}

PrepareDInputCallbackProxy::~PrepareDInputCallbackProxy()
{
}

void PrepareDInputCallbackProxy::OnResult(const std::string &deviceId, const int32_t &status)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("PrepareDInputCallbackProxy write token valid failed");
        return;
    }
    if (!data.WriteString(deviceId)) {
        return;
    }
    if (!data.WriteInt32(status)) {
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IPrepareDInputCallback::Message::RESULT), data, reply, option);
    if (ret != 0) {
        return;
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
