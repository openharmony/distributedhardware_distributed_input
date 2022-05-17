/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "distributed_input_kit_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "i_distributed_sink_input.h"
#include "i_distributed_source_input.h"
#include "distributed_input_kit.h"
#include "distributed_input_sink_handler.h"
#include "distributed_input_source_handler.h"
#include "distributed_input_handler.h"

#include "prepare_d_input_call_back_stub.h"
#include "unprepare_d_input_call_back_stub.h"
#include "start_d_input_call_back_stub.h"
#include "stop_d_input_call_back_stub.h"

#include "constants_dinput.h"

#include <thread>
#include <functional>
#include <iostream>
#include <refbase.h>

namespace OHOS {
namespace DistributedHardware {
class TestPrepareDInputCallback :
public OHOS::DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
public:
    TestPrepareDInputCallback() = default;
    virtual ~TestPrepareDInputCallback() = default;
    void OnResult(const std::string& deviceId, const int32_t& status) {};
};

class TestUnprepareDInputCallback :
public OHOS::DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
public:
    TestUnprepareDInputCallback() = default;
    virtual ~TestUnprepareDInputCallback() = default;
    void OnResult(const std::string& deviceId, const int32_t& status) {};
};

class TestStartDInputCallback :
public OHOS::DistributedHardware::DistributedInput::StartDInputCallbackStub {
public:
    void OnResult(const std::string& devId, const uint32_t& inputTypes, const int32_t& status) {};
};

class TestStopDInputCallback :
public OHOS::DistributedHardware::DistributedInput::StopDInputCallbackStub {
public:
    TestStopDInputCallback() = default;
    virtual ~TestStopDInputCallback() = default;
    void OnResult(const std::string& devId, const uint32_t& inputTypes, const int32_t& status) {};
};

void PrepareInputFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    std::string networkId(reinterpret_cast<const char*>(data), size);

    OHOS::sptr<TestPrepareDInputCallback> prepareCb = new(std::nothrow) TestPrepareDInputCallback();
    OHOS::sptr<TestUnprepareDInputCallback> unprepareCb = new(std::nothrow) TestUnprepareDInputCallback();
    DistributedInput::DistributedInputKit::PrepareRemoteInput(networkId, prepareCb);
    DistributedInput::DistributedInputKit::UnprepareRemoteInput(networkId, unprepareCb);
}

void StartRemoteInputFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    std::string dhId(reinterpret_cast<const char*>(data), size);
    uint32_t inputTypes = *(reinterpret_cast<const uint32_t*>(data));

    OHOS::sptr<TestStartDInputCallback> startCb = new(std::nothrow) TestStartDInputCallback();
    OHOS::sptr<TestStopDInputCallback> stopCb = new(std::nothrow) TestStopDInputCallback();
    DistributedInput::DistributedInputKit::StartRemoteInput(dhId, inputTypes, startCb);
    DistributedInput::DistributedInputKit::StopRemoteInput(dhId, inputTypes, stopCb);
}

void IsNeedFilterOutFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    std::string deviceId(reinterpret_cast<const char*>(data), size);
    int32_t pressedKey = *(reinterpret_cast<const int32_t*>(data));
    int32_t keyCode = *(reinterpret_cast<const int32_t*>(data));
    int32_t keyAction = *(reinterpret_cast<const int32_t*>(data));
    DistributedInput::BusinessEvent event;
    event.pressedKeys.push_back(pressedKey);
    event.keyAction = keyCode;
    event.keyAction = keyAction;

    DistributedInput::DistributedInputKit::IsNeedFilterOut(deviceId, event);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::PrepareInputFuzzTest(data, size);
    OHOS::DistributedHardware::StartRemoteInputFuzzTest(data, size);
    OHOS::DistributedHardware::IsNeedFilterOutFuzzTest(data, size);
    return 0;
}