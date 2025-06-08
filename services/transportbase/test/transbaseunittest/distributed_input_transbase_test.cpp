/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "distributed_input_transbase_test.h"

#include <cstdlib>

#include "dinput_errcode.h"
#include "dinput_softbus_define.h"
#include "softbus_permission_check.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    const std::string PEER_SESSION_NAME = "ohos.dhardware.dinput.session8647073e02e7a78f09473aa122";
    const std::string REMOTE_DEV_ID = "f6d4c0864707aefte7a78f09473aa122ff57fc81c00981fcf5be989e7d112591";
    const std::string DINPUT_PKG_NAME_TEST = "ohos.dhardware.dinput";
}

static bool g_checkSrc = true;
static bool g_checkSink = true;
static bool g_setAccess = true;
static bool g_transCaller = true;
static bool g_fillLocal = true;

bool SoftBusPermissionCheck::CheckSrcPermission(const std::string &sinkNetworkId)
{
    return g_checkSrc;
}

bool SoftBusPermissionCheck::CheckSinkPermission(const AccountInfo &callerAccountInfo)
{
    return g_checkSink;
}

bool SoftBusPermissionCheck::SetAccessInfoToSocket(const int32_t sessionId)
{
    return g_setAccess;
}
bool SoftBusPermissionCheck::TransCallerInfo(SocketAccessInfo *callerInfo,
    AccountInfo &callerAccountInfo, const std::string &networkId)
{
    return g_transCaller;
}

bool SoftBusPermissionCheck::FillLocalInfo(SocketAccessInfo *localInfo)
{
    return g_fillLocal;
}

void DistributedInputTransbaseTest::SetUp()
{
    g_checkSrc = true;
    g_checkSink = true;
    g_setAccess = true;
    g_transCaller = true;
    g_fillLocal = true;
}

void DistributedInputTransbaseTest::TearDown()
{
}

void DistributedInputTransbaseTest::SetUpTestCase()
{
}

void DistributedInputTransbaseTest::TearDownTestCase()
{
}

void DistributedInputTransbaseTest::TestRegisterSessionStateCallbackStub::OnResult(const std::string &devId,
    const uint32_t status)
{
    (void)devId;
    (void)status;
}

HWTEST_F(DistributedInputTransbaseTest, Init01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputTransportBase::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputTransbaseTest, StartSession01, testing::ext::TestSize.Level1)
{
    std::string remoteDevId = "";
    int32_t ret = DistributedInputTransportBase::GetInstance().StartSession(remoteDevId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputTransbaseTest, StartSession02, testing::ext::TestSize.Level1)
{
    std::string srcId = "f6d4c08647073e02e7a78f09473aa122ff57fc81c00981fcf5be989e7d112591";
    int32_t sessionId = 2;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = DistributedInputTransportBase::GetInstance().StartSession(srcId);
    EXPECT_EQ(DH_SUCCESS, ret);

    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    g_setAccess = false;
    ret = DistributedInputTransportBase::GetInstance().StartSession(srcId);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_CONTEXT, ret);

    g_checkSrc = false;
    ret = DistributedInputTransportBase::GetInstance().StartSession(srcId);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_PERMISSION_DENIED, ret);
}

HWTEST_F(DistributedInputTransbaseTest, GetDevIdBySessionId01, testing::ext::TestSize.Level1)
{
    int32_t sessionId = 0;
    std::string srcId = "f6d4c08647073e02e7a78f09473aa122ff57fc81c00981fcf5be989e7d112591";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    std::string ret = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    EXPECT_EQ(srcId, ret);
    ret = DistributedInputTransportBase::GetInstance().GetDevIdBySessionId(sessionId);
    EXPECT_EQ("", ret);
}

HWTEST_F(DistributedInputTransbaseTest, OnSessionOpened01, testing::ext::TestSize.Level1)
{
    int32_t sessionId = 0;
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DINPUT_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    std::string srcId = "f6d4c08647073e02e7a78f09473aa122ff57fc81c00981fcf5be989e7d112591";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = DistributedInputTransportBase::GetInstance().OnSessionOpened(sessionId, peerSocketInfo);
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = DistributedInputTransportBase::GetInstance().OnSessionOpened(sessionId, peerSocketInfo);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputTransbaseTest, HandleSession01, testing::ext::TestSize.Level1)
{
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t sessionId = 1;
    nlohmann::json recMsg;
    recMsg[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SINK_MSG_ON_RELAY_STOPTYPE;
    std::string message = recMsg.dump();
    DistributedInputTransportBase::GetInstance().HandleSession(sessionId, message);
    EXPECT_EQ(0, DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.size());
}

HWTEST_F(DistributedInputTransbaseTest, HandleSession02, testing::ext::TestSize.Level1)
{
    int32_t sessionId = 1;
    nlohmann::json recMsg;
    recMsg[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_MSG_PREPARE;
    std::string message = recMsg.dump();
    DistributedInputTransportBase::GetInstance().HandleSession(sessionId, message);
    EXPECT_EQ(0, DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.size());
}

HWTEST_F(DistributedInputTransbaseTest, CheckRecivedData01, testing::ext::TestSize.Level1)
{
    std::string message = "";
    DistributedInputTransportBase::GetInstance().CheckRecivedData(message);
    message = "message_test";
    DistributedInputTransportBase::GetInstance().CheckRecivedData(message);
    nlohmann::json recMsg;
    recMsg[DINPUT_SOFTBUS_KEY_CMD_TYPE] = "cmd_type_test";
    DistributedInputTransportBase::GetInstance().CheckRecivedData(recMsg.dump());
    recMsg[DINPUT_SOFTBUS_KEY_CMD_TYPE] = TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID_RESULT;
    DistributedInputTransportBase::GetInstance().CheckRecivedData(recMsg.dump());
    EXPECT_EQ(0, DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.size());
}

HWTEST_F(DistributedInputTransbaseTest, OnBytesReceived01, testing::ext::TestSize.Level1)
{
    int32_t sessionId = -1;
    char *data = nullptr;
    uint32_t dataLen = -1;
    DistributedInputTransportBase::GetInstance().OnBytesReceived(sessionId, data, dataLen);
    sessionId = 1;
    DistributedInputTransportBase::GetInstance().OnBytesReceived(sessionId, data, dataLen);
    char dataMsg[10] = "dataMsg";
    DistributedInputTransportBase::GetInstance().OnBytesReceived(sessionId, dataMsg, dataLen);
    dataLen = 1;
    DistributedInputTransportBase::GetInstance().OnBytesReceived(sessionId, dataMsg, dataLen);
    EXPECT_EQ(0, DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.size());
}

HWTEST_F(DistributedInputTransbaseTest, SendMsg01, testing::ext::TestSize.Level1)
{
    int32_t sessionId = 1;
    std::string message(MSG_MAX_SIZE + 1, 'a');
    int32_t ret = DistributedInputTransportBase::GetInstance().SendMsg(sessionId, message);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_TRANSPORT_SENDMESSSAGE, ret);

    std::string messageData(MSG_MAX_SIZE - 1, 'a');
    ret = DistributedInputTransportBase::GetInstance().SendMsg(sessionId, messageData);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputTransbaseTest, Release01, testing::ext::TestSize.Level1)
{
    DistributedInputTransportBase::GetInstance().Release();
    EXPECT_EQ(0, DistributedInputTransportBase::GetInstance().channelStatusMap_.size());
}

HWTEST_F(DistributedInputTransbaseTest, StopSession_001, testing::ext::TestSize.Level1)
{
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    std::string remoteDevId = "remoteDevId_test";
    DistributedInputTransportBase::GetInstance().StopSession(remoteDevId);
    EXPECT_EQ(0, DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.size());
}

HWTEST_F(DistributedInputTransbaseTest, RunSessionStateCallback_001, testing::ext::TestSize.Level1)
{
    std::string remoteDevId = "remoteDevId_test";
    uint32_t sessionState = 1;
    sptr<TestRegisterSessionStateCallbackStub> callback(new TestRegisterSessionStateCallbackStub());
    DistributedInputTransportBase::GetInstance().RegisterSessionStateCb(callback);
    DistributedInputTransportBase::GetInstance().RunSessionStateCallback(remoteDevId, sessionState);
    DistributedInputTransportBase::GetInstance().UnregisterSessionStateCb();
    auto ret = DistributedInputTransportBase::GetInstance().CountSession(remoteDevId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputTransbaseTest, OnSessionClosed_001, testing::ext::TestSize.Level1)
{
    int32_t sessionId = 1;
    std::string devId = "devId_741258";
    ShutdownReason reason = ShutdownReason::SHUTDOWN_REASON_UNKNOWN;
    DistributedInputTransportBase::GetInstance().OnSessionClosed(sessionId, reason);

    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[devId] = sessionId;
    DistributedInputTransportBase::GetInstance().OnSessionClosed(sessionId, reason);

    DistributedInputTransportBase::GetInstance().sinkCallback_ = std::shared_ptr<DInputTransbaseSinkCallback>();
    DistributedInputTransportBase::GetInstance().OnSessionClosed(sessionId, reason);

    DistributedInputTransportBase::GetInstance().srcCallback_ = std::shared_ptr<DInputTransbaseSourceCallback>();
    DistributedInputTransportBase::GetInstance().OnSessionClosed(sessionId, reason);

    DistributedInputTransportBase::GetInstance().srcMgrCallback_ = std::shared_ptr<DInputSourceManagerCallback>();
    DistributedInputTransportBase::GetInstance().OnSessionClosed(sessionId, reason);
    auto ret = DistributedInputTransportBase::GetInstance().CountSession(devId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputTransbaseTest, OnNegotiate2_001, testing::ext::TestSize.Level1)
{
    PeerSocketInfo info;
    EXPECT_TRUE(DistributedInputTransportBase::GetInstance().OnNegotiate2(0, info, nullptr, nullptr));

    char networkId[10] = "networkId";
    SocketAccessInfo peerInfo;
    info.networkId = networkId;
    g_transCaller = false;
    EXPECT_FALSE(DistributedInputTransportBase::GetInstance().OnNegotiate2(0, info, &peerInfo, nullptr));

    g_transCaller = true;
    g_fillLocal = false;
    EXPECT_FALSE(DistributedInputTransportBase::GetInstance().OnNegotiate2(0, info, &peerInfo, nullptr));

    g_fillLocal = true;
    g_checkSink = false;
    EXPECT_FALSE(DistributedInputTransportBase::GetInstance().OnNegotiate2(0, info, &peerInfo, nullptr));

    g_checkSink = true;
    EXPECT_TRUE(DistributedInputTransportBase::GetInstance().OnNegotiate2(0, info, &peerInfo, nullptr));
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
