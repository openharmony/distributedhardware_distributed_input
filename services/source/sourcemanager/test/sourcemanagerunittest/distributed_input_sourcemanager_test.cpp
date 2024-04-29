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

#include "distributed_input_sourcemanager_test.h"

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <linux/input.h>

#include "event_handler.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "system_ability_definition.h"

#include "dinput_errcode.h"
#include "distributed_input_inject.h"
#include "distributed_input_source_transport.h"
#include "distributed_input_transport_base.h"
#include "dinput_utils_tool.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    const uint32_t INPUTTYPE = static_cast<uint32_t>(DInputDeviceType::ALL);
    const uint32_t INPUTTYPE_MOUSE = static_cast<uint32_t>(DInputDeviceType::MOUSE);
}

void DistributedInputSourceManagerTest::SetUp()
{
    sourceManager_ = new DistributedInputSourceManager(DISTRIBUTED_HARDWARE_INPUT_SOURCE_SA_ID, true);
    statuslistener_ = std::make_shared<DInputSourceListener>(sourceManager_);
    DistributedInputSourceManagerTest::RegisterSourceRespCallback(statuslistener_);
}

void DistributedInputSourceManagerTest::TearDown()
{
}

void DistributedInputSourceManagerTest::SetUpTestCase()
{
}

void DistributedInputSourceManagerTest::TearDownTestCase()
{
}

void DistributedInputSourceManagerTest::RegisterSourceRespCallback(std::shared_ptr<DInputSourceTransCallback> callback)
{
    callback_ = callback;
    return;
}

