/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "distributed_input_sinkhandler_test.h"
#include "dinput_errcode.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputSinkHandlerTest::SetUp()
{
}

void DistributedInputSinkHandlerTest::TearDown()
{
}

void DistributedInputSinkHandlerTest::SetUpTestCase()
{
}

void DistributedInputSinkHandlerTest::TearDownTestCase()
{
}

HWTEST_F(DistributedInputSinkHandlerTest, InitSink01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSinkHandler::GetInstance().InitSink("params");
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, InitSink02, testing::ext::TestSize.Level1)
{
    std::string params = "";
    int32_t ret = DistributedInputSinkHandler::GetInstance().InitSink(params);
    EXPECT_EQ(DH_SUCCESS, ret);
}
HWTEST_F(DistributedInputSinkHandlerTest, InitSink03, testing::ext::TestSize.Level1)
{
    std::string params = "params";
    int32_t ret = DistributedInputSinkHandler::GetInstance().InitSink(params);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, ReleaseSink01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSinkHandler::GetInstance().ReleaseSink();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, SubscribeLocalHardware01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSinkHandler::GetInstance().SubscribeLocalHardware("dhId", "params");
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, SubscribeLocalHardware02, testing::ext::TestSize.Level1)
{
    std::string dhId = "SubscribeLocalHardware02";
    std::string params = "test2";
    int32_t ret = DistributedInputSinkHandler::GetInstance().SubscribeLocalHardware(dhId, params);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, SubscribeLocalHardware03, testing::ext::TestSize.Level1)
{
    std::string dhId = "SubscribeLocalHardware03";
    std::string params = "test3";
    int32_t ret = DistributedInputSinkHandler::GetInstance().SubscribeLocalHardware(dhId, params);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, UnsubscribeLocalHardware01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSinkHandler::GetInstance().UnsubscribeLocalHardware("dhId");
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, UnsubscribeLocalHardware02, testing::ext::TestSize.Level1)
{
    std::string dhId = "UnsubscribeLocalHardware02";
    int32_t ret = DistributedInputSinkHandler::GetInstance().UnsubscribeLocalHardware(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkHandlerTest, UnsubscribeLocalHardware03, testing::ext::TestSize.Level1)
{
    std::string dhId = "UnsubscribeLocalHardware03";
    int32_t ret = DistributedInputSinkHandler::GetInstance().UnsubscribeLocalHardware(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS