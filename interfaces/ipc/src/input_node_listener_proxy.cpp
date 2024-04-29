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

#include "input_node_listener_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
InputNodeListenerProxy::InputNodeListenerProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<InputNodeListener>(object)
{
}

InputNodeListenerProxy::~InputNodeListenerProxy() {}

void InputNodeListenerProxy::OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId,
    const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("InputNodeListenerProxy write token valid failed");
        return;
    }
    if (!data.WriteString(srcDevId)) {
        DHLOGE("InputNodeListenerProxy write srcDevId failed");
        return;
    }
    if (!data.WriteString(sinkDevId)) {
        DHLOGE("InputNodeListenerProxy write sinkDevId failed");
        return;
    }
    if (!data.WriteString(sinkNodeId)) {
        DHLOGE("InputNodeListenerProxy write sinkNodeId failed");
        return;
    }
    if (!data.WriteString(sinkNodeDesc)) {
        DHLOGE("InputNodeListenerProxy write sinkNodeDesc failed");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(InputNodeListener::Message::RESULT_ON), data, reply,
        option);
    if (ret != 0) {
        return;
    }
}

void InputNodeListenerProxy::OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId,
    const std::string &sinkNodeId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("InputNodeListenerProxy write token valid failed");
        return;
    }
    if (!data.WriteString(srcDevId)) {
        DHLOGE("InputNodeListenerProxy write srcDevId failed");
        return;
    }
    if (!data.WriteString(sinkDevId)) {
        DHLOGE("InputNodeListenerProxy write sinkDevId failed");
        return;
    }
    if (!data.WriteString(sinkNodeId)) {
        DHLOGE("InputNodeListenerProxy write sinkNodeId failed");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(InputNodeListener::Message::RESULT_OFF), data, reply,
        option);
    if (ret != 0) {
        return;
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
