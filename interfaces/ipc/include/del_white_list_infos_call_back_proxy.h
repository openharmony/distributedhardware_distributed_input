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

#ifndef DEL_WHITE_LIST_INFOS_CALL_BACK_PROXY_H
#define DEL_WHITE_LIST_INFOS_CALL_BACK_PROXY_H

#include "i_del_white_list_infos_call_back.h"

#include <string>

#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DelWhiteListInfosCallbackProxy : public IRemoteProxy<IDelWhiteListInfosCallback> {
public:
    explicit DelWhiteListInfosCallbackProxy(const sptr<IRemoteObject> &object);
    ~DelWhiteListInfosCallbackProxy() override;

    void OnResult(const std::string &deviceId) override;

private:
    static inline BrokerDelegator<DelWhiteListInfosCallbackProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // ADD_WHITE_LIST_INFOS_CALL_BACK_PROXY_H
