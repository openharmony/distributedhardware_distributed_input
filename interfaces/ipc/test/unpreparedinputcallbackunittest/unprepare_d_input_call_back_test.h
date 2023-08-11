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

#ifndef UNPREPARE_DINPUT_CALLBACK_TEST_H
#define UNPREPARE_DINPUT_CALLBACK_TEST_H

#include <gtest/gtest.h>

#include "unprepare_d_input_call_back_proxy.h"
#include "unprepare_d_input_call_back_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class UnprepareDInputCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class TestUnprepareDInputCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
    public:
        TestUnprepareDInputCallbackStub() = default;
        virtual ~TestUnprepareDInputCallbackStub() = default;
        void OnResult(const std::string &deviceId, const int32_t &status);
        std::string deviceId_;
        int32_t status_;
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // UNPREPARE_DINPUT_CALLBACK_TEST_H