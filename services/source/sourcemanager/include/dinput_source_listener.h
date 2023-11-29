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

#ifndef DINPUT_SOURCE_LISTENER_H
#define DINPUT_SOURCE_LISTENER_H

#include <cstring>
#include <mutex>
#include <set>

#include <unistd.h>
#include <sys/types.h>

#include "event_handler.h"
#include "ipublisher_listener.h"
#include "publisher_listener_stub.h"
#include "singleton.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_source_manager_callback.h"
#include "dinput_source_trans_callback.h"
#include "distributed_input_source_manager.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceManager;
class DInputSourceListener : public DInputSourceTransCallback {
public:
    explicit DInputSourceListener(DistributedInputSourceManager *manager);
    virtual ~DInputSourceListener();
    void OnResponseRegisterDistributedHardware(const std::string deviceId, const std::string dhId,
        bool result) override;
    void OnResponsePrepareRemoteInput(const std::string deviceId, bool result, const std::string &object) override;
    void OnResponseUnprepareRemoteInput(const std::string deviceId, bool result) override;
    void OnResponseStartRemoteInput(const std::string deviceId, const uint32_t inputTypes, bool result) override;
    void OnResponseStopRemoteInput(const std::string deviceId, const uint32_t inputTypes, bool result) override;
    void OnResponseStartRemoteInputDhid(const std::string deviceId, const std::string &dhids, bool result) override;
    void OnResponseStopRemoteInputDhid(const std::string deviceId, const std::string &dhids, bool result) override;
    void OnResponseKeyState(const std::string deviceId, const std::string &dhid, const uint32_t type,
        const uint32_t code, const uint32_t value) override;
    void OnResponseKeyStateBatch(const std::string deviceId, const std::string &event) override;
    void OnReceivedEventRemoteInput(const std::string deviceId, const std::string &event) override;
    void OnResponseRelayPrepareRemoteInput(int32_t sessionId, const std::string &deviceId, bool result,
        const std::string &object) override;
    void OnResponseRelayUnprepareRemoteInput(int32_t sessionId, const std::string &deviceId, bool result) override;

    void OnReceiveRelayPrepareResult(int32_t status, const std::string &srcId, const std::string &sinkId) override;
    void OnReceiveRelayUnprepareResult(int32_t status, const std::string &srcId,
        const std::string &sinkId) override;
    void OnReceiveRelayStartDhidResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        const std::string &dhids) override;
    void OnReceiveRelayStopDhidResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        const std::string &dhids) override;
    void OnReceiveRelayStartTypeResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        uint32_t inputTypes) override;
    void OnReceiveRelayStopTypeResult(int32_t status, const std::string &srcId, const std::string &sinkId,
        uint32_t inputTypes) override;
    void RecordEventLog(int64_t when, int32_t type, int32_t code, int32_t value, const std::string &path);

private:
    DistributedInputSourceManager *sourceManagerObj_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DINPUT_SOURCE_LISTENER_H