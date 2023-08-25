/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef REGISTER_SESSION_STATE_CALLBACK_STUB_H
#define REGISTER_SESSION_STATE_CALLBACK_STUB_H

#include "i_session_state_callback.h"

#include <string>

#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class RegisterSessionStateCallbackStub : public IRemoteStub<ISessionStateCallback> {
public:
    RegisterSessionStateCallbackStub();
    ~RegisterSessionStateCallbackStub() override;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    DISALLOW_COPY_AND_MOVE(RegisterSessionStateCallbackStub);
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // REGISTER_SESSION_STATE_CALLBACK_STUB_H
