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

#include "distributed_input_client_test.h"

#include "nlohmann/json.hpp"

#include "dinput_errcode.h"
#include "add_white_list_infos_call_back_proxy.h"
#include "add_white_list_infos_call_back_stub.h"
#include "distributed_input_inject.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputClientTest::SetUp()
{
}

void DistributedInputClientTest::TearDown()
{
}

void DistributedInputClientTest::SetUpTestCase()
{
}

void DistributedInputClientTest::TearDownTestCase()
{
}

int32_t DistributedInputClientTest::StructTransJson(const InputDevice &pBuf, std::string &strDescriptor) const
{
    nlohmann::json tmpJson;
    tmpJson["name"] = pBuf.name;
    tmpJson["physicalPath"] = pBuf.physicalPath;
    tmpJson["uniqueId"] = pBuf.uniqueId;
    tmpJson["bus"] = pBuf.bus;
    tmpJson["vendor"] = pBuf.vendor;
    tmpJson["product"] = pBuf.product;
    tmpJson["version"] = pBuf.version;
    tmpJson["descriptor"] = pBuf.descriptor;
    tmpJson["classes"] = pBuf.classes;

    std::ostringstream stream;
    stream << tmpJson.dump();
    strDescriptor = stream.str();
    return DH_SUCCESS;
}

int32_t DistributedInputClientTest::TestRegisterDInputCallback::OnRegisterResult(const std::string &devId,
    const std::string &dhId, int32_t status, const std::string &data)
{
    (void)dhId;
    (void)status;
    (void)data;
    return DH_SUCCESS;
}

int32_t DistributedInputClientTest::TestUnregisterDInputCallback::OnUnregisterResult(const std::string &devId,
    const std::string &dhId, int32_t status, const std::string &data)
{
    (void)dhId;
    (void)status;
    (void)data;
    return DH_SUCCESS;
}

void DistributedInputClientTest::TestPrepareDInputCallback::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DistributedInputClientTest::TestUnprepareDInputCallback::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DistributedInputClientTest::TestStartDInputCallback::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DistributedInputClientTest::TestStopDInputCallback::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DistributedInputClientTest::TestStartStopDInputCallback::OnResultDhids(
    const std::string &devId, const int32_t &status)
{
    (void)devId;
    (void)status;
    return;
}

void DistributedInputClientTest::TestInputNodeListener::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
    return;
}

void DistributedInputClientTest::TestInputNodeListener::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    return;
}

int32_t DistributedInputClientTest::TestSimulationEventListenerStub::OnSimulationEvent(
    uint32_t type, uint32_t code, int32_t value)
{
    (void)type;
    (void)code;
    (void)value;
    return DH_SUCCESS;
}

HWTEST_F(DistributedInputClientTest, InitSource01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputClient::GetInstance().InitSource();
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_INIT_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, InitSink01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputClient::GetInstance().InitSink();
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SINK_MANAGER_INIT_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, RegisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    std::string devId;
    std::string dhId;
    std::string parameters;
    std::shared_ptr<RegisterCallback> callback;
    int32_t ret = DistributedInputClient::GetInstance().RegisterDistributedHardware(devId, dhId, parameters, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_REGISTER_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, UnregisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    std::string devId;
    std::string dhId;
    std::shared_ptr<UnregisterCallback> callback;
    int32_t ret = DistributedInputClient::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_UNREGISTER_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, UnregisterDistributedHardware02, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestUnregisterDInputCallback> unregisterDInputCallback =
        std::make_shared<TestUnregisterDInputCallback>();
    int32_t ret = DistributedInputClient::GetInstance().UnregisterDistributedHardware("devId",
        "dhId", unregisterDInputCallback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, PrepareRemoteInput01, testing::ext::TestSize.Level1)
{
    string deviceId = "PrepareRemoteInput01";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, PrepareRemoteInput02, testing::ext::TestSize.Level1)
{
    string deviceId = "";
    sptr<TestPrepareDInputCallback> callback = nullptr;
    std::shared_ptr<TestUnregisterDInputCallback> unregisterDInputCallback =
        std::make_shared<TestUnregisterDInputCallback>();
    int32_t ret = DistributedInputClient::GetInstance().PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_PREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, PrepareRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string srcId = "PrepareRemoteInput_test";
    std::string sinkId = "PrepareRemoteInput_test";
    sptr<IPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, PrepareRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestPrepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_PREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, UnprepareRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "UnprepareRemoteInput01";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().UnprepareRemoteInput(deviceId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, UnprepareRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestUnprepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().UnprepareRemoteInput(deviceId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_UNPREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, UnprepareRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string srcId = "PrepareRemoteInput_src";
    std::string sinkId = "PrepareRemoteInput_sink";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, UnprepareRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestUnprepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_UNPREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "StartRemoteInput01";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(
        deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestStartDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(
        deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string sinkId = "StartRemoteInput_sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput05, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(
        srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput06, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    sptr<TestStartDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(
        srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput07, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StartRemoteInput08, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_START_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "StopRemoteInput01";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(
        deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(
        deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string sinkId = "StartRemoteInput_test";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput05, testing::ext::TestSize.Level0)
{
    string srcId = "StopRemoteInput03-src";
    string sinkId = "StopRemoteInput03-sink";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(
        srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput06, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    sptr<TestStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(
        srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput07, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, StopRemoteInput08, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t status = 0;
    DistributedInputClient::RegisterDInputCb regCb;
    regCb.OnResult(srcId, sinkId, status);
    DistributedInputClient::UnregisterDInputCb unregCb;
    unregCb.OnResult(srcId, sinkId, status);
    DistributedInputClient::AddWhiteListInfosCb addCb;
    addCb.OnResult(srcId, sinkId);
    DistributedInputClient::DelWhiteListInfosCb delCb;
    delCb.OnResult(srcId);
    DistributedInputClient::GetSinkScreenInfosCb getSinkCb;
    getSinkCb.OnResult(srcId);
    DistributedInputClient::SharingDhIdListenerCb sharingCb;
    sharingCb.OnSharing(srcId);
    sharingCb.OnNoSharing(srcId);

    int32_t ret = DistributedInputClient::GetInstance().StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_CLIENT_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputClientTest, ProcessEvent01, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    DistributedInputClient::DInputClientEventHandler eventHandler(runner);
    AppExecFwk::InnerEvent::Pointer event =
                AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CHECK_SOURCE_CALLBACK_REGISTER_MSG, 4809);
    eventHandler.ProcessEvent(event);
    EXPECT_EQ(event->GetInnerEventId(), DINPUT_CLIENT_CHECK_SOURCE_CALLBACK_REGISTER_MSG);
}

HWTEST_F(DistributedInputClientTest, ProcessEvent02, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    DistributedInputClient::DInputClientEventHandler eventHandler(runner);
    AppExecFwk::InnerEvent::Pointer event =
                AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CHECK_SINK_CALLBACK_REGISTER_MSG, 4809);
    eventHandler.ProcessEvent(event);
    EXPECT_EQ(event->GetInnerEventId(), DINPUT_CLIENT_CHECK_SINK_CALLBACK_REGISTER_MSG);
}

HWTEST_F(DistributedInputClientTest, ProcessEvent03, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    DistributedInputClient::DInputClientEventHandler eventHandler(runner);
    AppExecFwk::InnerEvent::Pointer event =
                AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CLEAR_SOURCE_CALLBACK_REGISTER_MSG, 4809);
    eventHandler.ProcessEvent(event);
    EXPECT_EQ(event->GetInnerEventId(), DINPUT_CLIENT_CLEAR_SOURCE_CALLBACK_REGISTER_MSG);
}

HWTEST_F(DistributedInputClientTest, ProcessEvent04, testing::ext::TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    DistributedInputClient::DInputClientEventHandler eventHandler(runner);
    AppExecFwk::InnerEvent::Pointer event =
                AppExecFwk::InnerEvent::Get(DINPUT_CLIENT_CLEAR_SINK_CALLBACK_REGISTER_MSG, 4809);
    eventHandler.ProcessEvent(event);
    EXPECT_EQ(event->GetInnerEventId(), DINPUT_CLIENT_CLEAR_SINK_CALLBACK_REGISTER_MSG);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS