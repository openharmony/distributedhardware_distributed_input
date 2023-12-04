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

#ifndef COMPILE_TEST_MODE
#include "session.h"
#else
#include "session_mock.h"
#endif

#ifndef COMPILE_TEST_MODE
#include "softbus_bus_center.h"
#else
#include "softbus_bus_center_mock.h"
#endif

#include "softbus_common.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
const int32_t DINPUT_LINK_TYPE_MAX = 4;
const int32_t SESSION_STATUS_OPENED = 0;
const int32_t SESSION_STATUS_CLOSED = 1;
static SessionAttribute g_sessionAttr = {
    .dataType = SessionType::TYPE_BYTES,
    .linkTypeNum = DINPUT_LINK_TYPE_MAX,
    .linkType = {
        LINK_TYPE_WIFI_P2P,
        LINK_TYPE_WIFI_WLAN_2G,
        LINK_TYPE_WIFI_WLAN_5G,
        LINK_TYPE_BR
    }
};
IMPLEMENT_SINGLE_INSTANCE(DistributedInputTransportBase);
DistributedInputTransportBase::~DistributedInputTransportBase()
{
    DHLOGI("Release Transport Session");
    Release();
}

static int32_t SessionOpened(int32_t sessionId, int32_t result)
{
    return DistributedInput::DistributedInputTransportBase::GetInstance().OnSessionOpened(sessionId, result);
}

static void SessionClosed(int32_t sessionId)
{
    DistributedInput::DistributedInputTransportBase::GetInstance().OnSessionClosed(sessionId);
}

static void BytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    DistributedInput::DistributedInputTransportBase::GetInstance().OnBytesReceived(sessionId, data, dataLen);
}

static void MessageReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    (void)sessionId;
    (void)data;
    (void)dataLen;
    DHLOGI("sessionId: %d, dataLen:%d", sessionId, dataLen);
}

static void StreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    (void)sessionId;
    (void)data;
    (void)ext;
    (void)param;
    DHLOGI("sessionId: %d", sessionId);
}

int32_t DistributedInputTransportBase::Init()
{
    DHLOGI("Init Transport Base Session");
    ISessionListener iSessionListener = {
        .OnSessionOpened = SessionOpened,
        .OnSessionClosed = SessionClosed,
        .OnBytesReceived = BytesReceived,
        .OnMessageReceived = MessageReceived,
        .OnStreamReceived = StreamReceived
    };

    auto localNode = std::make_unique<NodeBasicInfo>();
    int32_t retCode = GetLocalNodeDeviceInfo(DINPUT_PKG_NAME.c_str(), localNode.get());
    if (retCode != DH_SUCCESS) {
        DHLOGE("Init Could not get local device id.");
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_INIT_FAIL;
    }
    std::string networkId = localNode->networkId;
    DHLOGI("Init device local networkId is %s", GetAnonyString(networkId).c_str());

    std::unique_lock<std::mutex> sessionServerLock(sessSerOperMutex_);
    if (isSessSerCreateFlag_.load()) {
        DHLOGI("SessionServer already create success.");
        return DH_SUCCESS;
    }
    localSessionName_ = SESSION_NAME + networkId.substr(0, INTERCEPT_STRING_LENGTH);
    int32_t ret = CreateSessionServer(DINPUT_PKG_NAME.c_str(), localSessionName_.c_str(), &iSessionListener);
    if (ret != DH_SUCCESS) {
        DHLOGE("Init CreateSessionServer failed, error code %d.", ret);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_INIT_FAIL;
    }
    isSessSerCreateFlag_.store(true);
    return DH_SUCCESS;
}

void DistributedInputTransportBase::Release()
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    auto iter = remoteDevSessionMap_.begin();
    for (; iter != remoteDevSessionMap_.end(); ++iter) {
        CloseSession(iter->second);
    }

    {
        std::unique_lock<std::mutex> sessionServerLock(sessSerOperMutex_);
        if (!isSessSerCreateFlag_.load()) {
            DHLOGI("SessionServer already remove success.");
        } else {
            (void)RemoveSessionServer(DINPUT_PKG_NAME.c_str(), localSessionName_.c_str());
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
    DHLOGI("CheckDeviceSessionState has opened %s", GetAnonyString(remoteDevId).c_str());
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

int32_t DistributedInputTransportBase::StartSession(const std::string &remoteDevId)
{
    int32_t ret = CheckDeviceSessionState(remoteDevId);
    if (ret == DH_SUCCESS) {
        DHLOGE("Softbus session has already opened, deviceId: %s", GetAnonyString(remoteDevId).c_str());
        return DH_SUCCESS;
    }

    std::string peerSessionName = SESSION_NAME + remoteDevId.substr(0, INTERCEPT_STRING_LENGTH);
    DHLOGI("OpenInputSoftbus localSessionName: %s, peerSessionName:%s, remoteDevId: %s",
        localSessionName_.c_str(), peerSessionName.c_str(), remoteDevId.c_str());

    StartAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_OPEN_SESSION_START, DINPUT_OPEN_SESSION_TASK);
    int32_t sessionId = OpenSession(localSessionName_.c_str(), peerSessionName.c_str(), remoteDevId.c_str(),
        GROUP_ID.c_str(), &g_sessionAttr);
    if (sessionId < 0) {
        DHLOGE("OpenSession fail, remoteDevId: %s, sessionId: %d", GetAnonyString(remoteDevId).c_str(), sessionId);
        FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_OPEN_SESSION_START, DINPUT_OPEN_SESSION_TASK);
        return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_OPEN_SESSION_FAIL;
    }

    HiDumper::GetInstance().CreateSessionInfo(remoteDevId, sessionId, localSessionName_, peerSessionName,
        SessionStatus::OPENING);

    DHLOGI("Wait for channel session opened.");
    {
        std::unique_lock<std::mutex> waitLock(operationMutex_);
        auto status = openSessionWaitCond_.wait_for(waitLock, std::chrono::seconds(SESSION_WAIT_TIMEOUT_SECOND),
            [this, remoteDevId] () { return channelStatusMap_[remoteDevId]; });
        if (!status) {
            DHLOGE("OpenSession timeout, remoteDevId: %s, sessionId: %d",
                GetAnonyString(remoteDevId).c_str(), sessionId);
            return ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_OPEN_SESSION_TIMEOUT;
        }
    }

    DHLOGI("OpenSession success, remoteDevId:%s, sessionId: %d", GetAnonyString(remoteDevId).c_str(), sessionId);
    sessionId_ = sessionId;

    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DInputContext::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        DHLOGD("Enable low Latency!");
        dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, ENABLE_LOW_LATENCY.dump());
    }

    HiDumper::GetInstance().SetSessionStatus(remoteDevId, SessionStatus::OPENED);
    return DH_SUCCESS;
}

int32_t DistributedInputTransportBase::GetCurrentSessionId()
{
    return sessionId_;
}

void DistributedInputTransportBase::StopSession(const std::string &remoteDevId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);

    if (remoteDevSessionMap_.count(remoteDevId) == 0) {
        DHLOGE("remoteDevSessionMap not find remoteDevId: %s", GetAnonyString(remoteDevId).c_str());
        return;
    }
    int32_t sessionId = remoteDevSessionMap_[remoteDevId];

    DHLOGI("RemoteDevId: %s, sessionId: %d", GetAnonyString(remoteDevId).c_str(), sessionId);
    HiDumper::GetInstance().SetSessionStatus(remoteDevId, SessionStatus::CLOSING);
    CloseSession(sessionId);
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
    DHLOGI("RegisterTransbaseSourceRespCallback");
    srcCallback_ = callback;
}

void DistributedInputTransportBase::RegisterSinkHandleSessionCallback(
    std::shared_ptr<DInputTransbaseSinkCallback> callback)
{
    DHLOGI("RegisterTransbaseSinkRespCallback");
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
    DHLOGI("RegisterSourceManagerCallback");
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

int32_t DistributedInputTransportBase::OnSessionOpened(int32_t sessionId, int32_t result)
{
    DHLOGI("OnSessionOpened, sessionId: %d, result: %d", sessionId, result);
    FinishAsyncTrace(DINPUT_HITRACE_LABEL, DINPUT_OPEN_SESSION_START, DINPUT_OPEN_SESSION_TASK);
    if (result != DH_SUCCESS) {
        std::string deviceId = GetDevIdBySessionId(sessionId);
        DHLOGE("session open failed, sessionId: %d, result:%d, deviceId:%s", sessionId, result,
            GetAnonyString(deviceId).c_str());
        std::unique_lock<std::mutex> sessionLock(operationMutex_);
        if (CountSession(deviceId) > 0) {
            EraseSessionId(deviceId);
        }
        return DH_SUCCESS;
    }

    char mySessionName[SESSION_NAME_SIZE_MAX] = {0};
    char peerSessionName[SESSION_NAME_SIZE_MAX] = {0};
    char peerDevId[DEVICE_ID_SIZE_MAX] = {0};
    int32_t ret = GetMySessionName(sessionId, mySessionName, sizeof(mySessionName));
    if (ret != DH_SUCCESS) {
        DHLOGE("get my session name failed, session id is %d", sessionId);
    }
    ret = GetPeerSessionName(sessionId, peerSessionName, sizeof(peerSessionName));
    if (ret != DH_SUCCESS) {
        DHLOGE("get peer session name failed, session id is %d", sessionId);
    }
    ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != DH_SUCCESS) {
        DHLOGE("get peer device id failed, session id is %d", sessionId);
    }

    {
        std::unique_lock<std::mutex> sessionLock(operationMutex_);
        remoteDevSessionMap_[peerDevId] = sessionId;
    }

    int32_t sessionSide = GetSessionSide(sessionId);
    DHLOGI("session open succeed, sessionId: %d, sessionSide:%d(1 is "
        "client side), deviceId:%s", sessionId, sessionSide, GetAnonyString(peerDevId).c_str());

    DHLOGI("mySessionName:%s, peerSessionName:%s, peerDevId:%s",
        mySessionName, peerSessionName, GetAnonyString(peerDevId).c_str());
    {
        std::lock_guard<std::mutex> notifyLock(operationMutex_);
        channelStatusMap_[peerDevId] = true;
        openSessionWaitCond_.notify_all();
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

void DistributedInputTransportBase::OnSessionClosed(int32_t sessionId)
{
    DHLOGI("OnSessionClosed, sessionId: %d", sessionId);
    std::string deviceId = GetDevIdBySessionId(sessionId);
    DHLOGI("OnSessionClosed notify session closed, sessionId: %d, peer deviceId:%s",
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
    DHLOGI("OnBytesReceived, sessionId: %d, dataLen:%d", sessionId, dataLen);
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
    DHLOGI("OnBytesReceived message:%s.", SetAnonyId(message).c_str());
    HandleSession(sessionId, message);

    free(buf);
    DHLOGI("OnBytesReceived completed");
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
    DHLOGI("HandleSession cmdType %u.", cmdType);
    if (cmdType < TRANS_MSG_SRC_SINK_SPLIT) {
        if (srcCallback_ == nullptr) {
            DHLOGE("srcCallback is nullptr.");
            return;
        }
        DHLOGI("HandleSession to source.");
        srcCallback_->HandleSessionData(sessionId, message);
        return;
    }
    if (cmdType > TRANS_MSG_SRC_SINK_SPLIT) {
        if (sinkCallback_ == nullptr) {
            DHLOGE("sinkCallback is nullptr.");
            return;
        }
        DHLOGI("HandleSession to sink.");
        sinkCallback_->HandleSessionData(sessionId, message);
    }
}

int32_t DistributedInputTransportBase::SendMsg(int32_t sessionId, std::string &message)
{
    DHLOGD("start SendMsg");
    if (message.size() > MSG_MAX_SIZE) {
        DHLOGE("SendMessage error: message.size() > MSG_MAX_SIZE");
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
    std::map<std::string, int32_t>::iterator it = remoteDevSessionMap_.find(srcId);
    if (it != remoteDevSessionMap_.end()) {
        return it->second;
    }
    DHLOGE("get session id failed, srcId = %s", GetAnonyString(srcId).c_str());
    return ERR_DH_INPUT_SERVER_SINK_TRANSPORT_GET_SESSIONID_FAIL;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS