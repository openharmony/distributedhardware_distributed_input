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

#include "white_list_test.h"
#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "white_list_util.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void WhiteListTest::SetUp()
{
}

void WhiteListTest::TearDown()
{
}

void WhiteListTest::SetUpTestCase()
{
}

void WhiteListTest::TearDownTestCase()
{
}

HWTEST_F(WhiteListTest, SyncWhiteList01, testing::ext::TestSize.Level1)
{
    // 11|22,33|44,55,66
    // 1,2|3,4,5
    std::string deviceId = "test";
    TYPE_KEY_CODE_VEC vecKeyCode;
    TYPE_COMBINATION_KEY_VEC vecCombinationKey;
    TYPE_WHITE_LIST_VEC vecWhiteList;
    vecKeyCode.push_back(11);
    vecKeyCode.push_back(22);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(33);
    vecKeyCode.push_back(44);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(55);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(66);
    vecCombinationKey.push_back(vecKeyCode);
    vecWhiteList.push_back(vecCombinationKey);
    vecKeyCode.clear();
    vecCombinationKey.clear();
    int32_t ret = WhiteListUtil::GetInstance().SyncWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, SyncWhiteList02, testing::ext::TestSize.Level1)
{
    // 11|22,33|44,55,66
    // 1,2|3,4,5
    std::string deviceId = "test1";
    TYPE_KEY_CODE_VEC vecKeyCode;
    TYPE_COMBINATION_KEY_VEC vecCombinationKey;
    TYPE_WHITE_LIST_VEC vecWhiteList;

    vecKeyCode.push_back(1);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(2);
    vecKeyCode.push_back(3);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(4);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(5);
    vecCombinationKey.push_back(vecKeyCode);
    vecWhiteList.push_back(vecCombinationKey);
    vecKeyCode.clear();
    vecCombinationKey.clear();
    int32_t ret = WhiteListUtil::GetInstance().SyncWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, SyncWhiteList03, testing::ext::TestSize.Level1)
{
    // 1,2,1,2
    std::string deviceId = "test1";
    TYPE_KEY_CODE_VEC vecKeyCode;
    TYPE_COMBINATION_KEY_VEC vecCombinationKey;
    TYPE_WHITE_LIST_VEC vecWhiteList;

    vecKeyCode.push_back(1);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(2);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(1);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(2);
    vecCombinationKey.push_back(vecKeyCode);
    vecWhiteList.push_back(vecCombinationKey);
    vecKeyCode.clear();
    vecCombinationKey.clear();
    int32_t ret = WhiteListUtil::GetInstance().SyncWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, SyncWhiteList04, testing::ext::TestSize.Level1)
{
    // 2,8|3,4,5
    std::string deviceId = "test1";
    TYPE_KEY_CODE_VEC vecKeyCode;
    TYPE_COMBINATION_KEY_VEC vecCombinationKey;
    TYPE_WHITE_LIST_VEC vecWhiteList;
    vecKeyCode.push_back(2);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(8);
    vecKeyCode.push_back(3);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(4);
    vecCombinationKey.push_back(vecKeyCode);
    vecKeyCode.clear();
    vecKeyCode.push_back(5);
    vecCombinationKey.push_back(vecKeyCode);
    vecWhiteList.push_back(vecCombinationKey);
    vecKeyCode.clear();
    vecCombinationKey.clear();
    int32_t ret = WhiteListUtil::GetInstance().SyncWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, GetWhiteList01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "test";
    TYPE_WHITE_LIST_VEC vecWhiteList;
    int32_t ret = WhiteListUtil::GetInstance().GetWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, GetWhiteList02, testing::ext::TestSize.Level1)
{
    std::string deviceId = "test1";
    TYPE_WHITE_LIST_VEC vecWhiteList;
    int32_t ret = WhiteListUtil::GetInstance().GetWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, GetWhiteList03, testing::ext::TestSize.Level1)
{
    std::string deviceId = "xxx";
    TYPE_WHITE_LIST_VEC vecWhiteList;
    int32_t ret = WhiteListUtil::GetInstance().GetWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(ERR_DH_INPUT_WHILTELIST_GET_WHILTELIST_FAIL, ret);
}

HWTEST_F(WhiteListTest, GetWhiteList04, testing::ext::TestSize.Level1)
{
    std::string deviceId = "";
    TYPE_WHITE_LIST_VEC vecWhiteList;
    int32_t ret = WhiteListUtil::GetInstance().GetWhiteList(deviceId, vecWhiteList);
    EXPECT_EQ(ERR_DH_INPUT_WHILTELIST_GET_WHILTELIST_FAIL, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut01, testing::ext::TestSize.Level1)
{
    // 11,33,55,66
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(11);
    event.pressedKeys.push_back(33);
    event.keyCode = 55;
    event.keyAction = 66;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(true, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut02, testing::ext::TestSize.Level1)
{
    // 11,44,55,66
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(11);
    event.pressedKeys.push_back(44);
    event.keyCode = 55;
    event.keyAction = 66;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(true, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut03, testing::ext::TestSize.Level1)
{
    // 22,33,55,66
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(22);
    event.pressedKeys.push_back(33);
    event.keyCode = 55;
    event.keyAction = 66;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(true, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut04, testing::ext::TestSize.Level1)
{
    // 22,44,55,66
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(22);
    event.pressedKeys.push_back(44);
    event.keyCode = 55;
    event.keyAction = 66;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(true, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut05, testing::ext::TestSize.Level1)
{
    // 1,2,4,5
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(1);
    event.pressedKeys.push_back(2);
    event.keyCode = 4;
    event.keyAction = 5;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut06, testing::ext::TestSize.Level1)
{
    // 1,3,4,5
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(1);
    event.pressedKeys.push_back(3);
    event.keyCode = 4;
    event.keyAction = 5;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut07, testing::ext::TestSize.Level1)
{
    // 2,2,4,5
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(2);
    event.pressedKeys.push_back(2);
    event.keyCode = 4;
    event.keyAction = 5;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut08, testing::ext::TestSize.Level1)
{
    // 1,1,4,5
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(1);
    event.pressedKeys.push_back(1);
    event.keyCode = 4;
    event.keyAction = 5;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut09, testing::ext::TestSize.Level1)
{
    // 1,2,1,5
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(1);
    event.pressedKeys.push_back(2);
    event.keyCode = 1;
    event.keyAction = 5;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut10, testing::ext::TestSize.Level1)
{
    // 1,2,4,1
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(1);
    event.pressedKeys.push_back(2);
    event.keyCode = 4;
    event.keyAction = 1;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, IsNeedFilterOut11, testing::ext::TestSize.Level1)
{
    // 1,2,4,5,6
    std::string deviceId = "test";
    BusinessEvent event;
    event.pressedKeys.push_back(1);
    event.pressedKeys.push_back(2);
    event.pressedKeys.push_back(4);
    event.keyCode = 5;
    event.keyAction = 6;
    bool ret = WhiteListUtil::GetInstance().IsNeedFilterOut(deviceId, event);
    EXPECT_EQ(false, ret);
}

HWTEST_F(WhiteListTest, ClearWhiteList01, testing::ext::TestSize.Level1)
{
    std::string deviceId = "test";
    int32_t ret = WhiteListUtil::GetInstance().ClearWhiteList(deviceId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, ClearWhiteList02, testing::ext::TestSize.Level1)
{
    std::string deviceId = "test1";
    int32_t ret = WhiteListUtil::GetInstance().ClearWhiteList(deviceId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(WhiteListTest, ClearWhiteList03, testing::ext::TestSize.Level1)
{
    std::string deviceId;
    int32_t ret = WhiteListUtil::GetInstance().ClearWhiteList(deviceId);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