void DistributedInputSourceManagerTest::TestRegisterDInputCb::OnResult(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    (void)devId;
    (void)dhId;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestUnregisterDInputCb::OnResult(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    (void)devId;
    (void)dhId;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestPrepareDInputCallback::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestUnprepareDInputCallback::OnResult(
    const std::string &deviceId, const int32_t &status)
{
    (void)deviceId;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestStartDInputCallback::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestStopDInputCallback::OnResult(
    const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status)
{
    (void)deviceId;
    (void)inputTypes;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestStartStopVectorCallbackStub::OnResultDhids(const std::string &devId,
    const int32_t &status)
{
    (void)devId;
    (void)status;
    return;
}

void DistributedInputSourceManagerTest::TestAddWhiteListInfosCb::OnResult(
    const std::string &deviceId, const std::string &strJson)
{
    (void)deviceId;
    (void)strJson;
    return;
}

void DistributedInputSourceManagerTest::TestDelWhiteListInfosCb::OnResult(
    const std::string &deviceId)
{
    (void)deviceId;
    return;
}

void DistributedInputSourceManagerTest::TestStartStopResultCb::OnStart(const std::string &srcId,
    const std::string &sinkId, std::vector<std::string> &devData)
{
    (void)srcId;
    (void)sinkId;
    (void)devData;
    return;
}

void DistributedInputSourceManagerTest::TestStartStopResultCb::OnStop(const std::string &srcId,
    const std::string &sinkId, std::vector<std::string> &devData)
{
    (void)srcId;
    (void)sinkId;
    (void)devData;
    return;
}

void DistributedInputSourceManagerTest::TestInputNodeListenerCb::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
    return;
}

void DistributedInputSourceManagerTest::TestInputNodeListenerCb::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    return;
}

int32_t DistributedInputSourceManagerTest::TestSimulationEventCb::OnSimulationEvent(uint32_t type, uint32_t code,
    int32_t value)
{
    (void)type;
    (void)code;
    (void)value;
    return DH_SUCCESS;
}

void DistributedInputSourceManagerTest::TestStartStopDInputsCb::OnResultDhids(const std::string &devId,
    const int32_t &status)
{
    (void)devId;
    (void)status;
    return;
}

int32_t DistributedInputSourceManagerTest::StructTransJson(const InputDevice &pBuf, std::string &strDescriptor) const
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

HWTEST_F(DistributedInputSourceManagerTest, OnStart_01, testing::ext::TestSize.Level1)
{
    sourceManager_->serviceRunningState_ = ServiceSourceRunningState::STATE_RUNNING;
    sourceManager_->OnStart();
    EXPECT_EQ(0, sourceManager_->inputDevice_.size());

    sourceManager_->serviceRunningState_ = ServiceSourceRunningState::STATE_NOT_START;
    sourceManager_->OnStart();
    EXPECT_EQ(0, sourceManager_->inputDevice_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, Init01, testing::ext::TestSize.Level0)
{
    int32_t ret = sourceManager_->Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, CheckRegisterParam_01, testing::ext::TestSize.Level1)
{
    sourceManager_->UnregisterDHFwkPublisher();

    sourceManager_->startDScreenListener_ = nullptr;
    sourceManager_->stopDScreenListener_ = nullptr;
    sourceManager_->deviceOfflineListener_ = nullptr;
    sourceManager_->UnregisterDHFwkPublisher();

    std::string devId = "";
    std::string dhId = "";
    std::string parameters = "";
    sptr<IRegisterDInputCallback> callback = nullptr;
    bool ret = sourceManager_->CheckRegisterParam(devId, dhId, parameters, callback);
    EXPECT_EQ(false, ret);
    std::string devcieId(DEV_ID_LENGTH_MAX + 1, 'a');
    ret = sourceManager_->CheckRegisterParam(devcieId, dhId, parameters, callback);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, CheckRegisterParam_02, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "";
    std::string parameters = "";
    sptr<IRegisterDInputCallback> callback = nullptr;
    bool ret = sourceManager_->CheckRegisterParam(devId, dhId, parameters, callback);
    EXPECT_EQ(false, ret);
    std::string dhIds(DEV_ID_LENGTH_MAX + 1, 'a');
    ret = sourceManager_->CheckRegisterParam(devId, dhIds, parameters, callback);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, CheckRegisterParam_03, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    std::string parameters = "";
    sptr<IRegisterDInputCallback> callback = nullptr;
    bool ret = sourceManager_->CheckRegisterParam(devId, dhId, parameters, callback);
    EXPECT_EQ(false, ret);
    parameters = "parameters_test";
    ret = sourceManager_->CheckRegisterParam(devId, dhId, parameters, callback);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterDistributedHardware_01, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    std::string parameters = "";
    sptr<TestRegisterDInputCb> callback(new TestRegisterDInputCb());
    int32_t ret = sourceManager_->RegisterDistributedHardware(devId, dhId, parameters, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REGISTER_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterDistributedHardware_02, testing::ext::TestSize.Level1)
{
    InputDevice pBuffer;
    pBuffer.name = "uinput_name_touch";
    pBuffer.bus = 0x03;
    pBuffer.vendor = 0x1233;
    pBuffer.product = 0xfedb;
    pBuffer.version = 3;
    pBuffer.physicalPath = "usb-hiusb-ehci-2.1/input1";
    pBuffer.uniqueId = "3";
    pBuffer.classes = INPUT_DEVICE_CLASS_TOUCH;
    pBuffer.descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";

    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = pBuffer.descriptor;
    std::string parameters;
    StructTransJson(pBuffer, parameters);
    sptr<TestRegisterDInputCb> callback(new TestRegisterDInputCb());
    DistributedInputSourceManager::InputDeviceId inputDeviceId {devId, dhId, GetNodeDesc(parameters)};
    sourceManager_->inputDevice_.push_back(inputDeviceId);
    int32_t ret = sourceManager_->RegisterDistributedHardware(devId, dhId, parameters, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, handleStartServerCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "input_slkdiek3kddkeojfe";
    DistributedInputSourceManager::InputDeviceId inputDeviceId {devId, dhId};
    sourceManager_->inputDevice_.push_back(inputDeviceId);
    sourceManager_->handleStartServerCallback(devId);
    EXPECT_EQ(1, sourceManager_->inputDevice_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, handleStartServerCallback_02, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->handleStartServerCallback(devId);

    sourceManager_->DeviceMap_[devId] = DINPUT_SOURCE_SWITCH_ON;
    devId = "devId_20221221_test";
    sourceManager_->handleStartServerCallback(devId);
    EXPECT_EQ(2, sourceManager_->DeviceMap_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, UnregCallbackNotify_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "input_slkdiek3kddkeojfe";
    int32_t ret = sourceManager_->UnregCallbackNotify(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);

    sptr<TestUnregisterDInputCb> callback(new TestUnregisterDInputCb());
    DistributedInputSourceManager::DInputClientUnregistInfo info {devId, dhId, callback};
    sourceManager_->unregCallbacks_.push_back(info);
    devId = "devId_20221221_test";
    sourceManager_->UnregCallbackNotify(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhId = "input_48094810_test";
    sourceManager_->UnregCallbackNotify(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->UnregCallbackNotify(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, CheckUnregisterParam_01, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    std::string dhId = "";
    sptr<TestUnregisterDInputCb> callback = nullptr;
    bool ret = sourceManager_->CheckUnregisterParam(devId, dhId, callback);
    EXPECT_EQ(false, ret);
    std::string devcieId(DEV_ID_LENGTH_MAX + 1, 'a');
    ret = sourceManager_->CheckUnregisterParam(devcieId, dhId, callback);
    EXPECT_EQ(false, ret);
    devId = "umkyu1b165e1be98151891erbe8r91ev";
    ret = sourceManager_->CheckUnregisterParam(devId, dhId, callback);
    EXPECT_EQ(false, ret);
    std::string dhIds(DEV_ID_LENGTH_MAX + 1, 'a');
    sourceManager_->CheckUnregisterParam(devId, dhIds, callback);
    EXPECT_EQ(false, ret);
    dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    sourceManager_->CheckUnregisterParam(devId, dhId, callback);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, CheckDeviceIsExists_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    sptr<TestUnregisterDInputCb> callback(new TestUnregisterDInputCb());
    DistributedInputSourceManager::InputDeviceId inputDeviceId {devId, dhId};
    sourceManager_->inputDevice_.push_back(inputDeviceId);
    auto it = sourceManager_->inputDevice_.begin();
    int32_t ret = sourceManager_->CheckDeviceIsExists(devId, dhId, inputDeviceId, it);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, UnregisterDistributedHardware_01, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    sptr<TestUnregisterDInputCb> callback(new TestUnregisterDInputCb());
    int32_t ret = sourceManager_->UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, UnregisterDistributedHardware_02, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    sptr<TestUnregisterDInputCb> callback(new TestUnregisterDInputCb());
    int32_t ret = sourceManager_->UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNREGISTER_FAIL, ret);
}

/**
 * @tc.name: PrepareRemoteInput
 * @tc.desc: verify the function of distributing data from any device.
 * @tc.type: FUNC
 * @tc.require: SR000H9J75
 */
HWTEST_F(DistributedInputSourceManagerTest, PrepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = sourceManager_->PrepareRemoteInput(devId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, PrepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = sourceManager_->PrepareRemoteInput(devId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
    DistributedInputSourceTransport::GetInstance().CloseInputSoftbus(devId, false);
}

HWTEST_F(DistributedInputSourceManagerTest, UnprepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t sessionId = 1;
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[devId] = sessionId;
    int32_t ret = sourceManager_->UnprepareRemoteInput(devId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, UnprepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = sourceManager_->UnprepareRemoteInput(devId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    ret = sourceManager_->UnprepareRemoteInput(devId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);
}

/**
 * @tc.name: StartRemoteInput
 * @tc.desc: verify the function of starting distributed input on InputDeviceType.
 * @tc.type: FUNC
 * @tc.require: SR000H9J75
 */
HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t sessionId = 1;
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[devId] = sessionId;
    int32_t ret = sourceManager_->StartRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sourceManager_->StartRemoteInput(devId, INPUTTYPE_MOUSE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_02, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret = sourceManager_->StartRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
    devId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputSourceManager::DInputClientStartInfo info {devId, INPUTTYPE, callback};
    sourceManager_->staCallbacks_.push_back(info);
    ret = sourceManager_->StartRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);

    ret = sourceManager_->StartRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_03, testing::ext::TestSize.Level1)
{
    std::string devId = "devId_4810input4809_test";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret = sourceManager_->StartRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

/**
 * @tc.name: StopRemoteInput
 * @tc.desc: verify the function of stoping distributed input on InputDeviceType.
 * @tc.type: FUNC
 * @tc.require: SR000H9J75
 */
HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret = sourceManager_->StopRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sourceManager_->StopRemoteInput(devId, INPUTTYPE_MOUSE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_02, testing::ext::TestSize.Level1)
{
    std::string devId = "";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret = sourceManager_->StopRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
    devId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputSourceManager::DInputClientStopInfo info {devId, INPUTTYPE, callback};
    sourceManager_->stpCallbacks_.push_back(info);
    ret = sourceManager_->StopRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_03, testing::ext::TestSize.Level1)
{
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    std::string devId = "devId_4810input4809_test";
    int32_t ret = sourceManager_->StopRemoteInput(devId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

/**
 * @tc.name: StartRemoteInput
 * @tc.desc: verify the function of starting distributed input on InputDeviceType.
 * @tc.type: FUNC
 * @tc.require: SR000H9J75
 */
HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_04, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret = sourceManager_->StartRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_05, testing::ext::TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t ret = sourceManager_->StartRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
    srcId = "srcId_4810input4809_test";
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputSourceManager::DInputClientStartTypeInfo info(srcId, sinkId, INPUTTYPE, callback);
    sourceManager_->relayStaTypeCallbacks_.push_back(info);
    ret = sourceManager_->StartRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    DistributedInputSourceManager::DInputClientStartInfo startInfo {sinkId, INPUTTYPE, callback};
    sourceManager_->staCallbacks_.push_back(startInfo);
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    ret = sourceManager_->StartRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_06, testing::ext::TestSize.Level1)
{
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "devId_4810input4809_test";
    int32_t ret = sourceManager_->StartRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

/**
 * @tc.name: StopRemoteInput
 * @tc.desc: verify the function of stoping distributed input on InputDeviceType.
 * @tc.type: FUNC
 * @tc.require: SR000H9J75
 */
HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_04, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t sessionId = 1;
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    int32_t ret = sourceManager_->StopRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_05, testing::ext::TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t ret = sourceManager_->StopRemoteInput(srcId, sinkId,
        INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
    srcId = "srcId_4810input4809_test";
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputSourceManager::DInputClientStopTypeInfo info(srcId, sinkId, INPUTTYPE, callback);
    sourceManager_->relayStpTypeCallbacks_.push_back(info);
    ret = sourceManager_->StopRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    DistributedInputSourceManager::DInputClientStopInfo stopInfo {sinkId, INPUTTYPE, callback};
    sourceManager_->stpCallbacks_.push_back(stopInfo);
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    ret = sourceManager_->StopRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_06, testing::ext::TestSize.Level1)
{
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "devId_4810input4809_test";
    int32_t ret = sourceManager_->StopRemoteInput(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStartRemoteInputByType_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = sourceManager_->RelayStartRemoteInputByType(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_test";
    ret = sourceManager_->RelayStartRemoteInputByType(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sourceManager_->RelayStartRemoteInputByType(srcId, sinkId, INPUTTYPE_MOUSE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStartRemoteInputByType_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->RelayStartRemoteInputByType(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStopRemoteInputByType_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = sourceManager_->RelayStopRemoteInputByType(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_test";
    ret = sourceManager_->RelayStopRemoteInputByType(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sourceManager_->RelayStopRemoteInputByType(srcId, sinkId, INPUTTYPE_MOUSE, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStopRemoteInputByType_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->RelayStopRemoteInputByType(srcId, sinkId, INPUTTYPE, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

/**
 * @tc.name: PrepareRemoteInput
 * @tc.desc: verify the function of distributing data from any device.
 * @tc.type: FUNC
 * @tc.require: SR000H9J77
 */
HWTEST_F(DistributedInputSourceManagerTest, PrepareRemoteInput_03, testing::ext::TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t ret = sourceManager_->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_PREPARE_FAIL, ret);

    srcId = "srcId_4810input4809_test";
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    ret = sourceManager_->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    ret = sourceManager_->PrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
    DistributedInputSourceTransport::GetInstance().CloseInputSoftbus(sinkId, false);
}

/**
 * @tc.name: UnprepareRemoteInput
 * @tc.desc: verify the function of disabling a peripheral device.
 * @tc.type: FUNC
 * @tc.require: SR000H9J77
 */

HWTEST_F(DistributedInputSourceManagerTest, UnprepareRemoteInput_04, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t sessionId = 1;
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    int32_t ret = sourceManager_->UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, UnprepareRemoteInput_05, testing::ext::TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = sourceManager_->UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);
    srcId = "srcId_4810input4809_test";
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    ret = sourceManager_->UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);

    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    ret = sourceManager_->UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    ret = sourceManager_->UnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);
}

/**
 * @tc.name: StartRemoteInput
 * @tc.desc: verify the function of starting distributed input with dhid.
 * @tc.type: FUNC
 * @tc.require: SR000H9J74
 */
HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_07, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    int32_t sessionId = 1;
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    int32_t ret = sourceManager_->StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_20221221_test";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    ret = sourceManager_->StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhIds.push_back("Input_48094810_test");
    ret = sourceManager_->StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_08, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    int32_t ret = sourceManager_->StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);

    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputSourceManager::DInputClientStartDhidInfo info {srcId, sinkId, dhIds, callback};
    sourceManager_->staStringCallbacks_.push_back(info);
    ret = sourceManager_->StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_09, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    sourceManager_->staStringCallbacks_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->StartRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

/**
 * @tc.name: StopRemoteInput
 * @tc.desc: verify the function of stoping distributed input with dhid.
 * @tc.type: FUNC
 * @tc.require: SR000H9J74
 */
HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_07, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    int32_t ret = sourceManager_->StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_20221221_test";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    ret = sourceManager_->StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhIds.push_back("Input_48094810_test");
    ret = sourceManager_->StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_08, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    int32_t ret = sourceManager_->StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    DistributedInputSourceManager::DInputClientStopDhidInfo info {srcId, sinkId, dhIds, callback};
    sourceManager_->stpStringCallbacks_.push_back(info);
    ret = sourceManager_->StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_09, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    sourceManager_->stpStringCallbacks_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->StopRemoteInput(sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

/**
 * @tc.name: StartRemoteInput
 * @tc.desc: verify the function of starting distributed input with dhid.
 * @tc.type: FUNC
 * @tc.require: SR000H9J74
 */
HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_10, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    int32_t sessionId = 1;
    sourceManager_->stpStringCallbacks_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    int32_t ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_20221221_test";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhIds.push_back("Input_48094810_test");
    ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_11, testing::ext::TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    int32_t ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);

    dhIds.push_back("Input_slkdiek3kddkeojfe");
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    srcId = "srcId_4810input4809_test";
    DistributedInputSourceManager::DInputClientStartDhidInfo info{srcId, sinkId, dhIds, callback};
    sourceManager_->relayStaDhidCallbacks_.push_back(info);
    ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);

    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    DistributedInputSourceManager::DInputClientStartDhidInfo startDhIdInfo {srcId, sinkId, dhIds, callback};
    sourceManager_->staStringCallbacks_.push_back(startDhIdInfo);
    ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StartRemoteInput_12, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    sourceManager_->staStringCallbacks_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->StartRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

/**
 * @tc.name: StopRemoteInput
 * @tc.desc: verify the function of stoping distributed input with dhid.
 * @tc.type: FUNC
 * @tc.require: SR000H9J74
 */
HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_10, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    int32_t sessionId = 1;
    sourceManager_->stpStringCallbacks_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    int32_t ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_20221221_test";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[sinkId] = sessionId;
    ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhIds.push_back("Input_48094810_test");
    ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_11, testing::ext::TestSize.Level1)
{
    std::string srcId = "";
    std::string sinkId = "";
    std::vector<std::string> dhIds;
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    int32_t ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);

    dhIds.push_back("Input_slkdiek3kddkeojfe");
    sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    srcId = "srcId_4810input4809_test";
    DistributedInputSourceManager::DInputClientStopDhidInfo info{srcId, sinkId, dhIds, callback};
    sourceManager_->relayStpDhidCallbacks_.push_back(info);
    ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);

    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    DistributedInputSourceManager::DInputClientStopDhidInfo stopDhIdInfo {srcId, sinkId, dhIds, callback};
    sourceManager_->stpStringCallbacks_.push_back(stopDhIdInfo);
    ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, StopRemoteInput_12, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhIds;
    dhIds.push_back("Input_slkdiek3kddkeojfe");
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestStartStopVectorCallbackStub> callback(new TestStartStopVectorCallbackStub());
    sourceManager_->stpStringCallbacks_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->StopRemoteInput(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, IsStringDataSame_01, testing::ext::TestSize.Level1)
{
    std::vector<std::string> oldDhIds;
    std::vector<std::string> newDhIds;
    oldDhIds.push_back("afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d");
    bool ret = sourceManager_->IsStringDataSame(oldDhIds, newDhIds);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, IsStringDataSame_02, testing::ext::TestSize.Level1)
{
    std::vector<std::string> oldDhIds;
    std::vector<std::string> newDhIds;
    oldDhIds.push_back("afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d");
    newDhIds.push_back("rt12r1nr81n521be8rb1erbe1w8bg1erb18");
    bool ret = sourceManager_->IsStringDataSame(oldDhIds, newDhIds);
    EXPECT_EQ(false, ret);
    oldDhIds.push_back("rt12r1nr81n521be8rb1erbe1w8bg1erb18");
    newDhIds.push_back("afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d");
    ret = sourceManager_->IsStringDataSame(oldDhIds, newDhIds);
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterAddWhiteListCallback01, testing::ext::TestSize.Level0)
{
    sptr<TestAddWhiteListInfosCb> callback = nullptr;
    int32_t ret = sourceManager_->RegisterAddWhiteListCallback(callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REG_CALLBACK_ERR, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterAddWhiteListCallback02, testing::ext::TestSize.Level0)
{
    sptr<TestAddWhiteListInfosCb> callback(new TestAddWhiteListInfosCb());
    int32_t ret = sourceManager_->RegisterAddWhiteListCallback(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterDelWhiteListCallback01, testing::ext::TestSize.Level0)
{
    sptr<TestDelWhiteListInfosCb> callback = nullptr;
    int32_t ret = sourceManager_->RegisterDelWhiteListCallback(callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_REG_CALLBACK_ERR, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterDelWhiteListCallback02, testing::ext::TestSize.Level0)
{
    sptr<TestDelWhiteListInfosCb> callback(new TestDelWhiteListInfosCb());
    int32_t ret = sourceManager_->RegisterDelWhiteListCallback(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RegisterSimulationEventListener_01, testing::ext::TestSize.Level1)
{
    sptr<TestSimulationEventCb> callback = nullptr;
    int32_t ret = sourceManager_->RegisterSimulationEventListener(callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_SIMULATION_EVENT_CALLBACK_ERR, ret);

    ret = sourceManager_->UnregisterSimulationEventListener(callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_SIMULATION_EVENT_CALLBACK_ERR, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, UnregisterSimulationEventListener_02, testing::ext::TestSize.Level1)
{
    sptr<TestSimulationEventCb> callback(new TestSimulationEventCb());
    int32_t ret = sourceManager_->RegisterSimulationEventListener(callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sourceManager_->UnregisterSimulationEventListener(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayPrepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = sourceManager_->RelayPrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_test";
    sourceManager_->RelayPrepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayUnprepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    int32_t ret = sourceManager_->RelayUnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_test";
    sourceManager_->RelayUnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayUnprepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->RelayUnprepareRemoteInput(srcId, sinkId, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_UNPREPARE_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStartRemoteInputByDhid_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    std::vector<std::string> dhIds;
    dhIds.push_back("input_slkdiek3kddkeojfe");
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = sourceManager_->RelayStartRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    srcId = "srcId_20221221_test";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    ret = sourceManager_->RelayStartRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_20221221_test";
    sourceManager_->RelayStartRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhIds.push_back("Input_48094810_test");
    sourceManager_->RelayStartRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStartRemoteInputByDhid_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    std::vector<std::string> dhIds;
    dhIds.push_back("input_slkdiek3kddkeojfe");
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->RelayStartRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_START_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStopRemoteInputByDhid_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    std::vector<std::string> dhIds;
    dhIds.push_back("input_slkdiek3kddkeojfe");
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    int32_t sessionId = 1;
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    int32_t ret = sourceManager_->RelayStopRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    srcId = "srcId_20221221_test";
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_[srcId] = sessionId;
    ret = sourceManager_->RelayStopRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    sinkId = "sinkId_20221221_test";
    sourceManager_->RelayStopRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);

    dhIds.push_back("Input_48094810_test");
    sourceManager_->RelayStopRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RelayStopRemoteInputByDhid_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    std::vector<std::string> dhIds;
    dhIds.push_back("input_slkdiek3kddkeojfe");
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    DistributedInputTransportBase::GetInstance().remoteDevSessionMap_.clear();
    int32_t ret = sourceManager_->RelayStopRemoteInputByDhid(srcId, sinkId, dhIds, callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_STOP_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, RunRegisterCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "input_slkdiek3kddkeojfe";
    int32_t status = 0;
    sptr<TestRegisterDInputCb> callback(new TestRegisterDInputCb());
    DistributedInputSourceManager::DInputClientRegistInfo info {devId, dhId, callback};
    sourceManager_->regCallbacks_.push_back(info);
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(0, sourceManager_->regCallbacks_.size());

    DistributedInputSourceManager::DInputClientRegistInfo regInfo {devId, dhId, callback};
    sourceManager_->regCallbacks_.push_back(regInfo);
    devId = "devId_20221221_test";
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(1, sourceManager_->regCallbacks_.size());

    dhId = "dhId_20221221_test";
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(1, sourceManager_->regCallbacks_.size());

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(1, sourceManager_->regCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunUnregisterCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "input_slkdiek3kddkeojfe";
    int32_t status = 0;
    sptr<TestUnregisterDInputCb> callback(new TestUnregisterDInputCb());
    DistributedInputSourceManager::DInputClientUnregistInfo info {devId, dhId, callback};
    sourceManager_->unregCallbacks_.push_back(info);
    sourceManager_->RunUnregisterCallback(devId, dhId, status);
    EXPECT_EQ(0, sourceManager_->unregCallbacks_.size());

    DistributedInputSourceManager::DInputClientUnregistInfo unpreInfo {devId, dhId, callback};
    sourceManager_->unregCallbacks_.push_back(unpreInfo);
    devId = "devId_20221221_test";
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(1, sourceManager_->unregCallbacks_.size());

    dhId = "dhId_20221221_test";
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(1, sourceManager_->unregCallbacks_.size());

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->RunRegisterCallback(devId, dhId, status);
    EXPECT_EQ(1, sourceManager_->unregCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunPrepareCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string object = "runprepareobject";
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    DistributedInputSourceManager::DInputClientPrepareInfo info {devId, callback};
    sourceManager_->preCallbacks_.insert(info);
    sourceManager_->RunPrepareCallback(devId, status, object);
    EXPECT_EQ(0, sourceManager_->preCallbacks_.size());

    DistributedInputSourceManager::DInputClientPrepareInfo preInfo {devId, callback};
    sourceManager_->preCallbacks_.insert(preInfo);
    devId = "devId_20221221_test";
    sourceManager_->RunPrepareCallback(devId, status, object);
    EXPECT_EQ(1, sourceManager_->preCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunWhiteListCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string object = "runwhitelistobject";
    sptr<TestAddWhiteListInfosCb> callback(new TestAddWhiteListInfosCb());
    sourceManager_->addWhiteListCallbacks_.insert(callback);
    sourceManager_->RunWhiteListCallback(devId, object);
    EXPECT_EQ(1, sourceManager_->addWhiteListCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayPrepareCallback_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    sptr<TestPrepareDInputCallback> callback(new TestPrepareDInputCallback());
    DistributedInputSourceManager::DInputClientRelayPrepareInfo info {srcId, sinkId, callback};
    sourceManager_->relayPreCallbacks_.insert(info);
    sourceManager_->RunRelayPrepareCallback(srcId, sinkId, status);
    EXPECT_EQ(0, sourceManager_->relayPreCallbacks_.size());

    DistributedInputSourceManager::DInputClientRelayPrepareInfo preInfo {srcId, sinkId, callback};
    sourceManager_->relayPreCallbacks_.insert(preInfo);
    srcId = "devId_20221221_test";
    sourceManager_->RunRelayPrepareCallback(srcId, sinkId, status);
    EXPECT_EQ(1, sourceManager_->relayPreCallbacks_.size());

    sinkId = "sinkId_20221221_test";
    sourceManager_->RunRelayPrepareCallback(srcId, sinkId, status);
    EXPECT_EQ(1, sourceManager_->relayPreCallbacks_.size());

    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    sourceManager_->RunRelayPrepareCallback(srcId, sinkId, status);
    EXPECT_EQ(1, sourceManager_->relayPreCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayUnprepareCallback_01, testing::ext::TestSize.Level1)
{
    sourceManager_->relayUnpreCallbacks_.clear();
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    DistributedInputSourceManager::DInputClientRelayUnprepareInfo info {srcId, sinkId, callback};
    sourceManager_->relayUnpreCallbacks_.insert(info);
    sourceManager_->RunRelayUnprepareCallback(srcId, sinkId, status);
    EXPECT_EQ(0, sourceManager_->relayUnpreCallbacks_.size());

    DistributedInputSourceManager::DInputClientRelayUnprepareInfo unpreInfo {srcId, sinkId, callback};
    sourceManager_->relayUnpreCallbacks_.insert(unpreInfo);
    srcId = "devId_20221221_test";
    sourceManager_->RunRelayUnprepareCallback(srcId, sinkId, status);
    EXPECT_EQ(1, sourceManager_->relayUnpreCallbacks_.size());

    sinkId = "sinkId_20221221_test";
    sourceManager_->RunRelayUnprepareCallback(srcId, sinkId, status);
    EXPECT_EQ(1, sourceManager_->relayUnpreCallbacks_.size());

    srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    sourceManager_->RunRelayUnprepareCallback(srcId, sinkId, status);
    EXPECT_EQ(1, sourceManager_->relayUnpreCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunUnprepareCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string object = "runprepareobject";
    sptr<TestUnprepareDInputCallback> callback(new TestUnprepareDInputCallback());
    DistributedInputSourceManager::DInputClientUnprepareInfo info {devId, callback};
    sourceManager_->unpreCallbacks_.insert(info);
    sourceManager_->RunUnprepareCallback(devId, status);
    EXPECT_EQ(0, sourceManager_->unpreCallbacks_.size());

    DistributedInputSourceManager::DInputClientUnprepareInfo unpreInfo {devId, callback};
    sourceManager_->unpreCallbacks_.insert(unpreInfo);

    devId = "devId_20221221_test";
    sourceManager_->RunUnprepareCallback(devId, status);
    EXPECT_EQ(1, sourceManager_->unpreCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunStartCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    uint32_t inputTypes = 1;
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    DistributedInputSourceManager::DInputClientStartInfo info {devId, inputTypes, callback};
    sourceManager_->staCallbacks_.push_back(info);
    sourceManager_->RunStartCallback(devId, inputTypes, status);
    EXPECT_EQ(0, sourceManager_->staCallbacks_.size());

    DistributedInputSourceManager::DInputClientStartInfo startInfo {devId, inputTypes, callback};
    sourceManager_->staCallbacks_.push_back(startInfo);
    devId = "devId_20221221_test";
    sourceManager_->RunStartCallback(devId, inputTypes, status);
    EXPECT_EQ(1, sourceManager_->staCallbacks_.size());

    inputTypes = 3;
    sourceManager_->RunStartCallback(devId, inputTypes, status);
    EXPECT_EQ(1, sourceManager_->staCallbacks_.size());

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->RunStartCallback(devId, inputTypes, status);
    EXPECT_EQ(1, sourceManager_->staCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunStopCallback_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    uint32_t inputTypes = 1;
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    DistributedInputSourceManager::DInputClientStopInfo info {devId, inputTypes, callback};
    sourceManager_->stpCallbacks_.push_back(info);
    sourceManager_->RunStopCallback(devId, inputTypes, status);
    EXPECT_EQ(0, sourceManager_->stpCallbacks_.size());

    DistributedInputSourceManager::DInputClientStopInfo stopInfo {devId, inputTypes, callback};
    sourceManager_->stpCallbacks_.push_back(stopInfo);
    devId = "devId_20221221_test";
    sourceManager_->RunStopCallback(devId, inputTypes, status);
    EXPECT_EQ(1, sourceManager_->stpCallbacks_.size());

    inputTypes = 3;
    sourceManager_->RunStopCallback(devId, inputTypes, status);
    EXPECT_EQ(1, sourceManager_->stpCallbacks_.size());

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->RunStopCallback(devId, inputTypes, status);
    EXPECT_EQ(1, sourceManager_->stpCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunStartDhidCallback_01, testing::ext::TestSize.Level1)
{
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string dhId = "input_slkdiek3kddkeojfe";
    std::string localNetworkId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::vector<std::string> dhIds;
    dhIds.push_back("input_slkdiek3kddkeojfe");
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    DistributedInputSourceManager::DInputClientStartDhidInfo info {localNetworkId, sinkId, dhIds, callback};
    sourceManager_->staStringCallbacks_.push_back(info);
    sourceManager_->RunStartDhidCallback(sinkId, dhId, status);
    EXPECT_EQ(0, sourceManager_->staStringCallbacks_.size());

    DistributedInputSourceManager::DInputClientStartDhidInfo startInfo {localNetworkId, sinkId, dhIds, callback};
    sourceManager_->staStringCallbacks_.push_back(startInfo);
    sinkId = "sinkId_20221221_test";
    sourceManager_->RunStartDhidCallback(sinkId, dhId, status);
    EXPECT_EQ(1, sourceManager_->staStringCallbacks_.size());

    dhIds.clear();
    dhIds.push_back("input_48104809_test");
    sourceManager_->RunStartDhidCallback(sinkId, dhId, status);
    EXPECT_EQ(1, sourceManager_->staStringCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunStopDhidCallback_01, testing::ext::TestSize.Level1)
{
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string dhId = "input_slkdiek3kddkeojfe";
    std::string localNetworkId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    DistributedInputSourceManager::DInputClientStopDhidInfo info {localNetworkId, sinkId, dhIds, callback};
    sourceManager_->stpStringCallbacks_.push_back(info);
    sourceManager_->RunStopDhidCallback(sinkId, dhId, status);
    EXPECT_EQ(0, sourceManager_->stpStringCallbacks_.size());

    DistributedInputSourceManager::DInputClientStopDhidInfo stopInfo {localNetworkId, sinkId, dhIds, callback};
    sourceManager_->stpStringCallbacks_.push_back(stopInfo);
    sinkId = "sinkId_20221221_test";
    sourceManager_->RunStopDhidCallback(sinkId, dhId, status);
    EXPECT_EQ(1, sourceManager_->stpStringCallbacks_.size());

    dhIds.clear();
    dhIds.push_back("input_48104809_test");
    sourceManager_->RunStopDhidCallback(sinkId, dhId, status);
    EXPECT_EQ(1, sourceManager_->stpStringCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayStartDhidCallback_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string dhId = "input_slkdiek3kddkeojfe";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    DistributedInputSourceManager::DInputClientStartDhidInfo info{srcId, sinkId, dhIds, callback};
    sourceManager_->relayStaDhidCallbacks_.push_back(info);
    sourceManager_->RunRelayStartDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(0, sourceManager_->relayStaDhidCallbacks_.size());

    DistributedInputSourceManager::DInputClientStartDhidInfo startInfo{srcId, sinkId, dhIds, callback};
    sourceManager_->relayStaDhidCallbacks_.push_back(startInfo);
    srcId = "devId_20221221_test";
    sourceManager_->RunRelayStartDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(1, sourceManager_->relayStaDhidCallbacks_.size());

    sinkId = "sinkId_20221221_test";
    sourceManager_->RunRelayStartDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(1, sourceManager_->relayStaDhidCallbacks_.size());

    dhIds.clear();
    dhIds.push_back("input_48104809_test");
    sourceManager_->RunRelayStartDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(1, sourceManager_->relayStaDhidCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayStartDhidCallback_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string dhId = "input_slkdiek3kddkeojfe";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    sourceManager_->relayStaDhidCallbacks_.clear();
    sptr<TestStartStopDInputsCb> cb = nullptr;
    DistributedInputSourceManager::DInputClientStartDhidInfo startDhIdInfo {srcId, sinkId, dhIds, cb};
    sourceManager_->relayStaDhidCallbacks_.push_back(startDhIdInfo);
    sourceManager_->RunRelayStartDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(0, sourceManager_->relayStaDhidCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayStopDhidCallback_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string dhId = "input_slkdiek3kddkeojfe";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    sptr<TestStartStopDInputsCb> callback(new TestStartStopDInputsCb());
    DistributedInputSourceManager::DInputClientStopDhidInfo info{srcId, sinkId, dhIds, callback};
    sourceManager_->relayStpDhidCallbacks_.push_back(info);
    sourceManager_->RunRelayStopDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(0, sourceManager_->relayStpDhidCallbacks_.size());

    DistributedInputSourceManager::DInputClientStopDhidInfo stopInfo{srcId, sinkId, dhIds, callback};
    sourceManager_->relayStpDhidCallbacks_.push_back(stopInfo);
    srcId = "devId_20221221_test";
    sourceManager_->RunRelayStopDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(1, sourceManager_->relayStpDhidCallbacks_.size());

    sinkId = "sinkId_20221221_test";
    sourceManager_->RunRelayStopDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(1, sourceManager_->relayStpDhidCallbacks_.size());

    dhIds.clear();
    dhIds.push_back("input_48104809_test");
    sourceManager_->RunRelayStopDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(1, sourceManager_->relayStpDhidCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayStopDhidCallback_02, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    std::string dhId = "input_slkdiek3kddkeojfe";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    sourceManager_->relayStpDhidCallbacks_.clear();
    sptr<TestStartStopDInputsCb> cb = nullptr;
    DistributedInputSourceManager::DInputClientStopDhidInfo stopDhIdInfo {srcId, sinkId, dhIds, cb};
    sourceManager_->relayStpDhidCallbacks_.push_back(stopDhIdInfo);
    sourceManager_->RunRelayStopDhidCallback(srcId, sinkId, status, dhId);
    EXPECT_EQ(0, sourceManager_->relayStpDhidCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayStartTypeCallback_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    uint32_t inputTypes = 1;
    sptr<TestStartDInputCallback> callback(new TestStartDInputCallback());
    DistributedInputSourceManager::DInputClientStartTypeInfo info(srcId, sinkId, inputTypes, callback);
    sourceManager_->relayStaTypeCallbacks_.push_back(info);
    sourceManager_->RunRelayStartTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(0, sourceManager_->relayStaTypeCallbacks_.size());

    DistributedInputSourceManager::DInputClientStartTypeInfo startInfo(srcId, sinkId, inputTypes, callback);
    sourceManager_->relayStaTypeCallbacks_.push_back(startInfo);
    srcId = "devId_20221221_test";
    sourceManager_->RunRelayStartTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(1, sourceManager_->relayStaTypeCallbacks_.size());

    sinkId = "sinkId_20221221_test";
    sourceManager_->RunRelayStartTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(1, sourceManager_->relayStaTypeCallbacks_.size());

    inputTypes = 3;
    sourceManager_->RunRelayStartTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(1, sourceManager_->relayStaTypeCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RunRelayStopTypeCallback_01, testing::ext::TestSize.Level1)
{
    std::string srcId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    std::string sinkId = "umkyu1b165e1be98151891erbe8r91ev";
    int32_t status = 0;
    uint32_t inputTypes = 1;
    sptr<TestStopDInputCallback> callback(new TestStopDInputCallback());
    DistributedInputSourceManager::DInputClientStopTypeInfo info(srcId, sinkId, inputTypes, callback);
    sourceManager_->relayStpTypeCallbacks_.push_back(info);
    sourceManager_->RunRelayStopTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(0, sourceManager_->relayStpTypeCallbacks_.size());

    DistributedInputSourceManager::DInputClientStopTypeInfo stopInfo(srcId, sinkId, inputTypes, callback);
    sourceManager_->relayStpTypeCallbacks_.push_back(stopInfo);
    srcId = "devId_20221221_test";
    sourceManager_->RunRelayStopTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(1, sourceManager_->relayStpTypeCallbacks_.size());

    sinkId = "sinkId_20221221_test";
    sourceManager_->RunRelayStopTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(1, sourceManager_->relayStpTypeCallbacks_.size());

    inputTypes = 3;
    sourceManager_->RunRelayStopTypeCallback(srcId, sinkId, status, inputTypes);
    EXPECT_EQ(1, sourceManager_->relayStpTypeCallbacks_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, RemoveInputDeviceId_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "input_slkdiek3kddkeojfe";
    sourceManager_->RemoveInputDeviceId(deviceId, dhId);
    EXPECT_EQ(0, sourceManager_->inputDevice_.size());

    DistributedInputSourceManager::InputDeviceId inputDeviceId {deviceId, dhId};
    sourceManager_->inputDevice_.push_back(inputDeviceId);
    sourceManager_->RemoveInputDeviceId(deviceId, dhId);
    EXPECT_EQ(0, sourceManager_->inputDevice_.size());
}

HWTEST_F(DistributedInputSourceManagerTest, GetDeviceMapAllDevSwitchOff_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->DeviceMap_[devId] = DINPUT_SOURCE_SWITCH_OFF;
    bool ret = sourceManager_->GetDeviceMapAllDevSwitchOff();
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, GetDeviceMapAllDevSwitchOff_02, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->DeviceMap_[devId] = DINPUT_SOURCE_SWITCH_ON;
    bool ret = sourceManager_->GetDeviceMapAllDevSwitchOff();
    EXPECT_EQ(false, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, GetInputTypesMap_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->InputTypesMap_[devId] = 1;
    uint32_t ret = sourceManager_->GetInputTypesMap(devId);
    EXPECT_EQ(static_cast<uint32_t>(DInputDeviceType::MOUSE), ret);
}

HWTEST_F(DistributedInputSourceManagerTest, GetAllInputTypesMap_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    sourceManager_->InputTypesMap_[devId] = 1;
    uint32_t ret = sourceManager_->GetAllInputTypesMap();
    EXPECT_EQ(static_cast<uint32_t>(DInputDeviceType::MOUSE), ret);
}

HWTEST_F(DistributedInputSourceManagerTest, SetInputTypesMap_01, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    uint32_t value = 0;
    sourceManager_->InputTypesMap_[devId] = 1;
    sourceManager_->SetInputTypesMap(devId, value);
    EXPECT_EQ(true, sourceManager_->InputTypesMap_.empty());
}

HWTEST_F(DistributedInputSourceManagerTest, Dump_01, testing::ext::TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseRegisterDistributedHardware_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    std::string dhId = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    bool result = true;
    callback_->OnResponseRegisterDistributedHardware(deviceId, dhId, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponsePrepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    bool result = true;
    std::string object = "46sdf5g454dfsdfg4sd6fg";
    callback_->OnResponsePrepareRemoteInput(deviceId, result, object);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseUnprepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    bool result = true;
    callback_->OnResponseUnprepareRemoteInput(deviceId, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStartRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    uint32_t inputTypes = 7;
    bool result = true;
    callback_->OnResponseStartRemoteInput(deviceId, inputTypes, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStopRemoteInput_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    uint32_t inputTypes = 7;
    bool result = true;
    callback_->OnResponseStopRemoteInput(deviceId, inputTypes, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStartRemoteInputDhid_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    std::string dhids = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    bool result = true;
    callback_->OnResponseStartRemoteInputDhid(deviceId, dhids, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStopRemoteInputDhid_01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    std::string dhids = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    bool result = true;
    callback_->OnResponseStopRemoteInputDhid(deviceId, dhids, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseRelayPrepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    int32_t toSrcSessionId = 1;
    std::string deviceId = "as5d4a65sd4a65sd456as4d";
    bool result = true;
    std::string object = "46sdf5g454dfsdfg4sd6fg";
    callback_->OnResponseRelayPrepareRemoteInput(toSrcSessionId, deviceId, result, object);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseRelayUnprepareRemoteInput_01, testing::ext::TestSize.Level1)
{
    int32_t toSrcSessionId = 1;
    std::string deviceId = "as5d4a65sd4a65sd456as4d";
    bool result = true;
    callback_->OnResponseRelayUnprepareRemoteInput(toSrcSessionId, deviceId, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayPrepareResult_01, testing::ext::TestSize.Level1)
{
    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    callback_->OnReceiveRelayPrepareResult(status, srcId, sinkId);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayUnprepareResult_01, testing::ext::TestSize.Level1)
{
    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    callback_->OnReceiveRelayUnprepareResult(status, srcId, sinkId);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStartDhidResult_01, testing::ext::TestSize.Level1)
{
    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    std::string dhid = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    callback_->OnReceiveRelayStartDhidResult(status, srcId, sinkId, dhid);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStopDhidResult_01, testing::ext::TestSize.Level1)
{
    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    std::string dhid = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    callback_->OnReceiveRelayStopDhidResult(status, srcId, sinkId, dhid);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStartTypeResult_01, testing::ext::TestSize.Level1)
{
    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    uint32_t inputTypes = 7;
    callback_->OnReceiveRelayStartTypeResult(status, srcId, sinkId, inputTypes);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStopTypeResult_01, testing::ext::TestSize.Level1)
{
    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    uint32_t inputTypes = 7;
    callback_->OnReceiveRelayStopTypeResult(status, srcId, sinkId, inputTypes);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, InitAuto_01, testing::ext::TestSize.Level1)
{
    bool ret = sourceManager_->InitAuto();
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseRegisterDistributedHardware_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    std::string dhId = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    bool result = true;
    callback_->OnResponseRegisterDistributedHardware(deviceId, dhId, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponsePrepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    bool result = true;
    std::string object = "46sdf5g454dfsdfg4sd6fg";
    callback_->OnResponsePrepareRemoteInput(deviceId, result, object);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseUnprepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    bool result = true;
    callback_->OnResponseUnprepareRemoteInput(deviceId, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStartRemoteInput_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    uint32_t inputTypes = 7;
    bool result = true;
    callback_->OnResponseStartRemoteInput(deviceId, inputTypes, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStopRemoteInput_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    uint32_t inputTypes = 7;
    bool result = true;
    callback_->OnResponseStopRemoteInput(deviceId, inputTypes, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStartRemoteInputDhid_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    std::string dhids = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    bool result = true;
    callback_->OnResponseStartRemoteInputDhid(deviceId, dhids, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseStopRemoteInputDhid_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    std::string deviceId = "djfhskjdhf5465456ds4f654sdf6";
    std::string dhids = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    bool result = true;
    callback_->OnResponseStopRemoteInputDhid(deviceId, dhids, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseRelayPrepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t toSrcSessionId = 1;
    std::string deviceId = "as5d4a65sd4a65sd456as4d";
    bool result = true;
    std::string object = "46sdf5g454dfsdfg4sd6fg";
    callback_->OnResponseRelayPrepareRemoteInput(toSrcSessionId, deviceId, result, object);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnResponseRelayUnprepareRemoteInput_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t toSrcSessionId = 1;
    std::string deviceId = "as5d4a65sd4a65sd456as4d";
    bool result = true;
    callback_->OnResponseRelayUnprepareRemoteInput(toSrcSessionId, deviceId, result);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayPrepareResult_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    callback_->OnReceiveRelayPrepareResult(status, srcId, sinkId);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayUnprepareResult_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    callback_->OnReceiveRelayUnprepareResult(status, srcId, sinkId);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStartDhidResult_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    std::string dhid = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    callback_->OnReceiveRelayStartDhidResult(status, srcId, sinkId, dhid);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStopDhidResult_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    std::string dhid = "Input_s4df65s5d6f56asd5f6asdfasdfasdfv";
    callback_->OnReceiveRelayStopDhidResult(status, srcId, sinkId, dhid);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStartTypeResult_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    uint32_t inputTypes = 7;
    callback_->OnReceiveRelayStartTypeResult(status, srcId, sinkId, inputTypes);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, OnReceiveRelayStopTypeResult_02, testing::ext::TestSize.Level1)
{
    sourceManager_->InitAuto();

    int32_t status = 1;
    std::string srcId = "djfhskjdhf5465456ds4f654sdf6";
    std::string sinkId = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    uint32_t inputTypes = 7;
    callback_->OnReceiveRelayStopTypeResult(status, srcId, sinkId, inputTypes);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sourceManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, ParseMessage_01, testing::ext::TestSize.Level1)
{
    DistributedInputSourceManager::StopDScreenListener stopListener;

    stopListener.OnMessage(DHTopic::TOPIC_START_DSCREEN, "msessage_test");
    std::string messages = "";
    stopListener.OnMessage(DHTopic::TOPIC_STOP_DSCREEN, messages);

    std::string sinkDevId = "";
    uint64_t sourceWinId = 0;
    nlohmann::json jsonObj;
    jsonObj[SINK_DEVICE_ID] = 100;
    int32_t ret = stopListener.ParseMessage(jsonObj.dump(), sinkDevId, sourceWinId);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SINK_DEVICE_ID] = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    jsonObj[SOURCE_WINDOW_ID] = "source_window_id_test";
    ret = stopListener.ParseMessage(jsonObj.dump(), sinkDevId, sourceWinId);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SOURCE_WINDOW_ID] = 100;
    ret = stopListener.ParseMessage(jsonObj.dump(), sinkDevId, sourceWinId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceManagerTest, ParseMessage_02, testing::ext::TestSize.Level1)
{
    DistributedInputSourceManager::StartDScreenListener startListener;

    startListener.OnMessage(DHTopic::TOPIC_STOP_DSCREEN, "msessage_test");
    std::string message(SCREEN_MSG_MAX + 1, 'a');
    startListener.OnMessage(DHTopic::TOPIC_START_DSCREEN, message);
    std::string messages = "";
    startListener.OnMessage(DHTopic::TOPIC_START_DSCREEN, messages);

    std::string sinkDevId = "";
    SrcScreenInfo srcScreenInfo = {};
    nlohmann::json jsonObj;
    jsonObj[SINK_DEVICE_ID] = 100;
    int32_t ret = startListener.ParseMessage(jsonObj.dump(), sinkDevId, srcScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SINK_DEVICE_ID] = "asd4a65sd46as4da6s4d6asdasdafwebrb";
    jsonObj[SOURCE_WINDOW_ID] = "source_window_id_test";
    ret = startListener.ParseMessage(jsonObj.dump(), sinkDevId, srcScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SOURCE_WINDOW_ID] = 100;
    jsonObj[SOURCE_WINDOW_WIDTH] = "source_window_width_test";
    ret = startListener.ParseMessage(jsonObj.dump(), sinkDevId, srcScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SOURCE_WINDOW_WIDTH] = 100;
    jsonObj[SOURCE_WINDOW_HEIGHT] = "source_window_height_test";
    ret = startListener.ParseMessage(jsonObj.dump(), sinkDevId, srcScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SOURCE_WINDOW_HEIGHT] = 100;
    ret = startListener.ParseMessage(jsonObj.dump(), sinkDevId, srcScreenInfo);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS