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

#include "stop_d_input_call_back_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
StopDInputCallbackProxy::StopDInputCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IStopDInputCallback>(object)
{
}

StopDInputCallbackProxy::~StopDInputCallbackProxy()
{
}

void StopDInputCallbackProxy::OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("StopDInputCallbackProxy write token valid failed");
        return;
    }
    if (!data.WriteString(devId)) {
        DHLOGE("StopDInputCallbackProxy write devId failed");
        return;
    }
    if (!data.WriteUint32(inputTypes)) {
        DHLOGE("StopDInputCallbackProxy write inputTypes failed");
        return;
    }
    if (!data.WriteInt32(status)) {
        DHLOGE("StopDInputCallbackProxy write status failed");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(IStopDInputCallback::Message::RESULT), data, reply, option);
    if (ret != 0) {
        return;
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
