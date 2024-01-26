/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <string>

#include "constants_dinput.h"
#include "dinput_sink_state.h"
#include "dinput_errcode.h"
#include "touchpad_event_fragment_mgr.h"

using namespace testing::ext;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
using namespace std;
namespace {
const std::string DHID_1 = "input_159753qazplm";
RawEvent EVENT_1 = {
    .when = 0,
    .type = EV_KEY,
    .code = KEY_D,
    .value = 1,
    .descriptor = DHID_1
};
RawEvent EVENT_2 = {
    .when = 1,
    .type = EV_ABS,
    .code = KEY_D,
    .value = 0,
    .descriptor = DHID_1
};
RawEvent EVENT_3 = {
    .when = 1,
    .type = EV_ABS,
    .code = ABS_X,
    .value = 0,
    .descriptor = DHID_1
};

}
class DinputSinkStateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DinputSinkStateTest::SetUpTestCase()
{
}

void DinputSinkStateTest::TearDownTestCase()
{
}

void DinputSinkStateTest::SetUp()
{
}

void DinputSinkStateTest::TearDown()
{
}

HWTEST_F(DinputSinkStateTest, RecordDhIds_001, testing::ext::TestSize.Level0)
{
    std::vector<std::string> dhIds;
    dhIds.push_back(DHID_1);
    DhIdState state = DhIdState::THROUGH_IN;
    int32_t sessionId = 1;
    int32_t ret = DInputSinkState::GetInstance().RecordDhIds(dhIds, state, sessionId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DinputSinkStateTest, RecordDhIds_002, testing::ext::TestSize.Level0)
{
    std::vector<std::string> dhIds;
    dhIds.push_back(DHID_1);
    DhIdState state = DhIdState::THROUGH_OUT;
    int32_t sessionId = -1;
    DInputSinkState::GetInstance().dhIdStateMap_.clear();
    int32_t ret = DInputSinkState::GetInstance().RecordDhIds(dhIds, state, sessionId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DinputSinkStateTest, RemoveDhIds_001, testing::ext::TestSize.Level0)
{
    std::vector<std::string> dhIds;
    dhIds.push_back(DHID_1);
    int32_t ret = DInputSinkState::GetInstance().RemoveDhIds(dhIds);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DinputSinkStateTest, GetStateByDhid_001, testing::ext::TestSize.Level0)
{
    DInputSinkState::GetInstance().dhIdStateMap_[DHID_1] = DhIdState::THROUGH_OUT;
    DhIdState ret = DInputSinkState::GetInstance().GetStateByDhid(DHID_1);
    EXPECT_EQ(DhIdState::THROUGH_OUT, ret);
}

HWTEST_F(DinputSinkStateTest, GetStateByDhid_002, testing::ext::TestSize.Level0)
{
    std::string dhId = "dhId_test";
    DhIdState ret = DInputSinkState::GetInstance().GetStateByDhid(dhId);
    EXPECT_EQ(DhIdState::THROUGH_IN, ret);
}

HWTEST_F(DinputSinkStateTest, SimulateEventInjectToSrc_001, testing::ext::TestSize.Level0)
{
    int32_t sessionId = 1;
    std::string dhId = "dhId_test";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    DInputSinkState::GetInstance().Init();
    DInputSinkState::GetInstance().AddKeyDownState(EVENT_1);
    DInputSinkState::GetInstance().SimulateEventInjectToSrc(sessionId, dhIds);

    dhIds.clear();
    dhIds.push_back(DHID_1);
    DInputSinkState::GetInstance().SimulateEventInjectToSrc(sessionId, dhIds);
    EXPECT_EQ(DInputSinkState::GetInstance().keyDownStateMap_.size(), 0);
}

HWTEST_F(DinputSinkStateTest, RemoveKeyDownState_001, testing::ext::TestSize.Level0)
{
    DInputSinkState::GetInstance().keyDownStateMap_.clear();
    std::string dhId = "dhId_test";
    std::vector<std::string> dhIds;
    dhIds.push_back(dhId);
    DInputSinkState::GetInstance().AddKeyDownState(EVENT_1);
    DInputSinkState::GetInstance().RemoveKeyDownState(EVENT_1);

    dhIds.clear();
    dhIds.push_back(DHID_1);
    DInputSinkState::GetInstance().RemoveKeyDownState(EVENT_1);
    EXPECT_EQ(DInputSinkState::GetInstance().keyDownStateMap_.size(), 0);
}

HWTEST_F(DinputSinkStateTest, IsPositionEvent_001, testing::ext::TestSize.Level0)
{
    DInputSinkState::GetInstance().Init();
    auto ret = DInputSinkState::GetInstance().GetTouchPadEventFragMgr()->IsPositionEvent(EVENT_2);
    EXPECT_EQ(false, ret);
}

HWTEST_F(DinputSinkStateTest, PushEvent_001, testing::ext::TestSize.Level0)
{
    auto ret = DInputSinkState::GetInstance().GetTouchPadEventFragMgr()->PushEvent(DHID_1, EVENT_1);
    EXPECT_EQ(false, ret.first);

    ret = DInputSinkState::GetInstance().GetTouchPadEventFragMgr()->PushEvent(DHID_1, EVENT_3);
    EXPECT_EQ(false, ret.first);
}

HWTEST_F(DinputSinkStateTest, GetAndClearEvents_001, testing::ext::TestSize.Level0)
{
    std::string dhId = "dhId_test";
    auto ret = DInputSinkState::GetInstance().GetTouchPadEventFragMgr()->GetAndClearEvents(dhId);
    EXPECT_EQ(0, ret.size());
}
}
}
}