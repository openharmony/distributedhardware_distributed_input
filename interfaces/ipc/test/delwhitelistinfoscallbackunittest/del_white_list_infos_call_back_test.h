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

#ifndef DEL_WHITE_LIST_INFOS_CALLBACK_TEST_H
#define DEL_WHITE_LIST_INFOS_CALLBACK_TEST_H

#include <gtest/gtest.h>

#include "del_white_list_infos_call_back_proxy.h"
#include "del_white_list_infos_call_back_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DelWhiteListInfosCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class TestDelWhiteListInfosCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::DelWhiteListInfosCallbackStub {
    public:
        TestDelWhiteListInfosCallbackStub() = default;
        virtual ~TestDelWhiteListInfosCallbackStub() = default;
        void OnResult(const std::string &deviceId);
        std::string deviceId_;
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DEL_WHITE_LIST_INFOS_CALLBACK_TEST_H