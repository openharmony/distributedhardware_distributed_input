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

#include "dinput_context_test.h"

#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_utils_tool.h"
#include "dinput_softbus_define.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
constexpr uint32_t SIZE_AFTER_GET = 1;
constexpr uint32_t HEIGHT = 1080;

void DInputContextTest::SetUp()
{
}

void DInputContextTest::TearDown()
{
}

void DInputContextTest::SetUpTestCase()
{
}

void DInputContextTest::TearDownTestCase()
{
}

HWTEST_F(DInputContextTest, GetSourceWindId001, testing::ext::TestSize.Level0)
{
    std::string devId = "hello";
    uint64_t sourceWinId = 1;
    std::string ret = DInputContext::GetInstance().GetScreenInfoKey(devId, sourceWinId);
    EXPECT_EQ("hello###1", ret);
}

HWTEST_F(DInputContextTest, RemoveSinkScreenInfo001, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    int32_t ret = DInputContext::GetInstance().RemoveSinkScreenInfo(sourceWinId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputContextTest, UpdateSinkScreenInfo001, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SinkScreenInfo sinkScreenInfo;
    int32_t ret = DInputContext::GetInstance().UpdateSinkScreenInfo(sourceWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_CONTEXT_KEY_NOT_EXIST, ret);
}

HWTEST_F(DInputContextTest, UpdateSinkScreenInfo002, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SinkScreenInfo sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(sourceWinId);
    int32_t ret = DInputContext::GetInstance().UpdateSinkScreenInfo(sourceWinId, sinkScreenInfo);
    EXPECT_EQ(DH_SUCCESS, ret);
    DInputContext::GetInstance().RemoveSinkScreenInfo(sourceWinId);
}

HWTEST_F(DInputContextTest, GetSinkScreenInfo001, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SinkScreenInfo sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(sourceWinId);
    EXPECT_EQ(SIZE_AFTER_GET, DInputContext::GetInstance().sinkScreenInfoMap_.size());
    DInputContext::GetInstance().RemoveSinkScreenInfo(sourceWinId);
}

HWTEST_F(DInputContextTest, GetSinkScreenInfo002, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SinkScreenInfo sinkScreenInfo1;
    sinkScreenInfo1.sinkPhyHeight = HEIGHT;
    DInputContext::GetInstance().sinkScreenInfoMap_[sourceWinId] = sinkScreenInfo1;
    SinkScreenInfo sinkScreenInfo = DInputContext::GetInstance().GetSinkScreenInfo(sourceWinId);
    EXPECT_EQ(HEIGHT, sinkScreenInfo.sinkPhyHeight);
    DInputContext::GetInstance().RemoveSinkScreenInfo(sourceWinId);
}

HWTEST_F(DInputContextTest, RemoveSrcScreenInfo001, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    int32_t ret = DInputContext::GetInstance().RemoveSrcScreenInfo(sourceWinId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputContextTest, UpdateSrcScreenInfo001, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SrcScreenInfo srcScreenInfo;
    int32_t ret = DInputContext::GetInstance().UpdateSrcScreenInfo(sourceWinId, srcScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_CONTEXT_KEY_NOT_EXIST, ret);
}

HWTEST_F(DInputContextTest, UpdateSrcScreenInfo002, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(sourceWinId);
    int32_t ret = DInputContext::GetInstance().UpdateSrcScreenInfo(sourceWinId, srcScreenInfo);
    EXPECT_EQ(DH_SUCCESS, ret);
    DInputContext::GetInstance().RemoveSrcScreenInfo(sourceWinId);
}

HWTEST_F(DInputContextTest, GetSrcScreenInfo001, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(sourceWinId);
    EXPECT_EQ(SIZE_AFTER_GET, DInputContext::GetInstance().srcScreenInfoMap_.size());
    DInputContext::GetInstance().RemoveSrcScreenInfo(sourceWinId);
}

HWTEST_F(DInputContextTest, GetSrcScreenInfo002, testing::ext::TestSize.Level0)
{
    std::string sourceWinId = "hello";
    SrcScreenInfo srcScreenInfo1;
    srcScreenInfo1.sourcePhyHeight = HEIGHT;
    DInputContext::GetInstance().srcScreenInfoMap_[sourceWinId] = srcScreenInfo1;
    SrcScreenInfo srcScreenInfo = DInputContext::GetInstance().GetSrcScreenInfo(sourceWinId);
    EXPECT_EQ(HEIGHT, srcScreenInfo.sourcePhyHeight);
    DInputContext::GetInstance().RemoveSrcScreenInfo(sourceWinId);
}

HWTEST_F(DInputContextTest, SetGetLocalTouchScreenInfo001, testing::ext::TestSize.Level0)
{
    LocalTouchScreenInfo localTouchScreenInfo;
    localTouchScreenInfo.sinkShowWidth = HEIGHT;
    DInputContext::GetInstance().SetLocalTouchScreenInfo(localTouchScreenInfo);
    EXPECT_EQ(HEIGHT, DInputContext::GetInstance().GetLocalTouchScreenInfo().sinkShowWidth);
}

HWTEST_F(DInputContextTest, CalculateTransformInfo001, testing::ext::TestSize.Level0)
{
    SinkScreenInfo sinkScreenInfo;
    int32_t ret = DInputContext::GetInstance().CalculateTransformInfo(sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_CONTEXT_CALCULATE_FAIL, ret);
}

HWTEST_F(DInputContextTest, CalculateTransformInfo002, testing::ext::TestSize.Level0)
{
    SinkScreenInfo sinkScreenInfo;
    sinkScreenInfo.sinkPhyHeight = 1080;
    sinkScreenInfo.sinkPhyWidth = 960;
    sinkScreenInfo.sinkShowHeight = 1080;
    sinkScreenInfo.sinkShowWidth = 960;
    int32_t ret = DInputContext::GetInstance().CalculateTransformInfo(sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_CONTEXT_CALCULATE_FAIL, ret);
}

HWTEST_F(DInputContextTest, CalculateTransformInfo003, testing::ext::TestSize.Level0)
{
    SinkScreenInfo sinkScreenInfo;
    sinkScreenInfo.sinkPhyHeight = 1080;
    sinkScreenInfo.sinkPhyWidth = 960;
    sinkScreenInfo.sinkShowHeight = 1080;
    sinkScreenInfo.sinkShowWidth = 960;
    sinkScreenInfo.sinkProjShowHeight = 640;
    sinkScreenInfo.sinkProjShowWidth = 480;
    int32_t ret = DInputContext::GetInstance().CalculateTransformInfo(sinkScreenInfo);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DInputContextTest, GetLocalDeviceInfo_001, testing::ext::TestSize.Level1)
{
    DevInfo devInfo = GetLocalDeviceInfo();
    EXPECT_NE(0, devInfo.networkId.size());
}

HWTEST_F(DInputContextTest, GetLocalNetworkId_001, testing::ext::TestSize.Level1)
{
    std::string ret = GetLocalNetworkId();
    EXPECT_NE(0, ret.size());
}

HWTEST_F(DInputContextTest, GetUUIDBySoftBus_001, testing::ext::TestSize.Level1)
{
    std::string networkId = "";
    std::string ret = GetUUIDBySoftBus(networkId);
    EXPECT_EQ(0, ret.size());

    networkId = "networkidc08647073e02e7a78f09473aa122ff57fc81c00";
    ret = GetUUIDBySoftBus(networkId);
    EXPECT_EQ(0, ret.size());
}

HWTEST_F(DInputContextTest, GetCurrentTime_001, testing::ext::TestSize.Level1)
{
    uint64_t ret = GetCurrentTimeUs();
    EXPECT_NE(0, ret);
}

HWTEST_F(DInputContextTest, SetAnonyId_001, testing::ext::TestSize.Level1)
{
    std::string message = "";
    std::string ret = SetAnonyId(message);
    EXPECT_EQ(0, ret.size());
}

HWTEST_F(DInputContextTest, SetAnonyId_002, testing::ext::TestSize.Level1)
{
    nlohmann::json jsonObj;
    std::string deviceId = "deviceId_test";
    std::string inputData = "inputData_data";
    std::string vecDhId = "dhId_123.dhId_456.dhId_789";
    std::string srcDevId = "srcDevId_test";
    std::string sinkDevId = "sinkDevId_test";
    jsonObj[DINPUT_SOFTBUS_KEY_DEVICE_ID] = deviceId;
    jsonObj[DINPUT_SOFTBUS_KEY_INPUT_DATA] = inputData;
    jsonObj[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = vecDhId;
    jsonObj[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = srcDevId;
    jsonObj[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = sinkDevId;
    std::string ret = SetAnonyId(jsonObj.dump());
    EXPECT_NE(0, ret.size());
}

HWTEST_F(DInputContextTest, GetNodeDesc_001, testing::ext::TestSize.Level1)
{
    std::string parameters = "";
    std::string ret = GetNodeDesc(parameters);
    EXPECT_EQ(0, ret.size());
}

HWTEST_F(DInputContextTest, JointDhIds_001, testing::ext::TestSize.Level1)
{
    std::vector<std::string> dhids;
    std::string ret = JointDhIds(dhids);
    EXPECT_EQ("", ret);
}

} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
