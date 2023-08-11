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

#ifndef DISTRIBUTED_INPUT_SINK_PROXY_H
#define DISTRIBUTED_INPUT_SINK_PROXY_H

#include "i_distributed_sink_input.h"

#include <iostream>

#include "iremote_proxy.h"

#include "i_sharing_dhid_listener.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSinkProxy : public IRemoteProxy<IDistributedSinkInput> {
public:

    explicit DistributedInputSinkProxy(const sptr<IRemoteObject> &object);
    ~DistributedInputSinkProxy() override;

    int32_t Init() override;

    int32_t Release() override;

    int32_t RegisterGetSinkScreenInfosCallback(sptr<IGetSinkScreenInfosCallback> callback) override;

    int32_t NotifyStartDScreen(const SrcScreenInfo &srcScreenRemoteCtrlInfo) override;
    int32_t NotifyStopDScreen(const std::string &srcScreenInfoKey) override;

    int32_t RegisterSharingDhIdListener(sptr<ISharingDhIdListener> sharingDhIdListener) override;

private:
    bool SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply);

    static inline BrokerDelegator<DistributedInputSinkProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_PROXY_H
