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

#include "start_stop_result_call_back_stub.h"

#include "string_ex.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
StartStopResultCallbackStub::StartStopResultCallbackStub() {}

StartStopResultCallbackStub::~StartStopResultCallbackStub() {}

int32_t StartStopResultCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IStartStopResultCallback::GetDescriptor()) {
        return ERR_DH_INPUT_IPC_INVALID_DESCRIPTOR;
    }
    IStartStopResultCallback::Message msgCode = static_cast<IStartStopResultCallback::Message>(code);
    switch (msgCode) {
        case IStartStopResultCallback::Message::RESULT_START: {
            std::string srcId = data.ReadString();
            std::string sinkId = data.ReadString();
            uint32_t size = data.ReadUint32();
            if (size > IPC_VECTOR_MAX_SIZE) {
                DHLOGI("OnRemoteRequest start data size too large.");
                return ERR_DH_INPUT_IPC_READ_VALID_FAIL;
            }
            std::vector<std::string> dhIds;
            for (uint32_t i = 0; i < size; i++) {
                std::string dhId = data.ReadString();
                dhIds.push_back(dhId);
            }
            DHLOGW("OnStart received.");
            OnStart(srcId, sinkId, dhIds);
            break;
        }
        case IStartStopResultCallback::Message::RESULT_STOP: {
            std::string srcId = data.ReadString();
            std::string sinkId = data.ReadString();
            uint32_t size = data.ReadUint32();
            if (size > IPC_VECTOR_MAX_SIZE) {
                DHLOGI("OnRemoteRequest stop data size too large.");
                return ERR_DH_INPUT_IPC_READ_VALID_FAIL;
            }
            std::vector<std::string> dhIds;
            for (uint32_t i = 0; i < size; i++) {
                std::string dhId = data.ReadString();
                dhIds.push_back(dhId);
            }
            DHLOGW("OnStop received.");
            OnStop(srcId, sinkId, dhIds);
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
