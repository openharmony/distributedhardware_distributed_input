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

#include "distributed_input_transport_base.h"

#include <algorithm>
#include <cstring>

#include "distributed_hardware_fwk_kit.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_hitrace.h"
#include "dinput_log.h"
#include "dinput_softbus_define.h"
#include "dinput_utils_tool.h"
#include "hidumper.h"

#include "softbus_common.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
const int32_t SESSION_STATUS_OPENED = 0;
const int32_t SESSION_STATUS_CLOSED = 1;
static QosTV g_qosInfo[] = {
    { .qos = QOS_TYPE_MIN_BW, .value = 80 * 1024 * 1024},
    { .qos = QOS_TYPE_MAX_LATENCY, .value = 8000 },
    { .qos = QOS_TYPE_MIN_LATENCY, .value = 2000 }
};
static uint32_t g_QosTV_Param_Index = static_cast<uint32_t>(sizeof(g_qosInfo) / sizeof(g_qosInfo[0]));
}
IMPLEMENT_SINGLE_INSTANCE(DistributedInputTransportBase);
DistributedInputTransportBase::~DistributedInputTransportBase()
{
    DHLOGI("Release Transport Session");
    Release();
}

void OnBind(int32_t socket, PeerSocketInfo info)
{
    DistributedInput::DistributedInputTransportBase::GetInstance().OnSessionOpened(socket, info);
}

void OnShutdown(int32_t socket, ShutdownReason reason)
{
    DistributedInput::DistributedInputTransportBase::GetInstance().OnSessionClosed(socket, reason);
}

void OnBytes(int32_t socket, const void *data, uint32_t dataLen)
{
    DistributedInput::DistributedInputTransportBase::GetInstance().OnBytesReceived(socket, data, dataLen);
}

void OnMessage(int32_t socket, const void *data, uint32_t dataLen)
{
    (void)socket;
    (void)data;
    (void)dataLen;
    DHLOGI("socket: %{public}d, dataLen:%{public}d", socket, dataLen);
}

void OnStream(int32_t socket, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    (void)socket;
    (void)data;
    (void)ext;
    (void)param;
    DHLOGI("socket: %{public}d", socket);
}

void OnFile(int32_t socket, FileEvent *event)
{
    (void)event;
    DHLOGI("socket: %{public}d", socket);
}

void OnQos(int32_t socket, QoSEvent eventId, const QosTV *qos, uint32_t qosCount)
{
    DHLOGI("OnQos, socket: %{public}d, QoSEvent: %{public}d, qosCount: %{public}u", socket, (int32_t)eventId, qosCount);
    for (uint32_t idx = 0; idx < qosCount; idx++) {
        DHLOGI("QosTV: type: %{public}d, value: %{public}d", (int32_t)qos[idx].qos, qos[idx].value);
    }
}

ISocketListener iSocketListener = {
    .OnBind = OnBind,
    .OnShutdown = OnShutdown,
    .OnBytes = OnBytes,
    .OnMessage = OnMessage,
    .OnStream = OnStream,
    .OnFile = OnFile,
    .OnQos = OnQos
};

