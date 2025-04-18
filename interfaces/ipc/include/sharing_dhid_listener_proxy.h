/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef SHARING_DHID_LISTENER_PROXY_H
#define SHARING_DHID_LISTENER_PROXY_H

#include "i_sharing_dhid_listener.h"

#include <string>

#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class SharingDhIdListenerProxy : public IRemoteProxy<ISharingDhIdListener> {
public:
    explicit SharingDhIdListenerProxy(const sptr<IRemoteObject> &object);
    ~SharingDhIdListenerProxy() override;

    int32_t OnSharing(const std::string &dhId) override;
    int32_t OnNoSharing(const std::string &dhId) override;

private:
    static inline BrokerDelegator<SharingDhIdListenerProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // SHARING_DHID_LISTENER_PROXY_H