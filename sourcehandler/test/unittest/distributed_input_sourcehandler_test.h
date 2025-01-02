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

#ifndef DISRIBUTED_INPUT_SOURCEHANDLER_TEST_H
#define DISRIBUTED_INPUT_SOURCEHANDLER_TEST_H

#include <functional>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>
#include <refbase.h>

#include "idistributed_hardware_source.h"
#include "system_ability_load_callback_stub.h"

#include "constants_dinput.h"
#include "distributed_input_kit.h"
#include "distributed_input_source_handler.h"
#include "i_distributed_source_input.h"
#include "register_d_input_call_back_stub.h"
#include "unregister_d_input_call_back_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    int32_t CheckSystemAbility();

    class TestRegisterDInputCallback : public OHOS::DistributedHardware::RegisterCallback {
    public:
        TestRegisterDInputCallback() = default;
        virtual ~TestRegisterDInputCallback() = default;
        int32_t OnRegisterResult(const std::string &devId, const std::string &dhId, int32_t status,
            const std::string &data) override;
    };

    class TestUnregisterDInputCallback : public OHOS::DistributedHardware::UnregisterCallback {
    public:
        TestUnregisterDInputCallback() = default;
        virtual ~TestUnregisterDInputCallback() = default;
        int32_t OnUnregisterResult(const std::string &devId, const std::string &dhId, int32_t status,
        const std::string &data) override;
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_SOURCEHANDLER_TEST_H
