/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "unprepare_d_input_call_back_test.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void UnprepareDInputCallbackTest::SetUp()
{
}

void UnprepareDInputCallbackTest::TearDown()
{
}

void UnprepareDInputCallbackTest::SetUpTestCase()
{
}

void UnprepareDInputCallbackTest::TearDownTestCase()
{
}

void UnprepareDInputCallbackTest::TestUnprepareDInputCallbackStub::OnResult(const std::string &deviceId,
    const int32_t &status)
{
    deviceId_ = deviceId;
    status_ = status;
}

HWTEST_F(UnprepareDInputCallbackTest, UnprepareDInputCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestUnprepareDInputCallbackStub());
    UnprepareDInputCallbackProxy callBackProxy(callBackStubPtr);
    std::string deviceId = "deviceId0";
    int32_t status = 0;
    callBackProxy.OnResult(deviceId, status);
    EXPECT_STREQ(deviceId.c_str(), ((sptr<TestUnprepareDInputCallbackStub> &)callBackStubPtr)->deviceId_.c_str());
    EXPECT_EQ(status, ((sptr<TestUnprepareDInputCallbackStub> &)callBackStubPtr)->status_);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS