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

#include "dinput_source_callback_unittest.h"

#include <memory>

#include "accesstoken_kit.h"
#include "dinput_errcode.h"
#include "dinput_ipc_interface_code.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "softbus_common.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    const int32_t PERMISSION_ENABLE_INDEX = 0;
    const int32_t PERMISSION_DATASYNC_INDEX = 1;
    const int32_t PERMISSION_ACCESS_INDEX = 2;
    const int32_t PERMISSION_NUMS = 3;
}
void DInputSourceCallBackTest::SetUp()
{
    uint64_t tokenId;
    const char *perms[PERMISSION_NUMS];
    perms[PERMISSION_ENABLE_INDEX] = "ohos.permission.ENABLE_DISTRIBUTED_HARDWARE";
    perms[PERMISSION_DATASYNC_INDEX] = OHOS_PERMISSION_DISTRIBUTED_DATASYNC;
    perms[PERMISSION_ACCESS_INDEX] = "ohos.permission.ACCESS_DISTRIBUTED_HARDWARE";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = PERMISSION_NUMS,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "dinput",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DInputSourceCallBackTest::TearDown()
{
}

void DInputSourceCallBackTest::SetUpTestCase()
{
}

void DInputSourceCallBackTest::TearDownTestCase()
{
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::Init()
{
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::Release()
{
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::RegisterDistributedHardware(
    const std::string &devId, const std::string &dhId, const std::string &parameters,
    sptr<IRegisterDInputCallback> callback)
{
    (void)devId;
    (void)dhId;
    (void)parameters;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::UnregisterDistributedHardware(
    const std::string &devId, const std::string &dhId,
    sptr<IUnregisterDInputCallback> callback)
{
    (void)devId;
    (void)dhId;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::PrepareRemoteInput(
    const std::string &deviceId, sptr<IPrepareDInputCallback> callback)
{
    (void)deviceId;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::UnprepareRemoteInput(
    const std::string &deviceId, sptr<IUnprepareDInputCallback> callback)
{
    (void)deviceId;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StartRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    (void)deviceId;
    (void)inputTypes;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StopRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    (void)deviceId;
    (void)inputTypes;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StartRemoteInput(
    const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
    sptr<IStartDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)inputTypes;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StopRemoteInput(
    const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
    sptr<IStopDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)inputTypes;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::PrepareRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    sptr<IPrepareDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::UnprepareRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    sptr<IUnprepareDInputCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StartRemoteInput(
    const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StopRemoteInput(
    const std::string &sinkId, const std::vector<std::string> &dhIds,
    sptr<IStartStopDInputsCallback> callback)
{
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StartRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::StopRemoteInput(
    const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    (void)srcId;
    (void)sinkId;
    (void)dhIds;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::RegisterAddWhiteListCallback(
    sptr<IAddWhiteListInfosCallback> addWhiteListCallback)
{
    (void)addWhiteListCallback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::RegisterDelWhiteListCallback(
    sptr<IDelWhiteListInfosCallback> delWhiteListCallback)
{
    (void)delWhiteListCallback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::RegisterSimulationEventListener(
    sptr<ISimulationEventListener> listener)
{
    (void)listener;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::UnregisterSimulationEventListener(
    sptr<ISimulationEventListener> listener)
{
    (void)listener;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::RegisterSessionStateCb(
    sptr<ISessionStateCallback> callback)
{
    (void)callback;
    return DH_SUCCESS;
}

int32_t DInputSourceCallBackTest::TestDInputSourceCallBackStub::UnregisterSessionStateCb()
{
    return DH_SUCCESS;
}

void DInputSourceCallBackTest::TestDInputSourceCallBackStub::OnResult(const std::string &deviceId,
    const std::string &strJson)
{
    deviceId_ = deviceId;
    strJson_ = strJson;
}

void DInputSourceCallBackTest::TestDInputRegisterCallBack::OnResult(const std::string &devId,
    const std::string &dhId, const int32_t &status)
{
    (void)devId;
    (void)dhId;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestDInputUnregisterCallBack::OnResult(const std::string &devId,
    const std::string &dhId, const int32_t &status)
{
    (void)devId;
    (void)dhId;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestDInputPrepareCallBack::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestDInputUnprepareCallBack::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestDInputStartCallBack::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestDInputStopCallBack::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestVectorStartStopCallBackStub::OnResultDhids(const std::string &devId,
    const int32_t &status)
{
    (void)devId;
    (void)status;
    return;
}

void DInputSourceCallBackTest::TestAddWhiteListInfosCallBack::OnResult(
    const std::string &deviceId, const std::string &strJson)
{
    (void)deviceId;
    (void)strJson;
    return;
}

void DInputSourceCallBackTest::TestDelWhiteListInfosCallBack::OnResult(
    const std::string &deviceId)
{
    (void)deviceId;
    return;
}

void DInputSourceCallBackTest::TestDInputNodeListenerCallBack::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
    return;
}

void DInputSourceCallBackTest::TestDInputNodeListenerCallBack::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    return;
}

int32_t DInputSourceCallBackTest::TestDInputSimulationEventCallBack::OnSimulationEvent(uint32_t type, uint32_t code,
    int32_t value)
{
    (void)type;
    (void)code;
    (void)value;
    return DH_SUCCESS;
}

HWTEST_F(DInputSourceCallBackTest, SendRequest01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    IDInputSourceInterfaceCode code = IDInputSourceInterfaceCode::INIT;
    MessageParcel data;
    MessageParcel reply;
    bool ret = callBackProxy.SendRequest(static_cast<uint32_t>(code), data, reply);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DInputSourceCallBackTest, Init01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    int32_t ret = callBackProxy.Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, Release01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    int32_t ret = callBackProxy.Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, RegisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string devId = "d6f4s6d4f6";
    const std::string dhId = "Input_sd4f4s5d4f5s4";
    const std::string parameters = "d4a6s5d46asd";
    sptr<IRegisterDInputCallback> callback(new TestDInputRegisterCallBack());
    int32_t ret = callBackProxy.RegisterDistributedHardware(devId, dhId, parameters, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, UnregisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string devId = "d6f4s6d4f6";
    const std::string dhId = "Input_sd4f4s5d4f5s4";
    sptr<TestDInputUnregisterCallBack> callback(new TestDInputUnregisterCallBack());
    int32_t ret = callBackProxy.UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, PrepareRemoteInput01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string devId = "d6f4s6d4f6";
    sptr<TestDInputPrepareCallBack> callback(new TestDInputPrepareCallBack());
    int32_t ret = callBackProxy.PrepareRemoteInput(devId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, PrepareRemoteInput02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string srcId = "d6f4s6d4f6";
    const std::string sinkId = "5sd45s4d5s4d5";
    sptr<TestDInputPrepareCallBack> callback(new TestDInputPrepareCallBack());
    int32_t ret = callBackProxy.PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, UnprepareRemoteInput01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string devId = "d6f4s6d4f6";
    sptr<TestDInputUnprepareCallBack> callback(new TestDInputUnprepareCallBack());
    int32_t ret = callBackProxy.UnprepareRemoteInput(devId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, UnprepareRemoteInput02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string srcId = "d6f4s6d4f6";
    const std::string sinkId = "5sd45s4d5s4d5";
    sptr<TestDInputUnprepareCallBack> callback(new TestDInputUnprepareCallBack());
    int32_t ret = callBackProxy.UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StartRemoteInput01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string devId = "d6f4s6d4f6";
    uint32_t inputTypes = 1;
    sptr<TestDInputStartCallBack> callback(new TestDInputStartCallBack());
    int32_t ret = callBackProxy.StartRemoteInput(devId, inputTypes, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StartRemoteInput02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string srcId = "d6f4s6d4f6";
    const std::string sinkId = "5sd45s4d5s4d5";
    uint32_t inputTypes = 1;
    sptr<TestDInputStartCallBack> callback(new TestDInputStartCallBack());
    int32_t ret = callBackProxy.StartRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StartRemoteInput03, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string sinkId = "d6f4s6d4f6";
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_6ds54f6sd4f65sd4fsdf4s");
    sptr<TestVectorStartStopCallBackStub> callback(new TestVectorStartStopCallBackStub());
    int32_t ret = callBackProxy.StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StartRemoteInput04, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string srcId = "d6f4s6d4f6";
    const std::string sinkId = "5sd45s4d5s4d5";
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_6ds54f6sd4f65sd4fsdf4s");
    sptr<TestVectorStartStopCallBackStub> callback(new TestVectorStartStopCallBackStub());
    int32_t ret = callBackProxy.StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StopRemoteInput01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string devId = "d6f4s6d4f6";
    uint32_t inputTypes = 1;
    sptr<TestDInputStopCallBack> callback(new TestDInputStopCallBack());
    int32_t ret = callBackProxy.StopRemoteInput(devId, inputTypes, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StopRemoteInput02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string srcId = "d6f4s6d4f6";
    const std::string sinkId = "5sd45s4d5s4d5";
    uint32_t inputTypes = 1;
    sptr<TestDInputStopCallBack> callback(new TestDInputStopCallBack());
    int32_t ret = callBackProxy.StopRemoteInput(srcId, sinkId, inputTypes, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StopRemoteInput03, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string sinkId = "d6f4s6d4f6";
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_6ds54f6sd4f65sd4fsdf4s");
    sptr<TestVectorStartStopCallBackStub> callback(new TestVectorStartStopCallBackStub());
    int32_t ret = callBackProxy.StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, StopRemoteInput04, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    const std::string srcId = "d6f4s6d4f6";
    const std::string sinkId = "5sd45s4d5s4d5";
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_6ds54f6sd4f65sd4fsdf4s");
    sptr<TestVectorStartStopCallBackStub> callback(new TestVectorStartStopCallBackStub());
    int32_t ret = callBackProxy.StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, RegisterAddWhiteListCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    sptr<TestAddWhiteListInfosCallBack> callback(new TestAddWhiteListInfosCallBack());
    int32_t ret = callBackProxy.RegisterAddWhiteListCallback(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, RegisterDelWhiteListCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    sptr<TestDelWhiteListInfosCallBack> callback(new TestDelWhiteListInfosCallBack());
    int32_t ret = callBackProxy.RegisterDelWhiteListCallback(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, RegisterSimulationEventListener01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    sptr<TestDInputSimulationEventCallBack> listener(new TestDInputSimulationEventCallBack());
    int32_t ret = callBackProxy.RegisterSimulationEventListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputSourceCallBackTest, UnregisterSimulationEventListener01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDInputSourceCallBackStub());
    DistributedInputSourceProxy callBackProxy(callBackStubPtr);
    sptr<TestDInputSimulationEventCallBack> listener(new TestDInputSimulationEventCallBack());
    int32_t ret = callBackProxy.UnregisterSimulationEventListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS