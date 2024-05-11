/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef DISRIBUTED_INPUT_TRANSBASE_TEST_H
#define DISRIBUTED_INPUT_TRANSBASE_TEST_H

#include <functional>
#include <iostream>
#include <refbase.h>
#include <thread>

#include <gtest/gtest.h>

#include "constants_dinput.h"
#include "distributed_input_source_transport.h"
#include "distributed_input_sink_transport.h"
#include "distributed_input_transport_base.h"
#include "dinput_transbase_source_callback.h"
#include "dinput_transbase_sink_callback.h"
#include "register_session_state_callback_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputTransbaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class TestRegisterSessionStateCallbackStub :
        public OHOS::DistributedHardware::DistributedInput::RegisterSessionStateCallbackStub {
    public:
        TestRegisterSessionStateCallbackStub() = default;
        virtual ~TestRegisterSessionStateCallbackStub() = default;
        void OnResult(const std::string &devId, const uint32_t status);
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_TRANSBASE_TEST_H
