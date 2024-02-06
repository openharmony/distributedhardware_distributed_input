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

#ifndef DISTRIBUTED_INPUT_SOURCE_TRANSPORT_H
#define DISTRIBUTED_INPUT_SOURCE_TRANSPORT_H

#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include <thread>

#include "constants.h"
#include "event_handler.h"
#include "nlohmann/json.hpp"
#include "securec.h"

#include "dinput_source_trans_callback.h"
#include "dinput_transbase_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceTransport {
public:
    static DistributedInputSourceTransport &GetInstance();
    ~DistributedInputSourceTransport();

    class DInputTransbaseSourceListener : public DInputTransbaseSourceCallback {
    public:
        DInputTransbaseSourceListener(DistributedInputSourceTransport *transport);
        virtual ~DInputTransbaseSourceListener();
        void HandleSessionData(int32_t sessionId, const std::string &messageData) override;
        void NotifySessionClosed() override;

    private:
        DistributedInputSourceTransport *sourceTransportObj_;
    };

    int32_t Init();
    void Release();

    int32_t OpenInputSoftbus(const std::string &remoteDevId, bool isToSrc);
    void CloseInputSoftbus(const std::string &remoteDevId, bool isToSrc);

    void RegisterSourceRespCallback(std::shared_ptr<DInputSourceTransCallback> callback);

    int32_t PrepareRemoteInput(const std::string &deviceId);
    int32_t UnprepareRemoteInput(const std::string &deviceId);
    int32_t StartRemoteInput(const std::string &deviceId, const uint32_t &inputTypes);
    int32_t StopRemoteInput(const std::string &deviceId, const uint32_t &inputTypes);
    int32_t LatencyCount(const std::string &deviceId);
    void StartLatencyCount(const std::string &deviceId);
    void StartLatencyThread(const std::string &deviceId);
    void StopLatencyThread();

    int32_t StartRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhids);
    int32_t StopRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhids);

    int32_t SendRelayPrepareRequest(const std::string &srcId, const std::string &sinkId);
    int32_t SendRelayUnprepareRequest(const std::string &srcId, const std::string &sinkId);
    int32_t PrepareRemoteInput(int32_t sessionId, const std::string &deviceId);
    int32_t UnprepareRemoteInput(int32_t sessionId, const std::string &deviceId);
    int32_t NotifyOriginPrepareResult(int32_t srcTsrcSeId, const std::string &srcId, const std::string &sinkId,
        int32_t status);
    int32_t NotifyOriginUnprepareResult(int32_t srcTsrcSeId, const std::string &srcId, const std::string &sinkId,
        int32_t status);

    int32_t SendRelayStartDhidRequest(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhids);
    int32_t SendRelayStopDhidRequest(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhids);
    int32_t SendRelayStartTypeRequest(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes);
    int32_t SendRelayStopTypeRequest(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes);
    int32_t GetCurrentSessionId();

private:
    int32_t SendMessage(int32_t sessionId, std::string &message);
    void HandleData(int32_t sessionId, const std::string &message);
    void SessionClosed();
    void NotifyResponsePrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStartRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStopRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStartRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseStopRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseKeyState(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseKeyStateBatch(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyReceivedEventRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveSrcTSrcRelayPrepare(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveSrcTSrcRelayUnprepare(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseRelayPrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseRelayUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveRelayPrepareResult(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveRelayUnprepareResult(int32_t sessionId, const nlohmann::json &recMsg);

    void ReceiveSrcTSrcRelayStartDhid(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveSrcTSrcRelayStopDhid(int32_t sessionId, const nlohmann::json &recMsg);
    int32_t StartRemoteInputDhids(int32_t sessionId, const std::string &deviceId, const std::string &dhids);
    int32_t StopRemoteInputDhids(int32_t sessionId, const std::string &deviceId, const std::string &dhids);
    void NotifyResponseRelayStartDhidRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseRelayStopDhidRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    int32_t NotifyOriginStartDhidResult(int32_t srcTsrcSeId, const std::string &srcId, const std::string &sinkId,
        int32_t status, const std::string &dhids);
    int32_t NotifyOriginStopDhidResult(int32_t srcTsrcSeId, const std::string &srcId, const std::string &sinkId,
        int32_t status, const std::string &dhids);
    void ReceiveRelayStartDhidResult(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveRelayStopDhidResult(int32_t sessionId, const nlohmann::json &recMsg);

    void ReceiveSrcTSrcRelayStartType(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveSrcTSrcRelayStopType(int32_t sessionId, const nlohmann::json &recMsg);
    int32_t StartRemoteInputType(int32_t sessionId, const std::string &deviceId, const uint32_t &inputTypes);
    int32_t StopRemoteInputType(int32_t sessionId, const std::string &deviceId, const uint32_t &inputTypes);
    void NotifyResponseRelayStartTypeRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyResponseRelayStopTypeRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    int32_t NotifyOriginStartTypeResult(int32_t sessionId, const std::string &srcId, const std::string &sinkId,
            int32_t status, uint32_t inputTypes);
    int32_t NotifyOriginStopTypeResult(int32_t sessionId, const std::string &srcId, const std::string &sinkId,
            int32_t status, uint32_t inputTypes);
    void ReceiveRelayStartTypeResult(int32_t sessionId, const nlohmann::json &recMsg);
    void ReceiveRelayStopTypeResult(int32_t sessionId, const nlohmann::json &recMsg);

    void CalculateLatency(int32_t sessionId, const nlohmann::json &recMsg);
    void RegRespFunMap();

    void ResetKeyboardKeyState(const std::string &deviceId, const std::vector<std::string> &dhids);
private:
    std::mutex operationMutex_;
    std::set<int32_t> sessionIdSet_;
    std::shared_ptr<DInputSourceTransCallback> callback_;
    std::shared_ptr<DistributedInputSourceTransport::DInputTransbaseSourceListener> statuslistener_;
    std::string mySessionName_ = "";
    std::condition_variable openSessionWaitCond_;
    uint64_t deltaTime_ = 0;
    uint64_t deltaTimeAll_ = 0;
    uint64_t sendTime_ = 0;
    uint32_t sendNum_ = 0;
    uint32_t recvNum_ = 0;
    std::atomic<bool> isLatencyThreadRunning_ = false;
    std::thread latencyThread_;
    std::string eachLatencyDetails_ = "";
    std::atomic<int32_t> injectThreadNum = 0;
    std::atomic<int32_t> latencyThreadNum = 0;

    using SourceTransportFunc = void (DistributedInputSourceTransport::*)(int32_t sessionId,
        const nlohmann::json &recMsg);
    std::map<int32_t, SourceTransportFunc> memberFuncMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_SOURCE_TRANSPORT_H
