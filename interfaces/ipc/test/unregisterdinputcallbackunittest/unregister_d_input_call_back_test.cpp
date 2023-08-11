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

#include "unregister_d_input_call_back_test.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void UnregisterDInputCallbackTest::SetUp()
{
}

void UnregisterDInputCallbackTest::TearDown()
{
}

void UnregisterDInputCallbackTest::SetUpTestCase()
{
}

void UnregisterDInputCallbackTest::TearDownTestCase()
{
}

void UnregisterDInputCallbackTest::TestUnregisterDInputCallbackStub::OnResult(const std::string &devId,
    const std::string &dhId, const int32_t &status)
{
    deviceId_ = devId;
    dhId_ = dhId;
    status_ = status;
}

HWTEST_F(UnregisterDInputCallbackTest, UnregisterDInputCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestUnregisterDInputCallbackStub());
    UnregisterDInputCallbackProxy callBackProxy(callBackStubPtr);
    std::string deviceId = "deviceId0";
    std::string dhId = "dhId0";
    int32_t status = 0;
    callBackProxy.OnResult(deviceId, dhId, status);
    EXPECT_STREQ(deviceId.c_str(), ((sptr<TestUnregisterDInputCallbackStub> &)callBackStubPtr)->deviceId_.c_str());
    EXPECT_STREQ(dhId.c_str(), ((sptr<TestUnregisterDInputCallbackStub> &)callBackStubPtr)->dhId_.c_str());
    EXPECT_EQ(status, ((sptr<TestUnregisterDInputCallbackStub> &)callBackStubPtr)->status_);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS