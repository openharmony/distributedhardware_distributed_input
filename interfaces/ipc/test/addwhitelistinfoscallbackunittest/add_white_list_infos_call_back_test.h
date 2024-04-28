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

#ifndef ADD_WHITE_LIST_INFOS_CALLBACK_TEST_H
#define ADD_WHITE_LIST_INFOS_CALLBACK_TEST_H

#include <gtest/gtest.h>

#include "add_white_list_infos_call_back_proxy.h"
#include "add_white_list_infos_call_back_stub.h"
#include "get_sink_screen_infos_call_back_proxy.h"
#include "get_sink_screen_infos_call_back_stub.h"
#include "distributed_input_sink_proxy.h"
#include "distributed_input_sink_stub.h"
#include "input_node_listener_proxy.h"
#include "input_node_listener_stub.h"
#include "prepare_d_input_call_back_proxy.h"
#include "prepare_d_input_call_back_stub.h"
#include "sharing_dhid_listener_proxy.h"
#include "sharing_dhid_listener_stub.h"
#include "simulation_event_listener_proxy.h"
#include "simulation_event_listener_stub.h"
#include "start_d_input_call_back_proxy.h"
#include "start_d_input_call_back_stub.h"
#include "start_stop_d_inputs_call_back_proxy.h"
#include "start_stop_d_inputs_call_back_stub.h"
#include "start_stop_result_call_back_proxy.h"
#include "start_stop_result_call_back_stub.h"
#include "stop_d_input_call_back_proxy.h"
#include "stop_d_input_call_back_stub.h"
#include "register_d_input_call_back_proxy.h"
#include "register_d_input_call_back_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class AddWhiteListInfosCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class TestAddWhiteListInfosCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::AddWhiteListInfosCallbackStub {
    public:
        TestAddWhiteListInfosCallbackStub() = default;
        virtual ~TestAddWhiteListInfosCallbackStub() = default;
        void OnResult(const std::string &deviceId, const std::string &strJson);
        std::string deviceId_;
        std::string strJson_;
    };

    class TestGetSinkScreenInfosCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::GetSinkScreenInfosCallbackStub {
    public:
        TestGetSinkScreenInfosCallbackStub() = default;
        virtual ~TestGetSinkScreenInfosCallbackStub() = default;
        void OnResult(const std::string &strJson);
        std::string strJson_;
    };

    class TestInputNodeListenerStub : public
        OHOS::DistributedHardware::DistributedInput::InputNodeListenerStub {
    public:
        TestInputNodeListenerStub() = default;
        virtual ~TestInputNodeListenerStub() = default;
        void OnNodeOnLine(const std::string &srcDevId, const std::string &sinkDevId,
            const std::string &sinkNodeId, const std::string &sinkNodeDesc);

        void OnNodeOffLine(const std::string &srcDevId, const std::string &sinkDevId,
            const std::string &sinkNodeId);
        std::string srcDevId_;
        std::string sinkDevId_;
        std::string sinkNodeId_;
        std::string sinkNodeDesc_;
    };

    class TestPrepareDInputCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
    public:
        TestPrepareDInputCallbackStub() = default;
        virtual ~TestPrepareDInputCallbackStub() = default;
        void OnResult(const std::string &devId, const int32_t &status);
        std::string devId_;
        int32_t status_ = 0;
    };

    class TestRegisterDInputCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::RegisterDInputCallbackStub {
    public:
        TestRegisterDInputCallbackStub() = default;
        virtual ~TestRegisterDInputCallbackStub() = default;
        void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status);
        std::string devId_;
        std::string dhId_;
        int32_t status_ = 0;
    };

    class TestSharingDhIdListenerStub : public
        OHOS::DistributedHardware::DistributedInput::SharingDhIdListenerStub {
    public:
        TestSharingDhIdListenerStub() = default;
        virtual ~TestSharingDhIdListenerStub() = default;
        int32_t OnSharing(const std::string &dhId);
        int32_t OnNoSharing(const std::string &dhId);
        std::string dhId_;
    };

    class TestSimulationEventListenerStub : public
        OHOS::DistributedHardware::DistributedInput::SimulationEventListenerStub {
    public:
        TestSimulationEventListenerStub() = default;
        virtual ~TestSimulationEventListenerStub() = default;
        int32_t OnSimulationEvent(uint32_t type, uint32_t code, int32_t value);
        uint32_t type_ = 0;
        uint32_t code_ = 0;
        int32_t value_ = 0;
    };

    class TestStartDInputCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::StartDInputCallbackStub {
    public:
        TestStartDInputCallbackStub() = default;
        virtual ~TestStartDInputCallbackStub() = default;
        void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status);
        std::string devId_;
        uint32_t inputTypes_ = 0;
        int32_t status_ = 0;
    };

    class TestStartStopDInputsCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
    public:
        TestStartStopDInputsCallbackStub() = default;
        virtual ~TestStartStopDInputsCallbackStub() = default;
        void OnResultDhids(const std::string &devId, const int32_t &status);
        std::string devId_;
        int32_t status_ = 0;
    };

    class TestStartStopResultCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::StartStopResultCallbackStub {
    public:
        TestStartStopResultCallbackStub() = default;
        virtual ~TestStartStopResultCallbackStub() = default;
        void OnStart(const std::string &srcId, const std::string &sinkId, std::vector<std::string> &dhIds);
        void OnStop(const std::string &srcId, const std::string &sinkId, std::vector<std::string> &dhIds);
        std::string srcId_;
        std::string sinkId_;
        std::vector<std::string> dhIds_;
    };

    class TestStopDInputCallbackStub : public
        OHOS::DistributedHardware::DistributedInput::StopDInputCallbackStub {
    public:
        TestStopDInputCallbackStub() = default;
        virtual ~TestStopDInputCallbackStub() = default;
        void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status);
        std::string devId_;
        uint32_t inputTypes_ = 0;
        int32_t status_ = 0;
    };

    class TestDistributedInputSinkStub : public
        OHOS::DistributedHardware::DistributedInput::DistributedInputSinkStub {
    public:
        TestDistributedInputSinkStub() = default;
        virtual ~TestDistributedInputSinkStub() = default;
        int32_t Init();
        int32_t Release();
        int32_t RegisterGetSinkScreenInfosCallback(sptr<IGetSinkScreenInfosCallback> callback);
        int32_t NotifyStartDScreen(const SrcScreenInfo &remoteCtrlInfo);
        int32_t NotifyStopDScreen(const std::string &srcScreenInfoKey);
        int32_t RegisterSharingDhIdListener(sptr<ISharingDhIdListener> sharingDhIdListener);
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // ADD_WHITE_LIST_INFOS_CALLBACK_TEST_H