/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "dinputreleasesource_fuzzer.h"

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
void ReleaseSourceFuzzTest(const uint8_t *data, size_t size)
{
    (void)data;
    (void)size;
    DistributedInput::DistributedInputSourceHandler::GetInstance().ReleaseSource();
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::ReleaseSourceFuzzTest(data, size);
    return 0;
}