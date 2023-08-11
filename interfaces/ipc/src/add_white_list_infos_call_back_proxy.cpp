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

#include "add_white_list_infos_call_back_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
AddWhiteListInfosCallbackProxy::AddWhiteListInfosCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IAddWhiteListInfosCallback>(object)
{
}

AddWhiteListInfosCallbackProxy::~AddWhiteListInfosCallbackProxy()
{
}

void AddWhiteListInfosCallbackProxy::OnResult(const std::string &deviceId, const std::string &strJson)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("AddWhiteListInfosCallbackProxy write token valid failed");
        return;
    }
    if (!data.WriteString(deviceId)) {
        DHLOGE("AddWhiteListInfosCallbackProxy write deviceId failed");
        return;
    }
    if (!data.WriteString(strJson)) {
        DHLOGE("AddWhiteListInfosCallbackProxy write strJson failed");
        return;
    }
    remote->SendRequest(static_cast<uint32_t>(IAddWhiteListInfosCallback::Message::RESULT), data, reply, option);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
