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

#include "add_white_list_infos_call_back_test.h"

#include "accesstoken_kit.h"
#include "dinput_errcode.h"
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
void AddWhiteListInfosCallbackTest::SetUp()
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

void AddWhiteListInfosCallbackTest::TearDown()
{
}

void AddWhiteListInfosCallbackTest::SetUpTestCase()
{
}

void AddWhiteListInfosCallbackTest::TearDownTestCase()
{
}

void AddWhiteListInfosCallbackTest::TestAddWhiteListInfosCallbackStub::OnResult(const std::string &deviceId,
    const std::string &strJson)
{
    deviceId_ = deviceId;
    strJson_ = strJson;
}

void AddWhiteListInfosCallbackTest::TestGetSinkScreenInfosCallbackStub::OnResult(const std::string &strJson)
{
    strJson_ = strJson;
}

void AddWhiteListInfosCallbackTest::TestInputNodeListenerStub::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    srcDevId_ = srcDevId;
    sinkDevId_ = sinkDevId;
    sinkNodeId_ = sinkNodeId;
    sinkNodeDesc_ = sinkNodeDesc;
}

void AddWhiteListInfosCallbackTest::TestInputNodeListenerStub::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    srcDevId_ = srcDevId;
    sinkDevId_ = sinkDevId;
    sinkNodeId_ = sinkNodeId;
}

void AddWhiteListInfosCallbackTest::TestPrepareDInputCallbackStub::OnResult(const std::string &devId,
    const int32_t &status)
{
    devId_ = devId;
    status_ = status;
}

void AddWhiteListInfosCallbackTest::TestRegisterDInputCallbackStub::OnResult(const std::string &devId,
    const std::string &dhId, const int32_t &status)
{
    devId_ = devId;
    dhId_ = dhId;
    status_ = status;
}

