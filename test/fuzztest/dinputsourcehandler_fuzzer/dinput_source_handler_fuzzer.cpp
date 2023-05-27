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

#include "dinput_source_handler_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>

#include <refbase.h>

#include "constants_dinput.h"
#include "distributed_input_handler.h"
#include "distributed_input_kit.h"
#include "distributed_input_sink_handler.h"
#include "distributed_input_source_handler.h"
#include "i_distributed_sink_input.h"
#include "i_distributed_source_input.h"

namespace OHOS {
namespace DistributedHardware {
class TestRegisterInputCallback : public OHOS::DistributedHardware::RegisterCallback {
public:
    TestRegisterInputCallback() = default;
    virtual ~TestRegisterInputCallback() = default;
    int32_t OnRegisterResult(const std::string &devId, const std::string &dhId, int32_t status,
        const std::string &data) override
    {
        return 0;
    }
};

class TestUnregisterInputCallback : public OHOS::DistributedHardware::UnregisterCallback {
public:
    TestUnregisterInputCallback() = default;
    virtual ~TestUnregisterInputCallback() = default;
    int32_t OnUnregisterResult(const std::string &devId, const std::string &dhId, int32_t status,
        const std::string &data) override
    {
        return 0;
    }
};

void InitSourceFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    std::string params(reinterpret_cast<const char*>(data), size);
    DistributedInput::DistributedInputSourceHandler::GetInstance().InitSource(params);
}

void ReleaseSourceFuzzTest(const uint8_t* data, size_t size)
{
    (void)data;
    (void)size;
    DistributedInput::DistributedInputSourceHandler::GetInstance().ReleaseSource();
}

void ConfigDistributedHardwareFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string key(reinterpret_cast<const char*>(data), size);
    std::string value(reinterpret_cast<const char*>(data), size);
    DistributedInput::DistributedInputSourceHandler::GetInstance().ConfigDistributedHardware(devId, dhId, key, value);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::InitSourceFuzzTest(data, size);
    OHOS::DistributedHardware::ReleaseSourceFuzzTest(data, size);
    OHOS::DistributedHardware::ConfigDistributedHardwareFuzzTest(data, size);
    return 0;
}