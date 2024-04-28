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

#ifndef DISTRIBUTED_INPUT_IPC_TEST_H
#define DISTRIBUTED_INPUT_IPC_TEST_H

#include <functional>
#include <iostream>
#include <refbase.h>
#include <thread>

#include <gtest/gtest.h>

#include "constants_dinput.h"
#include "dinput_sa_manager.h"
#include "distributed_input_client.h"
#include "input_node_listener_stub.h"
#include "simulation_event_listener_stub.h"


namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputIpcTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class TestInputNodeListener : public
        OHOS::DistributedHardware::DistributedInput::InputNodeListenerStub {
    public:
        TestInputNodeListener() = default;
        virtual ~TestInputNodeListener() = default;
        void OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId,
            const std::string &sinkNodeId, const std::string &sinkNodeDesc);

        void OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId,
            const std::string &sinkNodeId);
    };

    class TestSimulationEventListenerStub : public
        OHOS::DistributedHardware::DistributedInput::SimulationEventListenerStub {
    public:
        TestSimulationEventListenerStub() = default;
        virtual ~TestSimulationEventListenerStub() = default;
        int32_t OnSimulationEvent(uint32_t type, uint32_t code, int32_t value);
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_IPC_TEST_H
