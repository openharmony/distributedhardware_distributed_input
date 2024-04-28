/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "distributed_input_inner_test.h"

#include "system_ability_definition.h"
#include "dinput_context.h"
#include "dinput_errcode.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
using namespace OHOS;

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputInnerTest::SetUp()
{
}

void DistributedInputInnerTest::TearDown()
{
}

void DistributedInputInnerTest::SetUpTestCase()
{
}

void DistributedInputInnerTest::TearDownTestCase()
{
}

void DistributedInputInnerTest::TestPrepareDInputCallback::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DistributedInputInnerTest::TestUnprepareDInputCallback::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DistributedInputInnerTest::TestStartDInputCallback::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DistributedInputInnerTest::TestStopDInputCallback::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DistributedInputInnerTest::TestStartStopDInputCallback::OnResultDhids(
    const std::string &devId, const int32_t &status)
{
    (void)devId;
    (void)status;
    return;
}

void DistributedInputInnerTest::TestInputNodeListener::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
    return;
}

void DistributedInputInnerTest::TestInputNodeListener::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    return;
}

int32_t DistributedInputInnerTest::TestSimulationEventListenerStub::OnSimulationEvent(
    uint32_t type, uint32_t code, int32_t value)
{
    (void)type;
    (void)code;
    (void)value;
    return DH_SUCCESS;
}

void DistributedInputInnerTest::TestRegisterSessionStateCb::OnResult(const std::string &devId, const uint32_t status)
{
    (void)devId;
    (void)status;
    return;
}

int DistributedInputInnerTest::CheckSourceProxy() const
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return DH_SUCCESS;
    }

    OHOS::sptr<OHOS::IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
        DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID);
    if (!remoteObject) {
        return DH_SUCCESS;
    }

    OHOS::sptr<IDistributedSourceInput> proxyTest;

    proxyTest = OHOS::iface_cast<IDistributedSourceInput>(remoteObject);
    if ((!proxyTest) || (!proxyTest->AsObject())) {
        return DH_SUCCESS;
    }

    return DH_SUCCESS;
}

int DistributedInputInnerTest::CheckSinkProxy() const
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return DH_SUCCESS;
    }

    OHOS::sptr<OHOS::IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
        DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID);
    if (!remoteObject) {
        return DH_SUCCESS;
    }

    OHOS::sptr<IDistributedSinkInput> proxyTest;

    proxyTest = OHOS::iface_cast<IDistributedSinkInput>(remoteObject);
    if ((!proxyTest) || (!proxyTest->AsObject())) {
        return DH_SUCCESS;
    }

    return DH_SUCCESS;
}

