/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "distributed_input_sink_transport_fuzzer.h"

#include <functional>
#include <iostream>
#include <thread>

#include <refbase.h>

#include "constants_dinput.h"
#include "distributed_input_sink_transport.h"

namespace OHOS {
namespace DistributedHardware {
void RespPrepareRemoteInputFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    int32_t sessionId = *(reinterpret_cast<const int32_t*>(data));
    std::string smsg(reinterpret_cast<const char*>(data), size);

    DistributedInput::DistributedInputSinkTransport::GetInstance().RespPrepareRemoteInput(sessionId, smsg);
    DistributedInput::DistributedInputSinkTransport::GetInstance().RespUnprepareRemoteInput(sessionId, smsg);
}

void RespStartRemoteInputFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    int32_t sessionId = *(reinterpret_cast<const int32_t*>(data));
    std::string smsg(reinterpret_cast<const char*>(data), size);

    DistributedInput::DistributedInputSinkTransport::GetInstance().RespStartRemoteInput(sessionId, smsg);
    DistributedInput::DistributedInputSinkTransport::GetInstance().RespStopRemoteInput(sessionId, smsg);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::RespPrepareRemoteInputFuzzTest(data, size);
    OHOS::DistributedHardware::RespStartRemoteInputFuzzTest(data, size);
    return 0;
}