int32_t DistributedInputTransportBase::Init()
{
    DHLOGI("Init Transport Base Session");
    std::unique_lock<std::mutex> sessionServerLock(sessSerOperMutex_);
    if (isSessSerCreateFlag_.load()) {
        DHLOGI("SessionServer already create success.");
        return DH_SUCCESS;
    }
    int32_t socket = CreateServerSocket();
    if (socket < DH_SUCCESS) {
        DHLOGE("CreateServerSocket failed, ret: %{public}d", socket);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_INIT_FAIL;
    }

    int32_t ret = Listen(socket, g_qosInfo, g_QosTV_Param_Index, &iSocketListener);
    if (ret != DH_SUCCESS) {
        DHLOGE("Socket Listen failed, error code %{public}d.", ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_INIT_FAIL;
    }
    isSessSerCreateFlag_.store(true);
    localServerSocket_ = socket;
    DHLOGI("Finish Init DSoftBus Server Socket, socket: %{public}d", socket);
    return DH_SUCCESS;
}

int32_t DistributedInputTransportBase::CreateServerSocket()
{
    DHLOGI("CreateServerSocket start");
    auto localNode = std::make_unique<NodeBasicInfo>();
    int32_t retCode = GetLocalNodeDeviceInfo(DINPUT_PKG_NAME.c_str(), localNode.get());
    if (retCode != DH_SUCCESS) {
        DHLOGE("Init Could not get local device id.");
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_INIT_FAIL;
    }
    std::string networkId = localNode->networkId;
    localSessionName_ = SESSION_NAME + networkId.substr(0, INTERCEPT_STRING_LENGTH);
    DHLOGI("CreateServerSocket local networkId is %{public}s, local socketName: %{public}s",
        GetAnonyString(networkId).c_str(), localSessionName_.c_str());
    SocketInfo info = {
        .name = const_cast<char*>(localSessionName_.c_str()),
        .pkgName = const_cast<char*>(DINPUT_PKG_NAME.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t socket = Socket(info);
    DHLOGI("CreateServerSocket Finish, socket: %{public}d", socket);
    return socket;
}

void DistributedInputTransportBase::Release()
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    auto iter = remoteDevSessionMap_.begin();
    for (; iter != remoteDevSessionMap_.end(); ++iter) {
        DHLOGI("Shutdown client socket: %{public}d to remote dev: %{public}s", iter->second,
            GetAnonyString(iter->first).c_str());
        Shutdown(iter->second);
    }

    {
        std::unique_lock<std::mutex> sessionServerLock(sessSerOperMutex_);
        if (!isSessSerCreateFlag_.load()) {
            DHLOGI("DSoftBus Server Socket already remove success.");
        } else {
            DHLOGI("Shutdown DSoftBus Server Socket, socket: %{public}d", localServerSocket_.load());
            Shutdown(localServerSocket_.load());
            localServerSocket_ = -1;
            isSessSerCreateFlag_.store(false);
        }
    }
    remoteDevSessionMap_.clear();
    channelStatusMap_.clear();
}

int32_t DistributedInputTransportBase::CheckDeviceSessionState(const std::string &remoteDevId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (remoteDevSessionMap_.find(remoteDevId) == remoteDevSessionMap_.end()) {
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_DEVICE_SESSION_STATE;
    }
    DHLOGI("CheckDeviceSessionState has opened, remoteDevId: %{public}s", GetAnonyString(remoteDevId).c_str());
    return DH_SUCCESS;
}

std::string DistributedInputTransportBase::GetDevIdBySessionId(int32_t sessionId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    for (auto iter = remoteDevSessionMap_.begin(); iter != remoteDevSessionMap_.end(); ++iter) {
        if (iter->second == sessionId) {
            return iter->first;
        }
    }
    return "";
}

int32_t DistributedInputTransportBase::CreateClientSocket(const std::string &remoteDevId)
{
    DHLOGI("CreateClientSocket start, peerNetworkId: %{public}s", GetAnonyString(remoteDevId).c_str());
    std::string localSesionName = localSessionName_ + "_" + std::to_string(GetCurrentTimeUs());
    std::string peerSessionName = SESSION_NAME + remoteDevId.substr(0, INTERCEPT_STRING_LENGTH);
    SocketInfo info = {
        .name = const_cast<char*>(localSesionName.c_str()),
        .peerName = const_cast<char*>(peerSessionName.c_str()),
        .peerNetworkId = const_cast<char*>(remoteDevId.c_str()),
        .pkgName = const_cast<char*>(DINPUT_PKG_NAME.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t socket = Socket(info);
    DHLOGI("Bind Socket server, socket: %{public}d, localSessionName: %{public}s, peerSessionName: %{public}s",
        socket, localSesionName.c_str(), peerSessionName.c_str());
    return socket;
}

int32_t DistributedInputTransportBase::StartSession(const std::string &remoteDevId)
{
    int32_t ret = CheckDeviceSessionState(remoteDevId);
    if (ret == DH_SUCCESS) {
        DHLOGE("Softbus session has already opened, deviceId: %{public}s", GetAnonyString(remoteDevId).c_str());
        return DH_SUCCESS;
    }

    int socket = CreateClientSocket(remoteDevId);
    if (socket < DH_SUCCESS) {
        DHLOGE("StartSession failed, ret: %{public}d", socket);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_OPEN_SESSION_FAIL;
    }
    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_OPEN_SESSION_START, DINPUT_OPEN_SESSION_TASK);
    ret = Bind(socket, g_qosInfo, g_QosTV_Param_Index, &iSocketListener);
    if (ret < DH_SUCCESS) {
        DHLOGE("OpenSession fail, remoteDevId: %{public}s, socket: %{public}d", GetAnonyString(remoteDevId).c_str(),
            socket);
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_OPEN_SESSION_START, DINPUT_OPEN_SESSION_TASK);
        Shutdown(socket);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_OPEN_SESSION_FAIL;
    }

    std::string peerSessionName = SESSION_NAME + remoteDevId.substr(0, INTERCEPT_STRING_LENGTH);
    HiDumper::GetInstance().CreateSessionInfo(remoteDevId, socket, localSessionName_, peerSessionName,
        SessionStatus::OPENED);
    DHLOGI("OpenSession success, remoteDevId:%{public}s, sessionId: %{public}d", GetAnonyString(remoteDevId).c_str(),
        socket);
    sessionId_ = socket;

    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        DHLOGD("Enable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, ENABLE_LOW_LATENCY.dump());
    }

    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(peerSessionName.c_str()),
        .networkId = const_cast<char*>(remoteDevId.c_str()),
        .pkgName = const_cast<char*>(DINPUT_PKG_NAME.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    OnSessionOpened(socket, peerSocketInfo);
    return DH_SUCCESS;
}

int32_t DistributedInputTransportBase::GetCurrentSessionId()
{
    return sessionId_;
}

void DistributedInputTransportBase::StopAllSession()
{
    std::map<std::string, int32_t> remoteDevSessions;
    {
        std::unique_lock<std::mutex> sessionLock(operationMutex_);
        std::for_each(remoteDevSessionMap_.begin(), remoteDevSessionMap_.end(),
            [&remoteDevSessions] (const std::pair<std::string, int32_t> &pair) {
            remoteDevSessions[pair.first] = pair.second;
        });
    }

    std::for_each(remoteDevSessions.begin(), remoteDevSessions.end(),
        [this](const std::pair<std::string, int32_t> &pair) {
        StopSession(pair.first);
    });
}

void DistributedInputTransportBase::StopSession(const std::string &remoteDevId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (remoteDevSessionMap_.count(remoteDevId) == 0) {
        DHLOGE("remoteDevSessionMap not find remoteDevId: %{public}s", GetAnonyString(remoteDevId).c_str());
        return;
    }
    int32_t sessionId = remoteDevSessionMap_[remoteDevId];

    DHLOGI("RemoteDevId: %{public}s, sessionId: %{public}d", GetAnonyString(remoteDevId).c_str(), sessionId);
    HiDumper::GetInstance().SetSessionStatus(remoteDevId, SessionStatus::CLOSING);
    Shutdown(sessionId);
    remoteDevSessionMap_.erase(remoteDevId);
    channelStatusMap_.erase(remoteDevId);

    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        DHLOGD("Disable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, DISABLE_LOW_LATENCY.dump());
    }

    HiDumper::GetInstance().SetSessionStatus(remoteDevId, SessionStatus::CLOSED);
    HiDumper::GetInstance().DeleteSessionInfo(remoteDevId);
}

void DistributedInputTransportBase::RegisterSrcHandleSessionCallback(
    std::shared_ptr<DInputTransbaseSourceCallback> callback)
{
    DHLOGI("RegisterSrcHandleSessionCallback");
    srcCallback_ = callback;
}

void DistributedInputTransportBase::RegisterSinkHandleSessionCallback(
    std::shared_ptr<DInputTransbaseSinkCallback> callback)
{
    DHLOGI("RegisterSinkHandleSessionCallback");
    sinkCallback_ = callback;
}

void DistributedInputTransportBase::RegisterSourceManagerCallback(
    std::shared_ptr<DInputSourceManagerCallback> callback)
{
    DHLOGI("RegisterSourceManagerCallback");
    srcMgrCallback_ = callback;
}

void DistributedInputTransportBase::RegisterSinkManagerCallback(
    std::shared_ptr<DInputSinkManagerCallback> callback)
{
    DHLOGI("RegisterSinkManagerCallback");
    sinkMgrCallback_ = callback;
}

void DistributedInputTransportBase::RegisterSessionStateCb(sptr<ISessionStateCallback> callback)
{
    DHLOGI("RegisterSessionStateCb");
    SessionStateCallback_ = callback;
}

void DistributedInputTransportBase::UnregisterSessionStateCb()
{
    DHLOGI("UnregisterSessionStateCb");
    SessionStateCallback_ = nullptr;
}

void DistributedInputTransportBase::RunSessionStateCallback(const std::string &remoteDevId,
    const uint32_t sessionState)
{
    DHLOGI("RunSessionStateCallback start.");
    if (SessionStateCallback_ != nullptr) {
        SessionStateCallback_->OnResult(remoteDevId, sessionState);
        return;
    }
    DHLOGE("RunSessionStateCallback SessionStateCallback_ is null.");
}

int32_t DistributedInputTransportBase::CountSession(const std::string &remoteDevId)
{
    return remoteDevSessionMap_.count(remoteDevId);
}

void DistributedInputTransportBase::EraseSessionId(const std::string &remoteDevId)
{
    remoteDevSessionMap_.erase(remoteDevId);
}

int32_t DistributedInputTransportBase::OnSessionOpened(int32_t sessionId, const PeerSocketInfo &info)
{
    std::string peerDevId;
    peerDevId.assign(info.networkId);
    DHLOGI("OnSessionOpened, socket: %{public}d, peerSocketName: %{public}s, peerNetworkId: %{public}s, "
        "peerPkgName: %{public}s", sessionId, info.name, GetAnonyString(peerDevId).c_str(), info.pkgName);
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_OPEN_SESSION_START, DINPUT_OPEN_SESSION_TASK);

    {
        std::unique_lock<std::mutex> sessionLock(operationMutex_);
        remoteDevSessionMap_[peerDevId] = sessionId;
        channelStatusMap_[peerDevId] = true;
    }
    RunSessionStateCallback(peerDevId, SESSION_STATUS_OPENED);
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        DHLOGD("Enable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, ENABLE_LOW_LATENCY.dump());
    }
    DHLOGI("OnSessionOpened finish");
    return DH_SUCCESS;
}

void DistributedInputTransportBase::OnSessionClosed(int32_t sessionId, ShutdownReason reason)
{
    DHLOGI("OnSessionClosed, socket: %{public}d, reason: %{public}d", sessionId, (int32_t)reason);
    std::string deviceId = GetDevIdBySessionId(sessionId);
    DHLOGI("OnSessionClosed notify session closed, sessionId: %{public}d, peer deviceId:%{public}s",
        sessionId, GetAnonyString(deviceId).c_str());
    RunSessionStateCallback(deviceId, SESSION_STATUS_CLOSED);

    {
        std::unique_lock<std::mutex> sessionLock(operationMutex_);
        if (CountSession(deviceId) > 0) {
            EraseSessionId(deviceId);
        }
        channelStatusMap_.erase(deviceId);

        if (sinkCallback_ == nullptr) {
            DHLOGE("sinkCallback is nullptr.");
            return;
        }
        sinkCallback_->NotifySessionClosed(sessionId);

        if (srcCallback_ == nullptr) {
            DHLOGE("srcCallback is nullptr.");
            return;
        }
        srcCallback_->NotifySessionClosed();

        if (srcMgrCallback_ == nullptr) {
            DHLOGE("srcMgrCallback is nullptr.");
            return;
        }
        srcMgrCallback_->ResetSrcMgrResStatus();

        if (sinkMgrCallback_ == nullptr) {
            DHLOGE("sinkMgrCallback is nullptr.");
            return;
        }
        sinkMgrCallback_->ResetSinkMgrResStatus();
    }

    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        DHLOGD("Disable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, DISABLE_LOW_LATENCY.dump());
    }
    DHLOGI("OnSessionClosed finish");
}

bool DistributedInputTransportBase::CheckRecivedData(const std::string &message)
{
    nlohmann::json recMsg = nlohmann::json::parse(message, nullptr, false);
    if (recMsg.is_discarded()) {
        DHLOGE("OnBytesReceived jsonStr error.");
        return false;
    }

    if (!IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_CMD_TYPE)) {
        DHLOGE("The key is invalid.");
        return false;
    }

    return true;
}

void DistributedInputTransportBase::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    if (sessionId < 0 || data == nullptr || dataLen <= 0) {
        DHLOGE("OnBytesReceived param check failed");
        return;
    }

    uint8_t *buf = reinterpret_cast<uint8_t *>(calloc(dataLen + 1, sizeof(uint8_t)));
    if (buf == nullptr) {
        DHLOGE("OnBytesReceived: malloc memory failed");
        return;
    }

    if (memcpy_s(buf, dataLen + 1,  reinterpret_cast<const uint8_t *>(data), dataLen) != EOK) {
        DHLOGE("OnBytesReceived: memcpy memory failed");
        free(buf);
        return;
    }

    std::string message(buf, buf + dataLen);
    HandleSession(sessionId, message);

    free(buf);
    return;
}

