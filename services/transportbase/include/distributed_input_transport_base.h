/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_TRANSPORT_BASE_H
#define DISTRIBUTED_INPUT_TRANSPORT_BASE_H

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "constants.h"
#include "event_handler.h"
#include "nlohmann/json.hpp"
#include "securec.h"
#include "single_instance.h"

#include "socket.h"
#include "softbus_bus_center.h"

#include "dinput_sink_manager_callback.h"
#include "dinput_source_manager_callback.h"
#include "dinput_transbase_source_callback.h"
#include "dinput_transbase_sink_callback.h"
#include "i_session_state_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputTransportBase {
    DECLARE_SINGLE_INSTANCE_BASE(DistributedInputTransportBase);
public:
    int32_t Init();
    int32_t StartSession(const std::string &remoteDevId);
    void StopSession(const std::string &remoteDevId);
    void StopAllSession();

    void RegisterSrcHandleSessionCallback(std::shared_ptr<DInputTransbaseSourceCallback> callback);
    void RegisterSinkHandleSessionCallback(std::shared_ptr<DInputTransbaseSinkCallback> callback);
    void RegisterSourceManagerCallback(std::shared_ptr<DInputSourceManagerCallback> callback);
    void RegisterSinkManagerCallback(std::shared_ptr<DInputSinkManagerCallback> callback);
    void RegisterSessionStateCb(sptr<ISessionStateCallback> callback);
    void UnregisterSessionStateCb();
    int32_t OnSessionOpened(int32_t sessionId, const PeerSocketInfo &info);
    void OnSessionClosed(int32_t sessionId, ShutdownReason reason);
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);

    int32_t GetCurrentSessionId();
    int32_t CountSession(const std::string &remoteDevId);
    void EraseSessionId(const std::string &remoteDevId);
    int32_t GetSessionIdByDevId(const std::string &srcId);
    std::string GetDevIdBySessionId(int32_t sessionId);
    int32_t SendMsg(int32_t sessionId, std::string &message);

private:
    DistributedInputTransportBase() = default;
    ~DistributedInputTransportBase();
    int32_t CheckDeviceSessionState(const std::string &remoteDevId);
    bool CheckRecivedData(const std::string &message);
    void HandleSession(int32_t sessionId, const std::string &message);
    void Release();
    void RunSessionStateCallback(const std::string &remoteDevId, const uint32_t sessionState);

    int32_t CreateServerSocket();
    int32_t CreateClientSocket(const std::string &remoteDevId);

private:
    std::atomic<bool> isSessSerCreateFlag_ = false;
    std::atomic<int32_t> localServerSocket_;
    std::mutex sessSerOperMutex_;
    std::mutex operationMutex_;
    std::string remoteDeviceId_;
    std::map<std::string, int32_t> remoteDevSessionMap_;
    std::map<std::string, bool> channelStatusMap_;
    std::string localSessionName_ = "";
    int32_t sessionId_ = 0;

    std::shared_ptr<DInputTransbaseSourceCallback> srcCallback_;
    std::shared_ptr<DInputTransbaseSinkCallback> sinkCallback_;
    std::shared_ptr<DInputSourceManagerCallback> srcMgrCallback_;
    std::shared_ptr<DInputSinkManagerCallback> sinkMgrCallback_;
    sptr<ISessionStateCallback> SessionStateCallback_;
};

} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_TRANSPORT_BASE_H