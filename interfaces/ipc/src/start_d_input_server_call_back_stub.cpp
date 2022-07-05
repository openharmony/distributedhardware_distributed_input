/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "start_d_input_server_call_back_stub.h"

#include "distributed_hardware_log.h"
#include "string_ex.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
StartDInputServerCallbackStub::StartDInputServerCallbackStub()
{
}

StartDInputServerCallbackStub::~StartDInputServerCallbackStub()
{
}

int32_t StartDInputServerCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("StartDInputServerCallbackStub read token valid failed");
        return ERR_DH_INPUT_IPC_READ_TOKEN_VALID_FAIL;
    }
    IStartDInputServerCallback::Message msgCode = static_cast<IStartDInputServerCallback::Message>(code);
    switch (msgCode) {
        case IStartDInputServerCallback::Message::RESULT: {
            int32_t status = data.ReadInt32();
            uint32_t inputTypes = data.ReadUint32();
            OnResult(status, inputTypes);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return DH_SUCCESS;
}
}  // namespace DistributedHardware
}  // namespace DistributedInput
}  // namespace OHOS
