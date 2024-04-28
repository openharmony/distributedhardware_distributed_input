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

#ifndef DINPUT_SOURCE_CALLBACK_TEST_H
#define DINPUT_SOURCE_CALLBACK_TEST_H

#include <gtest/gtest.h>

#include "add_white_list_infos_call_back_stub.h"
#include "constants_dinput.h"
#include "del_white_list_infos_call_back_stub.h"
#include "distributed_input_source_proxy.h"
#include "distributed_input_source_stub.h"
#include "input_node_listener_stub.h"
#include "prepare_d_input_call_back_stub.h"
#include "register_d_input_call_back_stub.h"
#include "start_d_input_call_back_stub.h"
#include "stop_d_input_call_back_stub.h"
#include "unprepare_d_input_call_back_stub.h"
#include "unregister_d_input_call_back_stub.h"
#include "start_stop_d_inputs_call_back_stub.h"
#include "start_stop_result_call_back_stub.h"
#include "simulation_event_listener_stub.h"
#include "register_session_state_callback_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DInputSourceCallBackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class TestDInputSourceCallBackStub : public
        OHOS::DistributedHardware::DistributedInput::DistributedInputSourceStub {
    public:
        TestDInputSourceCallBackStub() = default;
        virtual ~TestDInputSourceCallBackStub() = default;
        int32_t Init();

        int32_t Release() ;

        int32_t RegisterDistributedHardware(
            const std::string &devId, const std::string &dhId, const std::string &parameters,
            sptr<IRegisterDInputCallback> callback);

        int32_t UnregisterDistributedHardware(
            const std::string &devId, const std::string &dhId,
            sptr<IUnregisterDInputCallback> callback);

        int32_t PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> callback);

        int32_t UnprepareRemoteInput(const std::string &deviceId, sptr<IUnprepareDInputCallback> callback);

        int32_t StartRemoteInput(
            const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback);

        int32_t StopRemoteInput(
            const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback);

        int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
            sptr<IStartDInputCallback> callback);

        int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
            sptr<IStopDInputCallback> callback);

        int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
            sptr<IPrepareDInputCallback> callback);

        int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
            sptr<IUnprepareDInputCallback> callback);

        int32_t StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
            sptr<IStartStopDInputsCallback> callback);

        int32_t StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
            sptr<IStartStopDInputsCallback> callback);

        int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId,
            const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback);

        int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId,
            const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback);

        int32_t RegisterAddWhiteListCallback(sptr<IAddWhiteListInfosCallback> addWhiteListCallback);
        int32_t RegisterDelWhiteListCallback(sptr<IDelWhiteListInfosCallback> delWhiteListCallback);

        int32_t RegisterSimulationEventListener(sptr<ISimulationEventListener> listener);
        int32_t UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener);
        int32_t RegisterSessionStateCb(sptr<ISessionStateCallback> callback);
        int32_t UnregisterSessionStateCb();
        void OnResult(const std::string &deviceId, const std::string &strJson);
        std::string deviceId_;
        std::string strJson_;
    };

    class TestDInputRegisterCallBack : public
        OHOS::DistributedHardware::DistributedInput::RegisterDInputCallbackStub {
    public:
        TestDInputRegisterCallBack() = default;
        virtual ~TestDInputRegisterCallBack() = default;
        void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status);
    };

    class TestDInputUnregisterCallBack : public
        OHOS::DistributedHardware::DistributedInput::UnregisterDInputCallbackStub {
    public:
        TestDInputUnregisterCallBack() = default;
        virtual ~TestDInputUnregisterCallBack() = default;
        void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status);
    };

    class TestDInputPrepareCallBack : public
        OHOS::DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
    public:
        TestDInputPrepareCallBack() = default;
        virtual ~TestDInputPrepareCallBack() = default;
        void OnResult(const std::string &deviceId, const int32_t &status);
    };

    class TestDInputUnprepareCallBack : public
        OHOS::DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
    public:
        TestDInputUnprepareCallBack() = default;
        virtual ~TestDInputUnprepareCallBack() = default;
        void OnResult(const std::string &deviceId, const int32_t &status);
    };

    class TestDInputStartCallBack : public
        OHOS::DistributedHardware::DistributedInput::StartDInputCallbackStub {
    public:
        TestDInputStartCallBack() = default;
        virtual ~TestDInputStartCallBack() = default;
        void OnResult(const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status);
    };

    class TestDInputStopCallBack : public
        OHOS::DistributedHardware::DistributedInput::StopDInputCallbackStub {
    public:
        TestDInputStopCallBack() = default;
        virtual ~TestDInputStopCallBack() = default;
        void OnResult(const std::string &deviceId, const uint32_t &inputTypes, const int32_t &status);
    };

    class TestVectorStartStopCallBackStub
        : public OHOS::DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
    public:
        TestVectorStartStopCallBackStub() = default;
        virtual ~TestVectorStartStopCallBackStub() = default;
        void OnResultDhids(const std::string &devId, const int32_t &status);
    };

    class TestAddWhiteListInfosCallBack : public
        OHOS::DistributedHardware::DistributedInput::AddWhiteListInfosCallbackStub {
    public:
        TestAddWhiteListInfosCallBack() = default;
        virtual ~TestAddWhiteListInfosCallBack() = default;
        void OnResult(const std::string &deviceId, const std::string &strJson);
    };

    class TestDelWhiteListInfosCallBack : public
        OHOS::DistributedHardware::DistributedInput::DelWhiteListInfosCallbackStub {
    public:
        TestDelWhiteListInfosCallBack() = default;
        virtual ~TestDelWhiteListInfosCallBack() = default;
        void OnResult(const std::string &deviceId);
    };

    class TestDInputNodeListenerCallBack : public
        OHOS::DistributedHardware::DistributedInput::InputNodeListenerStub {
    public:
        TestDInputNodeListenerCallBack() = default;
        virtual ~TestDInputNodeListenerCallBack() = default;
        void OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId,
            const std::string &sinkNodeId, const std::string &sinkNodeDesc);

        void OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId,
            const std::string &sinkNodeId);
    };

    class TestDInputSimulationEventCallBack : public
        OHOS::DistributedHardware::DistributedInput::SimulationEventListenerStub {
    public:
        TestDInputSimulationEventCallBack() = default;
        virtual ~TestDInputSimulationEventCallBack() = default;
        int32_t OnSimulationEvent(uint32_t type, uint32_t code, int32_t value);
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DINPUT_SOURCE_CALLBACK_TEST_H