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

#include "ipc_types.h"
#include "parcel.h"

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "start_stop_result_call_back_proxy.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
StartStopResultCallbackProxy::StartStopResultCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IStartStopResultCallback>(object)
{
}

StartStopResultCallbackProxy::~StartStopResultCallbackProxy() {}

void StartStopResultCallbackProxy::OnStart(
    const std::string &srcId, const std::string &sinkId, std::vector<std::string> &dhIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("StartStopResultCallbackProxy get IRemoteObject failed");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("StartStopResultCallbackProxy write token valid failed");
        return;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("StartStopResultCallbackProxy write srcId valid failed");
        return;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("StartStopResultCallbackProxy write sinkId valid failed");
        return;
    }
    if (!data.WriteUint32(dhIds.size())) {
        DHLOGE("StartStopResultCallbackProxy write devData size valid failed");
        return;
    }
    for (auto it = dhIds.begin(); it != dhIds.end(); ++it) {
        if (!data.WriteString(*it)) {
            DHLOGE("StartStopResultCallbackProxy write devData valid failed");
            return;
        }
    }

    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IStartStopResultCallback::Message::RESULT_START), data,
        reply, option);
    if (ret != 0) {
        DHLOGE("StartStopResultCallbackProxy SendRequest errno:%{public}d", ret);
        return;
    }
    DHLOGE("OnStart success.");
}

void StartStopResultCallbackProxy::OnStop(
    const std::string &srcId, const std::string &sinkId, std::vector<std::string> &dhIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("StartStopResultCallbackProxy get IRemoteObject failed");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("StartStopResultCallbackProxy write token valid failed");
        return;
    }
    if (!data.WriteString(srcId)) {
        DHLOGE("StartStopResultCallbackProxy write srcId valid failed");
        return;
    }
    if (!data.WriteString(sinkId)) {
        DHLOGE("StartStopResultCallbackProxy write sinkId valid failed");
        return;
    }

    if (!data.WriteUint32(dhIds.size())) {
        DHLOGE("StartStopResultCallbackProxy write devData size valid failed");
        return;
    }
    for (auto it = dhIds.begin(); it != dhIds.end(); ++it) {
        if (!data.WriteString(*it)) {
            DHLOGE("StartStopResultCallbackProxy write devData valid failed");
            return;
        }
    }

    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IStartStopResultCallback::Message::RESULT_STOP), data,
        reply, option);
    if (ret != 0) {
        DHLOGE("StartStopResultCallbackProxy SendRequest errno:%{public}d", ret);
        return;
    }
    DHLOGI("OnStop success.");
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
