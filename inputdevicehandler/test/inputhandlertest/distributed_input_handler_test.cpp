/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "distributed_input_handler_test.h"

#include "system_ability_definition.h"

#include "dinput_errcode.h"
#include "distributed_input_handler.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DInputHandlerTest::SetUp()
{
}

void DInputHandlerTest::TearDown()
{
}

void DInputHandlerTest::SetUpTestCase()
{
}

void DInputHandlerTest::TearDownTestCase()
{
}

HWTEST_F(DInputHandlerTest, QueryExtraInfo_001, testing::ext::TestSize.Level1)
{
    DistributedInputHandler dInputHandler;
    uint32_t inputTypes = static_cast<uint32_t>(DInputDeviceType::ALL);
    std::map<int32_t, std::string> datas;
    std::vector<std::string> dhidsVec;
    dInputHandler.FindDevicesInfoByType(inputTypes, datas);
    dInputHandler.FindDevicesInfoByDhId(dhidsVec, datas);
    std::map<std::string, std::string> ret = dInputHandler.QueryExtraInfo();
    EXPECT_EQ(0, ret.size());
}

HWTEST_F(DInputHandlerTest, FindDevicesInfoByType_001, testing::ext::TestSize.Level1)
{
    DistributedInputHandler dInputHandler;
    dInputHandler.inputHub_ = nullptr;
    uint32_t inputTypes = static_cast<uint32_t>(DInputDeviceType::ALL);
    std::map<int32_t, std::string> datas;
    std::vector<std::string> dhidsVec;
    dInputHandler.FindDevicesInfoByType(inputTypes, datas);
    dInputHandler.FindDevicesInfoByDhId(dhidsVec, datas);
    dInputHandler.Query();
    dInputHandler.StartInputMonitorDeviceThread();

    InputDevice inputDevice;
    dInputHandler.mEventBuffer[0].type = DeviceType::DEVICE_ADDED;
    dInputHandler.mEventBuffer[0].deviceInfo = inputDevice;
    dInputHandler.NotifyHardWare(0);
    dInputHandler.mEventBuffer[1].type = DeviceType::DEVICE_REMOVED;
    dInputHandler.mEventBuffer[1].deviceInfo = inputDevice;
    dInputHandler.NotifyHardWare(1);
    dInputHandler.mEventBuffer[2].type = DeviceType::FINISHED_DEVICE_SCAN;
    dInputHandler.mEventBuffer[2].deviceInfo = inputDevice;
    dInputHandler.NotifyHardWare(2);
    std::map<std::string, std::string> ret = dInputHandler.QueryExtraInfo();
    EXPECT_EQ(0, ret.size());
    dInputHandler.inputHub_ = std::make_unique<InputHub>(true);
    dInputHandler.StartInputMonitorDeviceThread();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS