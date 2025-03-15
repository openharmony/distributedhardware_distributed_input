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

#include "distributed_input_sourcehandler_test.h"
#include "dinput_errcode.h"

using namespace testing::ext;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputSourceHandlerTest::SetUp()
{
}

void DistributedInputSourceHandlerTest::TearDown()
{
}

void DistributedInputSourceHandlerTest::SetUpTestCase()
{
}

void DistributedInputSourceHandlerTest::TearDownTestCase()
{
}

int32_t DistributedInputSourceHandlerTest::TestRegisterDInputCallback::OnRegisterResult(const std::string &devId,
    const std::string &dhId, int32_t status, const std::string &data)
{
    return DH_SUCCESS;
}

int32_t DistributedInputSourceHandlerTest::TestUnregisterDInputCallback::OnUnregisterResult(const std::string &devId,
    const std::string &dhId, int32_t status, const std::string &data)
{
    return DH_SUCCESS;
}

HWTEST_F(DistributedInputSourceHandlerTest, InitSource01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSourceHandler::GetInstance().InitSource("");
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, InitSource02, testing::ext::TestSize.Level1)
{
    std::string dhId = "test";
    int32_t ret = DistributedInputSourceHandler::GetInstance().InitSource(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, ReleaseSource01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSourceHandler::GetInstance().ReleaseSource();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, RegisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestRegisterDInputCallback> registerCallback = std::make_shared<TestRegisterDInputCallback>();
    int32_t ret = DistributedInputSourceHandler::GetInstance().RegisterDistributedHardware(
        "devId",
        "dhId",
        OHOS::DistributedHardware::EnableParam {
            "version", "attrs"
        },
        registerCallback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, RegisterDistributedHardware02, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestRegisterDInputCallback> registerCallback = nullptr;
    std::string devId = "";
    std::string dhId = "";
    int32_t ret = DistributedInputSourceHandler::GetInstance().RegisterDistributedHardware(
        devId, dhId, OHOS::DistributedHardware::EnableParam {"version", "attrs" }, registerCallback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, RegisterDistributedHardware03, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestRegisterDInputCallback> registerCallback = std::make_shared<TestRegisterDInputCallback>();
    int32_t ret = DistributedInputSourceHandler::GetInstance().RegisterDistributedHardware(
        "devId",
        "dhId",
        OHOS::DistributedHardware::EnableParam {
            "version", "attrs"
        },
        registerCallback);
    EXPECT_EQ(DH_SUCCESS, ret);
}
HWTEST_F(DistributedInputSourceHandlerTest, UnregisterDistributedHardware01, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestUnregisterDInputCallback> unregisterDInputCallback =
            std::make_shared<TestUnregisterDInputCallback>();
    int32_t ret = DistributedInputSourceHandler::GetInstance().UnregisterDistributedHardware("devId",
        "dhId", unregisterDInputCallback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, UnregisterDistributedHardware02, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestUnregisterDInputCallback> unregisterDInputCallback = nullptr;
    std::string devId = "";
    std::string dhId = "";
    int32_t ret = DistributedInputSourceHandler::GetInstance().UnregisterDistributedHardware("devId",
        "dhId", unregisterDInputCallback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, UnregisterDistributedHardware03, testing::ext::TestSize.Level1)
{
    std::shared_ptr<TestUnregisterDInputCallback> unregisterDInputCallback =
            std::make_shared<TestUnregisterDInputCallback>();
    int32_t ret = DistributedInputSourceHandler::GetInstance().UnregisterDistributedHardware("devId",
        "dhId", unregisterDInputCallback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSourceHandlerTest, ConfigDistributedHardware01, testing::ext::TestSize.Level1)
{
    int32_t ret = DistributedInputSourceHandler::GetInstance().ConfigDistributedHardware("devId",
        "dhId", "key", "value");
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS