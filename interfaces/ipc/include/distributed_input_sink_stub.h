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

#ifndef DISRIBUTED_INPUT_SINK_STUB_H
#define DISRIBUTED_INPUT_SINK_STUB_H

#include "i_distributed_sink_input.h"

#include <iostream>

#include "iremote_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSinkStub : public IRemoteStub<IDistributedSinkInput> {
public:
    DistributedInputSinkStub();
    ~DistributedInputSinkStub() override;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    int32_t InitInner(MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int32_t ReleaseInner(MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int32_t RegisterGetSinkScreenInfosInner(MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int32_t NotifyStartDScreenInner(MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int32_t NotifyStopDScreenInner(MessageParcel &data, MessageParcel &reply, MessageOption &option);

    int32_t RegisterSharingDhIdListenerInner(MessageParcel &data, MessageParcel &reply, MessageOption &option);

    bool HasEnableDHPermission();

private:
    DISALLOW_COPY_AND_MOVE(DistributedInputSinkStub);
    using DistributedInputSinkFunc = int32_t (DistributedInputSinkStub::*)(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    std::map<int32_t, DistributedInputSinkFunc> memberFuncMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_STUB_H