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

#include "start_stop_d_inputs_call_back_stub.h"

#include "string_ex.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
StartStopDInputsCallbackStub::StartStopDInputsCallbackStub() {}

StartStopDInputsCallbackStub::~StartStopDInputsCallbackStub() {}

int32_t StartStopDInputsCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IStartStopDInputsCallback::GetDescriptor()) {
        return ERR_DH_INPUT_IPC_INVALID_DESCRIPTOR;
    }
    IStartStopDInputsCallback::Message msgCode = static_cast<IStartStopDInputsCallback::Message>(code);
    switch (msgCode) {
        case IStartStopDInputsCallback::Message::RESULT_STRING: {
            std::string devId = data.ReadString();
            int32_t status = data.ReadInt32();
            DHLOGW("OnResultDhids received.");
            OnResultDhids(devId, status);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return DH_SUCCESS;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
