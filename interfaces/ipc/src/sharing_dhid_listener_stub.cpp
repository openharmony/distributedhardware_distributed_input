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

#include "sharing_dhid_listener_stub.h"

#include "string_ex.h"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
SharingDhIdListenerStub::SharingDhIdListenerStub() {}

SharingDhIdListenerStub::~SharingDhIdListenerStub() {}

int32_t SharingDhIdListenerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        DHLOGE("SharingDhIdListenerStub read token valid failed");
        return ERR_DH_INPUT_IPC_READ_TOKEN_VALID_FAIL;
    }
    ISharingDhIdListener::Message msgCode = static_cast<ISharingDhIdListener::Message>(code);
    switch (msgCode) {
        case ISharingDhIdListener::Message::SHARING: {
            std::string dhId = data.ReadString();
            int32_t ret = OnSharing(dhId);
            if (!reply.WriteInt32(ret)) {
                DHLOGE("SharingDhIdListenerStub write ret failed");
                return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
            }
            break;
        }
        case ISharingDhIdListener::Message::NO_SHARING: {
            std::string dhId = data.ReadString();
            int32_t ret = OnNoSharing(dhId);
            if (!reply.WriteInt32(ret)) {
                DHLOGE("SharingDhIdListenerStub write ret failed");
                return ERR_DH_INPUT_IPC_WRITE_VALID_FAIL;
            }
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
