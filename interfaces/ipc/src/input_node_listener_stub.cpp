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

#include "input_node_listener_stub.h"

#include "string_ex.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
InputNodeListenerStub::InputNodeListenerStub() {}

InputNodeListenerStub::~InputNodeListenerStub() {}

int32_t InputNodeListenerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("InputNodeListenerStub read token valid failed");
        return ERR_DH_INPUT_IPC_READ_TOKEN_VALID_FAIL;
    }
    InputNodeListener::Message msgCode = static_cast<InputNodeListener::Message>(code);
    switch (msgCode) {
        case InputNodeListener::Message::RESULT_ON: {
            std::string srcDevId = data.ReadString();
            std::string sinkDevId = data.ReadString();
            std::string sinkNodeId = data.ReadString();
            std::string sinkNodeDesc = data.ReadString();
            OnNodeOnLine(srcDevId, sinkDevId, sinkNodeId, sinkNodeDesc);
            break;
        }
        case InputNodeListener::Message::RESULT_OFF: {
            std::string srcDevId = data.ReadString();
            std::string sinkDevId = data.ReadString();
            std::string sinkNodeId = data.ReadString();
            OnNodeOffLine(srcDevId, sinkDevId, sinkNodeId);
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
