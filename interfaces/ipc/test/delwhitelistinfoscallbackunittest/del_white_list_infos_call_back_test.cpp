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

#include "del_white_list_infos_call_back_test.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DelWhiteListInfosCallbackTest::SetUp()
{
}

void DelWhiteListInfosCallbackTest::TearDown()
{
}

void DelWhiteListInfosCallbackTest::SetUpTestCase()
{
}

void DelWhiteListInfosCallbackTest::TearDownTestCase()
{
}

void DelWhiteListInfosCallbackTest::TestDelWhiteListInfosCallbackStub::OnResult(const std::string &deviceId)
{
    deviceId_ = deviceId;
}

HWTEST_F(DelWhiteListInfosCallbackTest, DelWhiteListInfosCallback01, testing::ext::TestSize.Level1)
{
    sptr<IRemoteObject> callBackStubPtr(new TestDelWhiteListInfosCallbackStub());
    DelWhiteListInfosCallbackProxy callBackProxy(callBackStubPtr);
    std::string deviceId = "deviceId0";
    callBackProxy.OnResult(deviceId);
    EXPECT_STREQ(deviceId.c_str(), ((sptr<TestDelWhiteListInfosCallbackStub> &)callBackStubPtr)->deviceId_.c_str());
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS