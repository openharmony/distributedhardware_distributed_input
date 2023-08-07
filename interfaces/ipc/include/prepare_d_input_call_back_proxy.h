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

#ifndef PREPARE_D_INPUT_CALL_BACK_PROXY_H
#define PREPARE_D_INPUT_CALL_BACK_PROXY_H

#include "i_prepare_d_input_call_back.h"

#include <string>

#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class PrepareDInputCallbackProxy : public IRemoteProxy<IPrepareDInputCallback> {
public:
    explicit PrepareDInputCallbackProxy(const sptr<IRemoteObject> &object);
    ~PrepareDInputCallbackProxy() override;

    void OnResult(const std::string &deviceId, const int32_t &status) override;

private:
    static inline BrokerDelegator<PrepareDInputCallbackProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // PREPARE_D_INPUT_CALL_BACK_PROXY_H
