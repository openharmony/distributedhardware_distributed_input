/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "del_white_list_infos_call_back_stub.h"
#include "string_ex.h"
#include "constants_dinput.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DelWhiteListInfosCallbackStub::DelWhiteListInfosCallbackStub()
{
}

DelWhiteListInfosCallbackStub::~DelWhiteListInfosCallbackStub()
{
}

int32_t DelWhiteListInfosCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IDelWhiteListInfosCallback::GetDescriptor()) {
        return ERROR;
    }
    IDelWhiteListInfosCallback::Message msgCode = static_cast<IDelWhiteListInfosCallback::Message>(code);
    switch (msgCode) {
        case IDelWhiteListInfosCallback::Message::RESULT: {
            std::string deviceId = data.ReadString();
            OnResult(deviceId);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return NO_ERROR;
}
}  // namespace DistributedHardware
}  // namespace DistributedInput
}  // namespace OHOS