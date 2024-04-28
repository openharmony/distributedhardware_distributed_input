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

#include "distributed_input_ipc_test.h"

#include "nlohmann/json.hpp"

#include "dinput_errcode.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputIpcTest::SetUp()
{
}

void DistributedInputIpcTest::TearDown()
{
}

void DistributedInputIpcTest::SetUpTestCase()
{
}

void DistributedInputIpcTest::TearDownTestCase()
{
}

void DistributedInputIpcTest::TestInputNodeListener::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
    return;
}

void DistributedInputIpcTest::TestInputNodeListener::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    return;
}

int32_t DistributedInputIpcTest::TestSimulationEventListenerStub::OnSimulationEvent(
    uint32_t type, uint32_t code, int32_t value)
{
    (void)type;
    (void)code;
    (void)value;
    return DH_SUCCESS;
}

HWTEST_F(DistributedInputIpcTest, GetDInputSinkProxy01, testing::ext::TestSize.Level1)
{
    bool ret = DInputSAManager::GetInstance().GetDInputSinkProxy();
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, CheckSourceRegisterCallback01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "46qweqwe46q5qw4e";
    BusinessEvent event;
    DistributedInputClient::GetInstance().CheckSourceRegisterCallback();
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, CheckSinkRegisterCallback01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "46qweqwe46q5qw4e";
    BusinessEvent event;
    DistributedInputClient::GetInstance().CheckSinkRegisterCallback();
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, CheckSharingDhIdsCallback01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "46qweqwe46q5qw4e";
    BusinessEvent event;
    DistributedInputClient::GetInstance().CheckSharingDhIdsCallback();
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, CheckSinkScreenInfoCallback01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "46qweqwe46q5qw4e";
    BusinessEvent event;
    DistributedInputClient::GetInstance().CheckSinkScreenInfoCallback();
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsNeedFilterOut01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "46qweqwe46q5qw4e";
    BusinessEvent event;
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsNeedFilterOut02, testing::ext::TestSize.Level1)
{
    std::string deviceId;
    BusinessEvent event;
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsNeedFilterOut03, testing::ext::TestSize.Level1)
{
    std::string deviceId(280, 'e');
    BusinessEvent event;
    bool ret = DistributedInputClient::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsTouchEventNeedFilterOut01, testing::ext::TestSize.Level1)
{
    TouchScreenEvent event = {100, 100};
    bool ret = DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsTouchEventNeedFilterOut02, testing::ext::TestSize.Level1)
{
    TouchScreenEvent event = {100, 100};
    nlohmann::json jsonObj;
    jsonObj = {{10, 10, 100, 100}};
    DistributedInputClient::GetInstance().UpdateSinkScreenInfos(jsonObj.dump());
    bool ret = DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputIpcTest, IsTouchEventNeedFilterOut03, testing::ext::TestSize.Level1)
{
    TouchScreenEvent event = {100, 100};
    nlohmann::json jsonObj;
    jsonObj = {{10, 10, 100, 80}};
    DistributedInputClient::GetInstance().UpdateSinkScreenInfos(jsonObj.dump());
    bool ret = DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsTouchEventNeedFilterOut04, testing::ext::TestSize.Level1)
{
    TouchScreenEvent event = {100, 100};
    nlohmann::json jsonObj;
    jsonObj = {{10, 10, 80, 100}};
    DistributedInputClient::GetInstance().UpdateSinkScreenInfos(jsonObj.dump());
    bool ret = DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsTouchEventNeedFilterOut05, testing::ext::TestSize.Level1)
{
    TouchScreenEvent event = {100, 100};
    nlohmann::json jsonObj;
    jsonObj = {{110, 10, 100, 100}};
    DistributedInputClient::GetInstance().UpdateSinkScreenInfos(jsonObj.dump());
    bool ret = DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsTouchEventNeedFilterOut06, testing::ext::TestSize.Level1)
{
    TouchScreenEvent event = {100, 100};
    nlohmann::json jsonObj;
    jsonObj = {{10, 110, 100, 100}};
    DistributedInputClient::GetInstance().UpdateSinkScreenInfos(jsonObj.dump());
    bool ret = DistributedInputClient::GetInstance().IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsStartDistributedInput01, testing::ext::TestSize.Level1)
{
    std::string dhId;
    bool ret = DistributedInputClient::GetInstance().IsStartDistributedInput(dhId);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsStartDistributedInput02, testing::ext::TestSize.Level1)
{
    std::string dhId = "654ew6qw4f6w1e6f1w6e5f";
    bool ret = DistributedInputClient::GetInstance().IsStartDistributedInput(dhId);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsStartDistributedInput03, testing::ext::TestSize.Level1)
{
    std::string dhId(280, 'e');
    bool ret = DistributedInputClient::GetInstance().IsStartDistributedInput(dhId);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, RegisterSimulationEventListener01, testing::ext::TestSize.Level1)
{
    sptr<TestSimulationEventListenerStub> listener = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().RegisterSimulationEventListener(listener);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_REG_UNREG_KEY_STATE_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, RegisterSimulationEventListener02, testing::ext::TestSize.Level1)
{
    sptr<TestSimulationEventListenerStub> listener(new TestSimulationEventListenerStub());
    int32_t ret = DistributedInputClient::GetInstance().RegisterSimulationEventListener(listener);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, UnregisterSimulationEventListener01, testing::ext::TestSize.Level1)
{
    sptr<TestSimulationEventListenerStub> listener = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().UnregisterSimulationEventListener(listener);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_REG_UNREG_KEY_STATE_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, UnregisterSimulationEventListener02, testing::ext::TestSize.Level1)
{
    sptr<TestSimulationEventListenerStub> listener(new TestSimulationEventListenerStub());
    int32_t ret = DistributedInputClient::GetInstance().UnregisterSimulationEventListener(listener);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, IsJsonData01, testing::ext::TestSize.Level1)
{
    std::string strData = "123";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, IsJsonData02, testing::ext::TestSize.Level1)
{
    std::string strData = "{{{}}}";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputIpcTest, IsJsonData03, testing::ext::TestSize.Level1)
{
    std::string strData = "{3413";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, AddWhiteListInfos01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "4646565465asdqweqwe";
    std::string strJson = "[[[2000][2000][1]]]";
    DistributedInputClient::GetInstance().AddWhiteListInfos(deviceId, strJson);
    std::string strData = "{3413";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, AddWhiteListInfos02, testing::ext::TestSize.Level1)
{
    std::string deviceId = "4646565465asdqweqwe";
    std::string strJson = "";
    DistributedInputClient::GetInstance().AddWhiteListInfos(deviceId, strJson);
    std::string strData = "{3413";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, DelWhiteListInfos01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "4646565465asdqweqwe";
    DistributedInputClient::GetInstance().DelWhiteListInfos(deviceId);
    std::string strData = "{3413";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, UpdateSinkScreenInfos01, testing::ext::TestSize.Level1)
{
    nlohmann::json jsonObj;
    jsonObj = {{1080, 720, 10, 10}};
    DistributedInputClient::GetInstance().UpdateSinkScreenInfos(jsonObj.dump());
    std::string strData = "{3413";
    bool ret = DistributedInputClient::GetInstance().IsJsonData(strData);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputIpcTest, NotifyStartDScreen01, testing::ext::TestSize.Level1)
{
    std::string sinkDevId = "46qw4e61dq6w1dq6w5e4q6";
    std::string srcDevId = "erq6w54e9q8w4eqw19q6d1";
    uint64_t srcWinId = 5;
    int32_t ret = DistributedInputClient::GetInstance().NotifyStartDScreen(sinkDevId, srcDevId, srcWinId);
    EXPECT_EQ(ERR_DH_INPUT_RPC_GET_REMOTE_DINPUT_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, NotifyStopDScreen01, testing::ext::TestSize.Level1)
{
    std::string networkId = "46qw4e61dq6w1dq6w5e4q6";
    std::string srcScreenInfoKey = "q65we46qw54e6q5we46q";
    int32_t ret = DistributedInputClient::GetInstance().NotifyStopDScreen(networkId, srcScreenInfoKey);
    EXPECT_EQ(ERR_DH_INPUT_RPC_GET_REMOTE_DINPUT_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, RegisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    std::string devId;
    std::string dhId;
    std::string parameters;
    std::shared_ptr<RegisterCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().RegisterDistributedHardware(devId, dhId, parameters, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, UnregisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    std::string devId;
    std::string dhId;
    std::shared_ptr<UnregisterCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, PrepareRemoteInput01, testing::ext::TestSize.Level1)
{
    string deviceId = "PrepareRemoteInput01";
    sptr<IPrepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, UnprepareRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "UnprepareRemoteInput01";
    sptr<IUnprepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().UnprepareRemoteInput(deviceId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StartRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "StartRemoteInput01";
    sptr<IStartDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(
        deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StartRemoteInput02, testing::ext::TestSize.Level0)
{
    std::string sinkId = "StartRemoteInput_sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<IStartStopDInputsCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StartRemoteInput03, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    sptr<IStartDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(
        srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StartRemoteInput04, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<IStartStopDInputsCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StopRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "StopRemoteInput01";
    sptr<IStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(
        deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StopRemoteInput02, testing::ext::TestSize.Level0)
{
    std::string sinkId = "StartRemoteInput_test";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<IStartStopDInputsCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StopRemoteInput03, testing::ext::TestSize.Level0)
{
    string srcId = "StopRemoteInput03-src";
    string sinkId = "StopRemoteInput03-sink";
    sptr<IStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(
        srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

HWTEST_F(DistributedInputIpcTest, StopRemoteInput04, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<IStartStopDInputsCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_GET_SOURCE_PROXY_FAIL, ret);
}

} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS