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

#ifndef INPUT_NODE_LISTENER_PROXY_H
#define INPUT_NODE_LISTENER_PROXY_H

#include "i_input_node_listener.h"

#include <string>

#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class InputNodeListenerProxy : public IRemoteProxy<InputNodeListener> {
public:
    explicit InputNodeListenerProxy(const sptr<IRemoteObject> &object);
    ~InputNodeListenerProxy() override;

    void OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId,
        const std::string &sinkNodeId, const std::string &sinkNodeDesc) override;

    void OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId,
        const std::string &sinkNodeId) override;

private:
    static inline BrokerDelegator<InputNodeListenerProxy> delegator_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // INPUT_NODE_LISTENER_PROXY_H
