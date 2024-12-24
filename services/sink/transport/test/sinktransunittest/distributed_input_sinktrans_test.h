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

#ifndef DISRIBUTED_INPUT_SINKTRANS_TEST_H
#define DISRIBUTED_INPUT_SINKTRANS_TEST_H

#include <functional>
#include <iostream>
#include <refbase.h>
#include <thread>

#include <gtest/gtest.h>

#include "constants_dinput.h"
#include "distributed_input_sink_transport.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSinkTransTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    nlohmann::json jsonStr_;
    nlohmann::json jsonStr1_;
    nlohmann::json jsonStr2_;
    nlohmann::json jsonStr3_;
    nlohmann::json jsonStr4_;
    nlohmann::json jsonStr5_;
    nlohmann::json jsonStr6_;
    nlohmann::json jsonStr7_;
    nlohmann::json jsonStr8_;
    nlohmann::json jsonStr9_;
    nlohmann::json jsonStr10_;
    nlohmann::json jsonStr11_;
    nlohmann::json jsonStr12_;
    nlohmann::json jsonStr13_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISRIBUTED_INPUT_SINKTRANS_TEST_H