HWTEST_F(DistributedInputInnerTest, PrepareRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "PrepareRemoteInput01";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = DistributedInputKit::PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, PrepareRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestPrepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::PrepareRemoteInput(deviceId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, PrepareRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string srcId = "PrepareRemoteInput_test";
    std::string sinkId = "PrepareRemoteInput_test";
    sptr<IPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = DistributedInputKit::PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, PrepareRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestPrepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnprepareRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "UnprepareRemoteInput01";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = DistributedInputKit::UnprepareRemoteInput(deviceId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnprepareRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestUnprepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::UnprepareRemoteInput(deviceId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnprepareRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string srcId = "PrepareRemoteInput_src";
    std::string sinkId = "PrepareRemoteInput_sink";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = DistributedInputKit::UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnprepareRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestUnprepareDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "StartRemoteInput01";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret =
        DistributedInputKit::StartRemoteInput(deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestStartDInputCallback> callback = nullptr;
    int32_t ret =
        DistributedInputKit::StartRemoteInput(deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string sinkId = "StartRemoteInput_sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputKit::StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput05, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret =
        DistributedInputKit::StartRemoteInput(srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput06, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    sptr<TestStartDInputCallback> callback = nullptr;
    int32_t ret =
        DistributedInputKit::StartRemoteInput(srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput07, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputKit::StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StartRemoteInput08, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput01, testing::ext::TestSize.Level0)
{
    string deviceId = "StopRemoteInput01";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret =
        DistributedInputKit::StopRemoteInput(deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput02, testing::ext::TestSize.Level0)
{
    string deviceId = "";
    sptr<TestStopDInputCallback> callback = nullptr;
    int32_t ret =
        DistributedInputKit::StopRemoteInput(deviceId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput03, testing::ext::TestSize.Level0)
{
    std::string sinkId = "StartRemoteInput_test";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputKit::StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput04, testing::ext::TestSize.Level0)
{
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput05, testing::ext::TestSize.Level0)
{
    string srcId = "StopRemoteInput03-src";
    string sinkId = "StopRemoteInput03-sink";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret =
        DistributedInputKit::StopRemoteInput(srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput06, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    sptr<TestStopDInputCallback> callback = nullptr;
    int32_t ret =
        DistributedInputKit::StopRemoteInput(srcId, sinkId, static_cast<uint32_t>(DInputDeviceType::ALL), callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput07, testing::ext::TestSize.Level0)
{
    string srcId = "StartRemoteInput01-src";
    string sinkId = "StartRemoteInput01-sink";
    std::vector<std::string> dhIds = {"dhIds_test"};
    sptr<TestStartStopDInputCallback> callback(new TestStartStopDInputCallback());
    int32_t ret = DistributedInputKit::StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, StopRemoteInput08, testing::ext::TestSize.Level0)
{
    string srcId = "";
    string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopDInputCallback> callback = nullptr;
    int32_t ret = DistributedInputKit::StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, IsNeedFilterOut01, testing::ext::TestSize.Level0)
{
    string deviceId = "IsNeedFilterOut01";
    BusinessEvent event;
    event.pressedKeys.push_back(29);
    event.pressedKeys.push_back(56);
    event.keyCode = 111;
    event.keyAction = 108;
    bool ret = DistributedInputKit::IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputInnerTest, IsNeedFilterOut02, testing::ext::TestSize.Level0)
{
    string deviceId;
    BusinessEvent event;
    bool ret = DistributedInputKit::IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: IsTouchEventNeedFilterOut01
 * @tc.desc: verify the function of filtering events on the touchscreen.
 * @tc.type: FUNC
 * @tc.require: SR000GNECO
 */
HWTEST_F(DistributedInputInnerTest, IsTouchEventNeedFilterOut01, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "123";
    SinkScreenInfo sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(sourceWinId);
    const TransformInfo trans{10, 10, 100, 100, 1.0, 1.0};
    sinkScreenInfo.transformInfo = trans;
    DInputContext::GetInstance().UpdateSinkScreenInfo(sourceWinId, sinkScreenInfo);

    sourceWinId = "456";
    sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(sourceWinId);
    const TransformInfo trans1{120, 130, 50, 50, 1.0, 1.0};
    sinkScreenInfo.transformInfo = trans1;
    DInputContext::GetInstance().UpdateSinkScreenInfo(sourceWinId, sinkScreenInfo);

    TouchScreenEvent event;
    event.absX = 100;
    event.absY = 100;
    bool ret = DistributedInputKit::IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(true, ret);

    event.absX = 140;
    event.absY = 150;
    ret = DistributedInputKit::IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(true, ret);

    event.absX = 150;
    event.absY = 20;
    ret = DistributedInputKit::IsTouchEventNeedFilterOut(event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputInnerTest, IsStartDistributedInput01, testing::ext::TestSize.Level0)
{
    uint32_t flag = 1;
    DInputServerType retFlag = DistributedInputKit::IsStartDistributedInput(flag);
    EXPECT_EQ(DInputServerType::NULL_SERVER_TYPE, retFlag);
}

HWTEST_F(DistributedInputInnerTest, IsStartDistributedInput02, testing::ext::TestSize.Level0)
{
    std::string dhId = "IsStartDistributedInput02";
    bool ret = DistributedInputKit::IsStartDistributedInput(dhId);
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputInnerTest, IsStartDistributedInput03, testing::ext::TestSize.Level0)
{
    std::string dhId = "";
    bool ret = DistributedInputKit::IsStartDistributedInput(dhId);
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputInnerTest, RegisterSimulationEventListener01, testing::ext::TestSize.Level0)
{
    sptr<TestSimulationEventListenerStub> listener(new TestSimulationEventListenerStub());
    int32_t ret = DistributedInputKit::RegisterSimulationEventListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, RegisterSimulationEventListener02, testing::ext::TestSize.Level0)
{
    sptr<TestSimulationEventListenerStub> listener = nullptr;
    int32_t ret = DistributedInputKit::RegisterSimulationEventListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnregisterSimulationEventListener01, testing::ext::TestSize.Level0)
{
    sptr<TestSimulationEventListenerStub> listener(new TestSimulationEventListenerStub());
    int32_t ret = DistributedInputKit::UnregisterSimulationEventListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnregisterSimulationEventListener02, testing::ext::TestSize.Level0)
{
    sptr<TestSimulationEventListenerStub> listener = nullptr;
    int32_t ret = DistributedInputKit::UnregisterSimulationEventListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, RegisterSessionStateCb01, testing::ext::TestSize.Level0)
{
    sptr<TestRegisterSessionStateCb> listener(new TestRegisterSessionStateCb());
    int32_t ret = DistributedInputKit::RegisterSessionStateCb(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputInnerTest, UnregisterSessionStateCb01, testing::ext::TestSize.Level0)
{
    int32_t ret = DistributedInputKit::UnregisterSessionStateCb();
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS