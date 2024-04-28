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
#include "start_stop_d_inputs_call_back_stub.h"
#include "prepare_d_input_call_back_stub.h"
#include "start_d_input_call_back_stub.h"
#include "stop_d_input_call_back_stub.h"
#include "simulation_event_listener_stub.h"
#include "unprepare_d_input_call_back_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

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

    class TestPrepareDInputCallback : public
        OHOS::DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
    public:
        TestPrepareDInputCallback() = default;
        virtual ~TestPrepareDInputCallback() = default;
        void OnResult(const std::string &deviceId, const int32_t &status);
    };

    class TestUnprepareDInputCallback : public
        OHOS::DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
    public:
        TestUnprepareDInputCallback() = default;
        virtual ~TestUnprepareDInputCallback() = default;
        void OnResult(const std::string &deviceId, const int32_t &status);
    };

    class TestStartDInputCallback : public
        OHOS::DistributedHardware::DistributedInput::StartDInputCallbackStub {
    public:
        TestStartDInputCallback() = default;
        virtual ~TestStartDInputCallback() = default;
        void OnResult(const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status);
    };

    class TestStopDInputCallback : public
        OHOS::DistributedHardware::DistributedInput::StopDInputCallbackStub {
    public:
        TestStopDInputCallback() = default;
        virtual ~TestStopDInputCallback() = default;
        void OnResult(const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status);
    };

    class TestStartStopDInputCallback : public
        OHOS::DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
    public:
        TestStartStopDInputCallback() = default;
        virtual ~TestStartStopDInputCallback() = default;
        void OnResultDhids(const std::string &devId, const int32_t &status);
    };

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

private:
    int32_t StructTransJson(const InputDevice &pBuf, std::string &strDescriptor) const;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_IPC_TEST_H
