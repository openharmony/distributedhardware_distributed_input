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

#ifndef DISRIBUTED_INPUT_SINKMANAGER_TEST_H
#define DISRIBUTED_INPUT_SINKMANAGER_TEST_H


#include <functional>
#include <iostream>
#include <refbase.h>
#include <thread>

#include <gtest/gtest.h>

#include "constants_dinput.h"
#include "distributed_input_sink_manager.h"
#include "get_sink_screen_infos_call_back_stub.h"
#include "sharing_dhid_listener_stub.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSinkManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    class TestGetSinkScreenInfosCb : public
        OHOS::DistributedHardware::DistributedInput::GetSinkScreenInfosCallbackStub {
    public:
        TestGetSinkScreenInfosCb() = default;
        virtual ~TestGetSinkScreenInfosCb() = default;
        void OnResult(const std::string &strJson) override;
    };

    class TestSharingDhIdListenerStub : public
        OHOS::DistributedHardware::DistributedInput::SharingDhIdListenerStub {
    public:
        TestSharingDhIdListenerStub() = default;
        virtual ~TestSharingDhIdListenerStub() = default;
        int32_t OnSharing(const std::string &dhId) override;
        int32_t OnNoSharing(const std::string &dhId) override;
    };
private:
    DistributedInputSinkManager* sinkManager_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_SINKMANAGER_TEST_H
