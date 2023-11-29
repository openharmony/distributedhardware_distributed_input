/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

void DistributedInputSourceInjectTest::TestInputNodeListener::OnNodeOnLine(const std::string srcDevId,
    const std::string sinkDevId, const std::string sinkNodeId, const std::string sinkNodeDesc)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
    (void)sinkNodeDesc;
}

void DistributedInputSourceInjectTest::TestInputNodeListener::OnNodeOffLine(const std::string srcDevId,
    const std::string sinkDevId, const std::string sinkNodeId)
{
    (void)srcDevId;
    (void)sinkDevId;
    (void)sinkNodeId;
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
    struct RawEvent writeBuffer[4];
    RawEvent* event = writeBuffer;

    event->when = 0;
    event->type = EV_KEY;
    event->code = KEY_D;
    event->value = 1;
    event->descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d";
    event += 1;

    event->when = 1;
    event->type = EV_KEY;
    event->code = KEY_D;
    event->value = 0;
    event->descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d";
    event += 1;

    event->when = 2;
    event->type = EV_KEY;
    event->code = KEY_D;
    event->value = 1;
    event->descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d";
    event += 1;

    event->when = 3;
    event->type = EV_KEY;
    event->code = KEY_D;
    event->value = 0;
    event->descriptor = "afv4s8b1dr1b8er1bd65fb16redb1dfb18d1b56df1b68d";

    size_t count = (size_t)(sizeof(writeBuffer) / sizeof(RawEvent));
    DistributedInputInject::GetInstance().inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
    DistributedInputInject::GetInstance().StartInjectThread();
    DistributedInputInject::GetInstance().StopInjectThread();
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(writeBuffer, count);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent02, testing::ext::TestSize.Level1)
{
    struct RawEvent writeBuffer[4];
    RawEvent* event = writeBuffer;

    event->when = 0;
    event->type = EV_REL;
    event->code = REL_X;
    event->value = 2;
    event->descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18";
    event += 1;

    event->when = 1;
    event->type = EV_REL;
    event->code = REL_Y;
    event->value = 2;
    event->descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18";
    event += 1;

    event->when = 2;
    event->type = EV_REL;
    event->code = REL_X;
    event->value = 2;
    event->descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18";
    event += 1;

    event->when = 3;
    event->type = EV_REL;
    event->code = REL_Y;
    event->value = 2;
    event->descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18";

    event->when = 4;
    event->type = EV_SYN;
    event->code = SYN_REPORT;
    event->value = 0;
    event->descriptor = "rt12r1nr81n521be8rb1erbe1w8bg1erb18";

    size_t count = (size_t)(sizeof(writeBuffer) / sizeof(RawEvent));
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(writeBuffer, count);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent03, testing::ext::TestSize.Level1)
{
    struct RawEvent writeBuffer[4];
    RawEvent* event = writeBuffer;

    event->when = 0;
    event->type = EV_ABS;
    event->code = ABS_X;
    event->value = 1;
    event->descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    event += 1;

    event->when = 1;
    event->type = EV_ABS;
    event->code = ABS_X;
    event->value = 2;
    event->descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    event += 1;

    event->when = 2;
    event->type = EV_ABS;
    event->code = ABS_X;
    event->value = 3;
    event->descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    event += 1;

    event->when = 3;
    event->type = EV_ABS;
    event->code = ABS_X;
    event->value = 4;
    event->descriptor = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";

    size_t count = (size_t)(sizeof(writeBuffer) / sizeof(RawEvent));
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(writeBuffer, count);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, RegisterDistributedEvent04, testing::ext::TestSize.Level1)
{
    struct RawEvent writeBuffer[4];
    size_t count = (size_t)(sizeof(writeBuffer) / sizeof(RawEvent));
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().RegisterDistributedEvent(writeBuffer, count);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetDhIdsByInputType_001, testing::ext::TestSize.Level1)
{
    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::vector<std::string> dhIds;
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    dhIds.push_back(dhId);
    std::shared_ptr<RawEvent> rawEvent = nullptr;
    DistributedInputInject::GetInstance().InputDeviceEventInject(rawEvent);
    DistributedInputInject::GetInstance().inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
    int32_t ret = DistributedInputInject::GetInstance().GetDhIdsByInputType(devId,
        static_cast<uint32_t>(DInputDeviceType::ALL), dhIds);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetDhIdsByInputType_002, testing::ext::TestSize.Level1)
{
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    std::shared_ptr<RawEvent> rawEvent = std::make_shared<RawEvent>();
    DistributedInputInject::GetInstance().InputDeviceEventInject(rawEvent);

    std::string devId = "umkyu1b165e1be98151891erbe8r91ev";
    std::vector<std::string> dhIds;
    int32_t ret = DistributedInputInject::GetInstance().GetDhIdsByInputType(devId,
        static_cast<uint32_t>(DInputDeviceType::ALL), dhIds);
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
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    int32_t ret = DistributedInputInject::GetInstance().RemoveVirtualTouchScreenNode(dhId);
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
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    DistributedInputInject::GetInstance().inputNodeManager_ = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().RemoveVirtualTouchScreenNode(dhId);
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
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->RemoveVirtualTouchScreenNode(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetVirtualTouchScreenFd_003, testing::ext::TestSize.Level1)
{
    std::string networkId = "umkyu1b165e1be98151891erbe8r91ev";
    uint32_t inputTypes = 1;
    std::map<int32_t, std::string> datas;
    DistributedInputInject::GetInstance().inputNodeManager_->GetDevicesInfoByType(networkId, inputTypes, datas);

    inputTypes = 2;
    DistributedInputInject::GetInstance().inputNodeManager_->GetDevicesInfoByType(networkId, inputTypes, datas);

    std::vector<std::string> dhidsVec;
    DistributedInputInject::GetInstance().inputNodeManager_->GetDevicesInfoByDhId(dhidsVec, datas);
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->GetVirtualTouchScreenFd();
    EXPECT_NE(-1, ret);
}

HWTEST_F(DistributedInputSourceInjectTest, GetDevice_001, testing::ext::TestSize.Level1)
{
    std::string dhId = "1ds56v18e1v21v8v1erv15r1v8r1j1ty8";
    VirtualDevice* device = nullptr;
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->GetDevice(dhId, device);
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

HWTEST_F(DistributedInputSourceInjectTest, GetDeviceInfo_001, testing::ext::TestSize.Level1)
{
    std::string deviceId = "";
    DistributedInputInject::GetInstance().inputNodeManager_->InjectEvent();
    int32_t ret = DistributedInputInject::GetInstance().inputNodeManager_->GetDeviceInfo(deviceId);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
