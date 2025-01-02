/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifndef DISRIBUTED_INPUT_COLLECTOR_TEST_H
#define DISRIBUTED_INPUT_COLLECTOR_TEST_H

#include <gtest/gtest.h>

#include "constants_dinput.h"
#include "distributed_input_collector.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputCollectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    class DInputSinkCollectorEventHandler : public AppExecFwk::EventHandler {
    public:
        explicit DInputSinkCollectorEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
        ~DInputSinkCollectorEventHandler() {}

        void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    };
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISRIBUTED_INPUT_COLLECTOR_TEST_H