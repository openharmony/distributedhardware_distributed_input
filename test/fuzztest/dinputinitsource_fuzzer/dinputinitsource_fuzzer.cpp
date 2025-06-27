/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "dinputinitsource_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>

#include <refbase.h>

#include "constants_dinput.h"
#include "distributed_input_handler.h"
#include "distributed_input_kit.h"
#include "distributed_input_source_handler.h"
#include "i_distributed_source_input.h"

namespace OHOS {
namespace DistributedHardware {
void InitSourceFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    std::string params(reinterpret_cast<const char*>(data), size);
    DistributedInput::DistributedInputSourceHandler::GetInstance().InitSource(params);
}

void OnLoadSystemAbilitySuccessFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    int32_t saId = *(reinterpret_cast<const int32_t*>(data));
    DistributedInput::DistributedInputSourceHandler::SALoadSourceCb saLoadSourceCb;
    saLoadSourceCb.OnLoadSystemAbilitySuccess(saId, nullptr);
}

void OnLoadSystemAbilityFailFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    int32_t saId = *(reinterpret_cast<const int32_t*>(data));
    DistributedInput::DistributedInputSourceHandler::SALoadSourceCb saLoadSourceCb;
    saLoadSourceCb.OnLoadSystemAbilityFail(saId);
}

void OnRemoteSourceSvrDiedFuzzTest(const uint8_t *data, size_t size)
{
    (void)data;
    (void)size;
    DistributedInput::DistributedInputSourceHandler::GetInstance().OnRemoteSourceSvrDied(nullptr);
    DistributedInput::DistributedInputSourceHandler::GetInstance().RegisterDistributedHardwareStateListener(nullptr);
    DistributedInput::DistributedInputSourceHandler::GetInstance().UnregisterDistributedHardwareStateListener();
    DistributedInput::DistributedInputSourceHandler::GetInstance().RegisterDataSyncTriggerListener(nullptr);
    DistributedInput::DistributedInputSourceHandler::GetInstance().UnregisterDataSyncTriggerListener();
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::InitSourceFuzzTest(data, size);
    OHOS::DistributedHardware::OnLoadSystemAbilitySuccessFuzzTest(data, size);
    OHOS::DistributedHardware::OnLoadSystemAbilityFailFuzzTest(data, size);
    OHOS::DistributedHardware::OnRemoteSourceSvrDiedFuzzTest(data, size);
    return 0;
}