void DistributedInputTransportBase::HandleSession(int32_t sessionId, const std::string &message)
{
    if (CheckRecivedData(message) != true) {
        return;
    }
    nlohmann::json recMsg = nlohmann::json::parse(message, nullptr, false);
    if (recMsg.is_discarded()) {
        DHLOGE("recMsg parse failed!");
        return;
    }
    if (!IsUInt32(recMsg, DINPUT_SOFTBUS_KEY_CMD_TYPE)) {
        DHLOGE("softbus cmd key is invalid");
        return;
    }
    uint32_t cmdType = recMsg[DINPUT_SOFTBUS_KEY_CMD_TYPE];
    if (cmdType < TRANS_MSG_SRC_SINK_SPLIT) {
        if (srcCallback_ == nullptr) {
            DHLOGE("srcCallback is nullptr.");
            return;
        }
        srcCallback_->HandleSessionData(sessionId, message);
        return;
    }
    if (cmdType > TRANS_MSG_SRC_SINK_SPLIT) {
        if (sinkCallback_ == nullptr) {
            DHLOGE("sinkCallback is nullptr.");
            return;
        }
        sinkCallback_->HandleSessionData(sessionId, message);
    }
}

int32_t DistributedInputTransportBase::SendMsg(int32_t sessionId, std::string &message)
{
    if (message.size() > MSG_MAX_SIZE) {
        DHLOGE("SendMessage error: message.size() > MSG_MAX_SIZE, msg size: %{public}zu", message.size());
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_SENDMESSSAGE;
    }
    uint8_t *buf = reinterpret_cast<uint8_t *>(calloc((MSG_MAX_SIZE), sizeof(uint8_t)));
    if (buf == nullptr) {
        DHLOGE("SendMsg: malloc memory failed");
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_SENDMESSSAGE;
    }
    int32_t outLen = 0;
    if (memcpy_s(buf, MSG_MAX_SIZE, reinterpret_cast<const uint8_t *>(message.c_str()), message.size()) != EOK) {
        DHLOGE("SendMsg: memcpy memory failed");
        free(buf);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_SENDMESSSAGE;
    }
    outLen = static_cast<int32_t>(message.size());
    int32_t ret = SendBytes(sessionId, buf, outLen);
    free(buf);
    return ret;
}

int32_t DistributedInputTransportBase::GetSessionIdByDevId(const std::string &srcId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    std::map<std::string, int32_t>::iterator it = remoteDevSessionMap_.find(srcId);
    if (it != remoteDevSessionMap_.end()) {
        return it->second;
    }
    DHLOGE("get session id failed, srcId = %{public}s", GetAnonyString(srcId).c_str());
    return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_GET_SESSIONID_FAIL;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS