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

#ifndef DISTRIBUTED_INPUT_SINK_TRANSPORT_H
#define DISTRIBUTED_INPUT_SINK_TRANSPORT_H

#include <mutex>
#include <set>
#include <string>
#include <vector>

#include "constants.h"
#include "event_handler.h"
#include "nlohmann/json.hpp"

#include "dinput_sink_trans_callback.h"
#include "dinput_transbase_sink_callback.h"
#include "dinput_softbus_define.h"
#include "distributed_input_sink_switch.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSinkTransport {
public:
    static DistributedInputSinkTransport &GetInstance();
    DistributedInputSinkTransport();
    ~DistributedInputSinkTransport();

    class DInputTransbaseSinkListener : public DInputTransbaseSinkCallback {
    public:
        DInputTransbaseSinkListener(DistributedInputSinkTransport *transport);
        virtual ~DInputTransbaseSinkListener();
        void HandleSessionData(int32_t sessionId, const std::string &messageData) override;
        void NotifySessionClosed(int32_t sessionId) override;

    private:
        DistributedInputSinkTransport *sinkTransportObj_;
    };

    int32_t Init();

    void RegistSinkRespCallback(std::shared_ptr<DInputSinkTransCallback> callback);
    int32_t RespPrepareRemoteInput(const int32_t sessionId, std::string &smsg);
    int32_t RespUnprepareRemoteInput(const int32_t sessionId, std::string &smsg);
    int32_t RespStartRemoteInput(const int32_t sessionId, std::string &smsg);
    int32_t RespStopRemoteInput(const int32_t sessionId, std::string &smsg);
    int32_t RespLatency(const int32_t sessionId, std::string &smsg);
    void SendKeyStateNodeMsg(const int32_t sessionId, const std::string &dhId, uint32_t type, const uint32_t btnCode,
        int32_t value);
    void SendKeyStateNodeMsgBatch(const int32_t sessionId, const std::vector<struct RawEvent> &events);

    class DInputSinkEventHandler : public AppExecFwk::EventHandler {
    public:
        explicit DInputSinkEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
        ~DInputSinkEventHandler() override = default;

        void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
        void RecordEventLog(const std::shared_ptr<nlohmann::json> &events);
    };

    std::shared_ptr<DistributedInputSinkTransport::DInputSinkEventHandler> GetEventHandler();
    void CloseAllSession();

private:
    int32_t SendMessage(int32_t sessionId, std::string &message);
    void HandleData(int32_t sessionId, const std::string &message);
    void RegRespFunMap();
    void NotifyPrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyStartRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyStopRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyLatency(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyStartRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyStopRemoteInputDhid(int32_t sessionId, const nlohmann::json &recMsg);

    void NotifyRelayPrepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyRelayUnprepareRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyRelayStartDhidRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyRelayStopDhidRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyRelayStartTypeRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);
    void NotifyRelayStopTypeRemoteInput(int32_t sessionId, const nlohmann::json &recMsg);

    void RecordEventLog(const std::string &dhId, int32_t type, int32_t code, int32_t value);
    void RecordEventLog(const std::vector<struct RawEvent> &events);

    void DoSendMsgBatch(const int32_t sessionId, const std::vector<struct RawEvent> &events);
private:
    std::string mySessionName_;
    std::shared_ptr<DistributedInputSinkTransport::DInputSinkEventHandler> eventHandler_;
    std::shared_ptr<DistributedInputSinkTransport::DInputTransbaseSinkListener> statuslistener_;
    std::shared_ptr<DInputSinkTransCallback> callback_;

    using SinkTransportFunc = void (DistributedInputSinkTransport::*)(int32_t sessionId,
        const nlohmann::json &recMsg);
    std::map<int32_t, SinkTransportFunc> memberFuncMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_SINK_TRANSPORT_H
