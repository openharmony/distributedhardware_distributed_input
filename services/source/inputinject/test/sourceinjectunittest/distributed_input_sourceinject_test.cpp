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

#include "distributed_input_sourceinject_test.h"

#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <linux/input.h>

#include "event_handler.h"
#include "nlohmann/json.hpp"

#include "dinput_errcode.h"
#include "softbus_bus_center.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputSourceInjectTest::SetUp()
{
}

void DistributedInputSourceInjectTest::TearDown()
{
}

void DistributedInputSourceInjectTest::SetUpTestCase()
{
}

void DistributedInputSourceInjectTest::TearDownTestCase()
{
}

void DistributedInputSourceInjectTest::TestInputNodeListener::OnNodeOnLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
}

void DistributedInputSourceInjectTest::TestInputNodeListener::OnNodeOffLine(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
}

void DistributedInputSourceInjectTest::TestRegisterSessionStateCallbackStub::OnResult(const std::string &devId,
    const uint32_t status)
{
    (void)devId;
    (void)status;
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    InputDevice pBuffer;
    pBuffer.name = "uinput_name_keyboard";
    pBuffer.bus = 0x03;
    pBuffer.vendor = 0x1234;
    pBuffer.product = 0xfedc;
    pBuffer.version = 1;
    pBuffer.physicalPath = "usb-hiusb-ehci-2.1/input1";
    pBuffer.uniqueId = "1";
    pBuffer.classes = INPUT_DEVICE_CLASS_KEYBOARD;
    pBuffer.descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d";

    std::string devId = "y4umjym16tgn21m896f1nt2y1894ty61nty651m89t1m";
    std::string dhId = pBuffer.descriptor;
    std::string parameters;
    DistributedInputInject::GetInstance().StructTransJson(pBuffer, parameters);
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedHardware(devId, dhId, parameters);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedHardware02, testing::ext::TestSize.Level1)
{
    InputDevice pBuffer;
    pBuffer.name = "uinput_name_mouse";
    pBuffer.bus = 0x03;
    pBuffer.vendor = 0x1222;
    pBuffer.product = 0xfeda;
    pBuffer.version = 2;
    pBuffer.physicalPath = "usb-hiusb-ehci-2.1/input1";
    pBuffer.uniqueId = "2";
    pBuffer.classes = INPUT_DEVICE_CLASS_CURSOR;
    pBuffer.descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18";

    std::string devId = "1sdvsd1v5w1v2d1v8d1v562sd11v5sd1";
    std::string dhId = pBuffer.descriptor;
    std::string parameters;
    DistributedInputInject::GetInstance().StructTransJson(pBuffer, parameters);
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedHardware(devId, dhId, parameters);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedHardware03, testing::ext::TestSize.Level1)
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
    DistributedInputInject::GetInstance().StructTransJson(pBuffer, parameters);
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedHardware(devId, dhId, parameters);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, UnregisterDistributedHardware_001, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    int32_t ret = DistributedInputInject::GetInstance().UnregisterDistributedHardware(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedHardware04, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    std::string parameters;
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedHardware(devId, dhId, parameters);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, UnregisterDistributedHardware_002, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().UnregisterDistributedHardware(devId, dhId);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent01, testing::ext::TestSize.Level1)
{
    RawEvent event1 = {
        .when = 0,
        .type = EV_KEY,
        .code = KEY_D,
        .value = 1,
        .descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d"
    };
    RawEvent event2 = {
        .when = 1,
        .type = EV_KEY,
        .code = KEY_D,
        .value = 0,
        .descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d"
    };
    RawEvent event3 = {
        .when = 2,
        .type = EV_KEY,
        .code = KEY_D,
        .value = 1,
        .descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d"
    };
    RawEvent event4 = {
        .when = 3,
        .type = EV_KEY,
        .code = KEY_D,
        .value = 0,
        .descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d"
    };
    std::vector<RawEvent> writeBuffer = { event1, event2, event3, event4 };

    DistributedInputInject::GetInstance().inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
    DistributedInputInject::GetInstance().StartInjectThread();
    DistributedInputInject::GetInstance().StopInjectThread();
    std::string deviceId = "aefbg1nr81n521be8rb1erbe1w8bg1erb18";
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(deviceId, writeBuffer);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent02, testing::ext::TestSize.Level1)
{
    RawEvent event1 = {
        .when = 0,
        .type = EV_REL,
        .code = REL_X,
        .value = 2,
        .descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18"
    };
    RawEvent event2 = {
        .when = 1,
        .type = EV_REL,
        .code = REL_Y,
        .value = 2,
        .descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18"
    };
    RawEvent event3 = {
        .when = 2,
        .type = EV_REL,
        .code = REL_X,
        .value = 3,
        .descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18"
    };
    RawEvent event4 = {
        .when = 3,
        .type = EV_REL,
        .code = REL_Y,
        .value = 3,
        .descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18"
    };
    RawEvent event5 = {
        .when = 4,
        .type = EV_SYN,
        .code = SYN_REPORT,
        .value = 0,
        .descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18"
    };
    std::vector<RawEvent> writeBuffer = { event1, event2, event3, event4, event5 };
    std::string deviceId = "aefbg1nr81n521be8rb1erbe1w8bg1erb18";
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(deviceId, writeBuffer);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent03, testing::ext::TestSize.Level1)
{
    RawEvent event1 = {
        .when = 0,
        .type = EV_ABS,
        .code = ABS_X,
        .value = 1,
        .descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8"
    };
    RawEvent event2 = {
        .when = 1,
        .type = EV_ABS,
        .code = ABS_X,
        .value = 2,
        .descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8"
    };
    RawEvent event3 = {
        .when = 2,
        .type = EV_ABS,
        .code = ABS_X,
        .value = 3,
        .descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8"
    };
    RawEvent event4 = {
        .when = 3,
        .type = EV_ABS,
        .code = ABS_X,
        .value = 4,
        .descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8"
    };
    std::vector<RawEvent> writeBuffer = { event1, event2, event3, event4 };
    std::string deviceId = "aefbg1nr81n521be8rb1erbe1w8bg1erb18";
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(deviceId, writeBuffer);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent04, testing::ext::TestSize.Level1)
{
    std::string deviceId = "aefbg1nr81n521be8rb1erbe1w8bg1erb18";
    std::vector<RawEvent> writeBuffer(4);
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(deviceId, writeBuffer);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GenerateVirtualTouchScreenDHId_001, testing::ext::TestSize.Level1)
{
    std::string ret = DistributedInputInject::GetInstance().GenerateVirtualTouchScreenDHId(1, 1860, 980);
    EXPECT_NE(0, ret.size());
}

HWTEST_F(DistributedInputSourceInjectTest, CreateVirtualTouchScreenNode_001, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    DistributedInputInject::GetInstance().inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
    int32_t ret = DistributedInputInject::GetInstance().CreateVirtualTouchScreenNode(devId, dhId, 1, 1860, 980);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetVirtualTouchScreenFd_001, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputInject::GetInstance().GetVirtualTouchScreenFd();
    EXPECT_NE(-1, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RemoveVirtualTouchScreenNode_001, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    int32_t ret = DistributedInputInject::GetInstance().RemoveVirtualTouchScreenNode(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, CreateVirtualTouchScreenNode_002, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().CreateVirtualTouchScreenNode(devId, dhId, 1, 1860, 980);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RemoveVirtualTouchScreenNode_002, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().RemoveVirtualTouchScreenNode(devId, dhId);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetVirtualTouchScreenFd_002, testing::ext::TestSize.Level1)
{
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().GetVirtualTouchScreenFd();
    EXPECT_EQ(-1, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, CreateVirtualTouchScreenNode_003, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    const uint64_t srcWinId = 1;
    DistributedInputInject::GetInstance().inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->
        CreateVirtualTouchScreenNode(devId, dhId, srcWinId, 1860, 980);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RemoveVirtualTouchScreenNode_003, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->RemoveVirtualTouchScreenNode(devId, dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetDevice_001, testing::ext::TestSize.Level1)
{
    std::string deviceId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    VirtualDevice* device = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->GetDevice(deviceId, dhId, device);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_GET_DEVICE_FAIL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, OpenDevicesNode_001, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    std::string parameters = "";
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->OpenDevicesNode(devId, dhId, parameters);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL, ret);

    devId = "";
    parameters = "parameters_test";
    ret = DistributedInputInject::GetInstance().inputNodeManager_->OpenDevicesNode(devId, dhId, parameters);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL, ret);

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    dhId = "";
    ret = DistributedInputInject::GetInstance().inputNodeManager_->OpenDevicesNode(devId, dhId, parameters);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, OpenDevicesNode_002, testing::ext::TestSize.Level1)
{
    std::string devId(DEV_ID_LENGTH_MAX + 1, 'a');
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    std::string parameters = "parameters_test";
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->OpenDevicesNode(devId, dhId, parameters);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL, ret);

    devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::string dhIds(DH_ID_LENGTH_MAX + 1, 'a');
    ret = DistributedInputInject::GetInstance().inputNodeManager_->OpenDevicesNode(devId, dhIds, parameters);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL, ret);

    std::string dhIdtest = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    std::string param(STRING_MAX_SIZE + 1, 'a');
    ret = DistributedInputInject::GetInstance().inputNodeManager_->OpenDevicesNode(devId, dhIdtest, param);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterInjectEventCb_001, testing::ext::TestSize.Level1)
{
    sptr<ISessionStateCallback> callback = nullptr;
    auto ret = DistributedInputInject::GetInstance().RegisterInjectEventCb(callback);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_MANAGER_INJECT_EVENT_CB_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterInjectEventCb_002, testing::ext::TestSize.Level1)
{
    sptr<TestRegisterSessionStateCallbackStub> callback(new TestRegisterSessionStateCallbackStub());
    auto ret = DistributedInputInject::GetInstance().RegisterInjectEventCb(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetVirtualKeyboardPaths_001, testing::ext::TestSize.Level1)
{
    std::string devId;
    std::vector<std::string> dhIds;
    std::vector<std::string> virKeyboardPaths;
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    DistributedInputInject::GetInstance().GetVirtualKeyboardPaths(devId, dhIds, virKeyboardPaths);

    std::string dhId;
    DistributedInputInject::GetInstance().NotifyNodeMgrScanVirNode(devId, dhId);

    DistributedInputInject::GetInstance().inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
    DistributedInputInject::GetInstance().NotifyNodeMgrScanVirNode(devId, dhId);

    auto ret = DistributedInputInject::GetInstance().UnregisterInjectEventCb();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, MatchAndSavePhysicalPath_001, testing::ext::TestSize.Level1)
{
    std::string devicePath = "";
    std::string devId = "";
    std::string dhId = "";
    auto ret =
        DistributedInputInject::GetInstance().inputNodeManager_->MatchAndSavePhysicalPath(devicePath, devId, dhId);
    EXPECT_EQ(false, ret);

    DistributedInputInject::GetInstance().inputNodeManager_->isInjectThreadCreated_.store(true);
    DistributedInputInject::GetInstance().inputNodeManager_->StartInjectThread();
    DistributedInputInject::GetInstance().inputNodeManager_->isInjectThreadCreated_.store(false);
    DistributedInputInject::GetInstance().inputNodeManager_->StopInjectThread();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