int32_t AddWhiteListInfosCallbackTest::TestSharingDhIdListenerStub::OnSharing(const std::string &dhId)
{
    dhId_ = dhId;
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestSharingDhIdListenerStub::OnNoSharing(const std::string &dhId)
{
    dhId_ = dhId;
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestSimulationEventListenerStub::OnSimulationEvent(uint32_t type,
    uint32_t code, int32_t value)
{
    type_ = type;
    code_ = code;
    value_ = value;
    return DH_SUCCESS;
}
void AddWhiteListInfosCallbackTest::TestStartDInputCallbackStub::OnResult(const std::string &devId,
    const uint32_t &inputTypes, const int32_t &status)
{
    devId_ = devId;
    inputTypes_ = inputTypes;
    status_ = status;
}

void AddWhiteListInfosCallbackTest::TestStartStopDInputsCallbackStub::OnResultDhids(const std::string &devId,
    const int32_t &status)
{
    devId_ = devId;
    status_ = status;
}

void AddWhiteListInfosCallbackTest::TestStartStopResultCallbackStub::OnStart(const std::string &srcId,
    const std::string &sinkId, std::vector<std::string> &dhIds)
{
    srcId_ = srcId;
    sinkId_ = sinkId;
    dhIds.swap(dhIds);
}

void AddWhiteListInfosCallbackTest::TestStartStopResultCallbackStub::OnStop(const std::string &srcId,
    const std::string &sinkId, std::vector<std::string> &dhIds)
{
    srcId_ = srcId;
    sinkId_ = sinkId;
    dhIds.swap(dhIds);
}

void AddWhiteListInfosCallbackTest::TestStopDInputCallbackStub::OnResult(const std::string &devId,
    const uint32_t &inputTypes, const int32_t &status)
{
    devId_ = devId;
    inputTypes_ = inputTypes;
    status_ = status;
}

int32_t AddWhiteListInfosCallbackTest::TestDistributedInputSinkStub::Init()
{
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestDistributedInputSinkStub::Release()
{
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestDistributedInputSinkStub::RegisterGetSinkScreenInfosCallback(
    sptr<IGetSinkScreenInfosCallback> callback)
{
    (void)callback;
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestDistributedInputSinkStub::NotifyStartDScreen(
    const SrcScreenInfo &remoteCtrlInfo)
{
    (void)remoteCtrlInfo;
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestDistributedInputSinkStub::NotifyStopDScreen(
    const std::string &srcScreenInfoKey)
{
    (void)srcScreenInfoKey;
    return DH_SUCCESS;
}

int32_t AddWhiteListInfosCallbackTest::TestDistributedInputSinkStub::RegisterSharingDhIdListener(
    sptr<ISharingDhIdListener> sharingDhIdListener)
{
    (void)sharingDhIdListener;
    return DH_SUCCESS;
}

HWTEST_F(AddWhiteListInfosCallbackTest, AddWhiteListInfosCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestAddWhiteListInfosCallbackStub());
    AddWhiteListInfosCallbackProxy callBackProxy(callBackStubPtr);
    std::string deviceId = "deviceId0";
    std::string json = "json0";
    callBackProxy.OnResult(deviceId, json);
    EXPECT_STREQ(deviceId.c_str(), ((sptr<TestAddWhiteListInfosCallbackStub> &)callBackStubPtr)->deviceId_.c_str());
    EXPECT_STREQ(json.c_str(), ((sptr<TestAddWhiteListInfosCallbackStub> &)callBackStubPtr)->strJson_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, GetSinkScreenInfosCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestGetSinkScreenInfosCallbackStub());
    GetSinkScreenInfosCallbackProxy callBackProxy(callBackStubPtr);
    std::string json = "json0";
    callBackProxy.OnResult(json);
    EXPECT_STREQ(json.c_str(), ((sptr<TestGetSinkScreenInfosCallbackStub> &)callBackStubPtr)->strJson_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, InputNodeListenerStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> nodeListener(new TestInputNodeListenerStub());
    InputNodeListenerProxy nodeListenerProxy(nodeListener);
    std::string srcDevId = "srcDevId_test";
    std::string sinkDevId = "sinkDevId_test";
    std::string sinkNodeId = "sinkNodeId_test";
    std::string sinkNodeDesc = "sinkNodeDesc_test";
    nodeListenerProxy.OnNodeOnLine(srcDevId, sinkDevId, sinkNodeId, sinkNodeDesc);
    EXPECT_STREQ(srcDevId.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->srcDevId_.c_str());
    EXPECT_STREQ(sinkDevId.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->sinkDevId_.c_str());
    EXPECT_STREQ(sinkNodeId.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->sinkNodeId_.c_str());
    EXPECT_STREQ(sinkNodeDesc.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->sinkNodeDesc_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, InputNodeListenerStub02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> nodeListener(new TestInputNodeListenerStub());
    InputNodeListenerProxy nodeListenerProxy(nodeListener);
    std::string srcDevId = "srcDevId_test";
    std::string sinkDevId = "sinkDevId_test";
    std::string sinkNodeId = "sinkNodeId_test";
    nodeListenerProxy.OnNodeOffLine(srcDevId, sinkDevId, sinkNodeId);
    EXPECT_STREQ(srcDevId.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->srcDevId_.c_str());
    EXPECT_STREQ(sinkDevId.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->sinkDevId_.c_str());
    EXPECT_STREQ(sinkNodeId.c_str(), ((sptr<TestInputNodeListenerStub> &)nodeListener)->sinkNodeId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, PrepareDInputCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestPrepareDInputCallbackStub());
    PrepareDInputCallbackProxy callBackProxy(callBackStubPtr);
    std::string devId = "devId_test";
    int32_t status = 0;
    callBackProxy.OnResult(devId, status);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestPrepareDInputCallbackStub> &)callBackStubPtr)->devId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, RegisterDInputCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestRegisterDInputCallbackStub());
    RegisterDInputCallbackProxy callBackProxy(callBackStubPtr);
    std::string devId = "devId_test";
    std::string dhId = "dhId_test";
    int32_t status = 0;
    callBackProxy.OnResult(devId, dhId, status);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestRegisterDInputCallbackStub> &)callBackStubPtr)->devId_.c_str());
    EXPECT_STREQ(dhId.c_str(), ((sptr<TestRegisterDInputCallbackStub> &)callBackStubPtr)->dhId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, SharingDhIdListenerStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> sharingStubPtr(new TestSharingDhIdListenerStub());
    SharingDhIdListenerProxy sharingProxy(sharingStubPtr);
    std::string dhId = "dhId_test";
    int32_t ret = sharingProxy.OnSharing(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(AddWhiteListInfosCallbackTest, SharingDhIdListenerStub02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> sharingStubPtr(new TestSharingDhIdListenerStub());
    SharingDhIdListenerProxy sharingProxy(sharingStubPtr);
    std::string dhId = "dhId_test";
    int32_t ret = sharingProxy.OnNoSharing(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(AddWhiteListInfosCallbackTest, SimulationEventListenerStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> simulationStubPtr(new TestSimulationEventListenerStub());
    SimulationEventListenerProxy sharingProxy(simulationStubPtr);
    uint32_t type = 1;
    uint32_t code = 1;
    int32_t value = 1;
    int32_t ret = sharingProxy.OnSimulationEvent(type, code, value);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(AddWhiteListInfosCallbackTest, StartDInputCallbackStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestStartDInputCallbackStub());
    StartDInputCallbackProxy callBackProxy(callBackStubPtr);
    std::string devId = "devId_test";
    uint32_t inputTypes = static_cast<uint32_t>(DInputDeviceType::ALL);
    int32_t status = 1;
    callBackProxy.OnResult(devId, inputTypes, status);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestStartDInputCallbackStub> &)callBackStubPtr)->devId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, StartStopDInputsCallbackStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestStartStopDInputsCallbackStub());
    StartStopDInputsCallbackProxy callBackProxy(callBackStubPtr);
    std::string devId = "devId_test";
    int32_t status = 1;
    callBackProxy.OnResultDhids(devId, status);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestStartStopDInputsCallbackStub> &)callBackStubPtr)->devId_.c_str());
    EXPECT_EQ(status, ((sptr<TestStartStopDInputsCallbackStub> &)callBackStubPtr)->status_);
}

HWTEST_F(AddWhiteListInfosCallbackTest, StartStopResultCallbackStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestStartStopResultCallbackStub());
    StartStopResultCallbackProxy callBackProxy(callBackStubPtr);
    std::string srcId = "srcId_test";
    std::string sinkId = "sinkId_test";
    std::vector<std::string> dhIds;
    dhIds.push_back("dhId_test");
    callBackProxy.OnStart(srcId, sinkId, dhIds);
    EXPECT_STREQ(srcId.c_str(), ((sptr<TestStartStopResultCallbackStub> &)callBackStubPtr)->srcId_.c_str());
    EXPECT_STREQ(sinkId.c_str(), ((sptr<TestStartStopResultCallbackStub> &)callBackStubPtr)->sinkId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, StartStopResultCallbackStub02, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestStartStopResultCallbackStub());
    StartStopResultCallbackProxy callBackProxy(callBackStubPtr);
    std::string srcId = "srcId_test";
    std::string sinkId = "sinkId_test";
    std::vector<std::string> dhIds;
    dhIds.push_back("dhId_test");
    callBackProxy.OnStop(srcId, sinkId, dhIds);
    EXPECT_STREQ(srcId.c_str(), ((sptr<TestStartStopResultCallbackStub> &)callBackStubPtr)->srcId_.c_str());
    EXPECT_STREQ(sinkId.c_str(), ((sptr<TestStartStopResultCallbackStub> &)callBackStubPtr)->sinkId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, StopDInputCallbackStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestStopDInputCallbackStub());
    StopDInputCallbackProxy callBackProxy(callBackStubPtr);
    std::string devId = "devId_test";
    uint32_t inputTypes = static_cast<uint32_t>(DInputDeviceType::ALL);
    int32_t status = 1;
    callBackProxy.OnResult(devId, inputTypes, status);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestStopDInputCallbackStub> &)callBackStubPtr)->devId_.c_str());
}

HWTEST_F(AddWhiteListInfosCallbackTest, DistributedInputSinkStub01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr(new TestDistributedInputSinkStub());
    DistributedInputSinkProxy sinkProxy(sinkStubPtr);
    int32_t ret = sinkProxy.Init();
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sinkProxy.Release();
    EXPECT_EQ(DH_SUCCESS, ret);

    sptr<IGetSinkScreenInfosCallback> callback = nullptr;
    ret = sinkProxy.RegisterGetSinkScreenInfosCallback(callback);
    EXPECT_EQ(ERR_DH_INPUT_IPC_WRITE_TOKEN_VALID_FAIL, ret);

    sptr<IGetSinkScreenInfosCallback> sinkScreenCb(new TestGetSinkScreenInfosCallbackStub());
    ret = sinkProxy.RegisterGetSinkScreenInfosCallback(sinkScreenCb);
    EXPECT_EQ(DH_SUCCESS, ret);

    std::string srcScreenInfoKey = "srcScreenInfoKey_test";
    ret = sinkProxy.NotifyStopDScreen(srcScreenInfoKey);
    EXPECT_EQ(DH_SUCCESS, ret);

    sptr<ISharingDhIdListener> sharingDhIdListene(new TestSharingDhIdListenerStub());
    ret = sinkProxy.RegisterSharingDhIdListener(sharingDhIdListene);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS