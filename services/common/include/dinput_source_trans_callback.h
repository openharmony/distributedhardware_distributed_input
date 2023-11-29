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

#ifndef OHOS_DISTRIBUTED_INPUT_SOURCE_TRANS_H
#define OHOS_DISTRIBUTED_INPUT_SOURCE_TRANS_H

#include <string>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DInputSourceTransCallback {
public:
    virtual void OnResponseRegisterDistributedHardware(const std::string deviceId, const std::string dhId,
        bool result) = 0;
    virtual void OnResponsePrepareRemoteInput(const std::string deviceId, bool result, const std::string &object) = 0;
    virtual void OnResponseUnprepareRemoteInput(const std::string deviceId, bool result) = 0;
    virtual void OnResponseStartRemoteInput(const std::string deviceId, const uint32_t inputTypes, bool result) = 0;
    virtual void OnResponseStopRemoteInput(const std::string deviceId, const uint32_t inputTypes, bool result) = 0;
    virtual void OnResponseStartRemoteInputDhid(const std::string deviceId, const std::string &dhids, bool result) = 0;
    virtual void OnResponseStopRemoteInputDhid(const std::string deviceId, const std::string &dhids, bool result) = 0;
    virtual void OnResponseKeyState(const std::string deviceId, const std::string &dhid, const uint32_t type,
        const uint32_t code, const uint32_t value) = 0;
    virtual void OnResponseKeyStateBatch(const std::string deviceId, const std::string &object) = 0;
    virtual void OnReceivedEventRemoteInput(const std::string deviceId, const std::string &object) = 0;

    virtual void OnResponseRelayPrepareRemoteInput(int32_t sessionId, const std::string &deviceId, bool result,
        const std::string &object) = 0;
    virtual void OnResponseRelayUnprepareRemoteInput(int32_t sessionId, const std::string &deviceId, bool result) = 0;

    virtual void OnReceiveRelayPrepareResult(int32_t status, const std::string &srcId, const std::string &sinkId) = 0;
    virtual void OnReceiveRelayUnprepareResult(int32_t status, const std::string &srcId, const std::string &sinkId) = 0;
    virtual void OnReceiveRelayStartDhidResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        const std::string &dhids) = 0;
    virtual void OnReceiveRelayStopDhidResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        const std::string &dhids) = 0;
    virtual void OnReceiveRelayStartTypeResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        uint32_t inputTypes) = 0;
    virtual void OnReceiveRelayStopTypeResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        uint32_t inputTypes) = 0;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_INPUT_SOURCE_TRANS_H
