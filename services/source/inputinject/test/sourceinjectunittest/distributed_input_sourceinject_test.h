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

#ifndef DISRIBUTED_INPUT_SOURCEINJECT_TEST_H
#define DISRIBUTED_INPUT_SOURCEINJECT_TEST_H

#include <functional>
#include <iostream>
#include <thread>
#include <refbase.h>

#include <gtest/gtest.h>

#include "constants_dinput.h"
#include "distributed_input_inject.h"
#include "distributed_input_node_manager.h"
#include "input_node_listener_stub.h"
#include "register_session_state_callback_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceInjectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    class TestInputNodeListener : public OHOS::DistributedHardware::DistributedInput::InputNodeListenerStub {
    public:
        TestInputNodeListener() = default;
        virtual ~TestInputNodeListener() = default;
        void OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId, const std::string &sinkNodeId,
            const std::string &sinkNodeDesc);
        void OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId, const std::string &sinkNodeId);
    };

    class TestRegisterSessionStateCallbackStub :
        public OHOS::DistributedHardware::DistributedInput::RegisterSessionStateCallbackStub {
    public:
        TestRegisterSessionStateCallbackStub() = default;
        virtual ~TestRegisterSessionStateCallbackStub() = default;
        void OnResult(const std::string &devId, const uint32_t status);
    };
private:
    sptr<TestInputNodeListener> inputNodelistener_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_SOURCEINJECT_TEST_H
