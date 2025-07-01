/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "dinput_permission_check_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <unistd.h>
#include <refbase.h>

#include "constants_dinput.h"
#include "distributed_input_transport_base.h"
#include "softbus_permission_check.h"

namespace OHOS {
namespace DistributedHardware {
void CheckSinkPermissionFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t) + sizeof(uint64_t)))) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    uint64_t tokenId = fdp.ConsumeIntegral<uint64_t>();
    std::string accountId = fdp.ConsumeRandomLengthString();
    std::string networkId = fdp.ConsumeRandomLengthString();
    DistributedInput::AccountInfo callerAccountInfo = {
        .userId_ = userId,
        .tokenId_ = tokenId,
        .accountId_ = accountId,
        .networkId_ = networkId,
    };
    DistributedInput::SoftBusPermissionCheck::CheckSinkPermission(callerAccountInfo);
}

void CheckSrcAccessControlFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    std::string sinkNetworkId(reinterpret_cast<const char*>(data), size);
    DistributedInput::AccountInfo localAccountInfo;
    DistributedInput::SoftBusPermissionCheck::CheckSrcAccessControl(sinkNetworkId, localAccountInfo);
}

void CheckSinkAccessControlFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t) + sizeof(uint64_t)))) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    uint64_t tokenId = fdp.ConsumeIntegral<uint64_t>();
    std::string accountId = fdp.ConsumeRandomLengthString();
    std::string networkId = fdp.ConsumeRandomLengthString();
    DistributedInput::AccountInfo callerAccountInfo = {
        .userId_ = userId,
        .tokenId_ = tokenId,
        .accountId_ = accountId,
        .networkId_ = networkId,
    };
    DistributedInput::AccountInfo calleeAccountInfo = {
        .userId_ = userId,
        .tokenId_ = tokenId,
        .accountId_ = accountId,
        .networkId_ = networkId,
    };
    DistributedInput::SoftBusPermissionCheck::CheckSinkAccessControl(callerAccountInfo, calleeAccountInfo);
}

void GetLocalNetworkIdFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    std::string networkId(reinterpret_cast<const char*>(data), size);
    DistributedInput::SoftBusPermissionCheck::GetLocalNetworkId(networkId);
}

void TransCallerInfoOneFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t) + sizeof(uint64_t)))) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    uint64_t tokenId = fdp.ConsumeIntegral<uint64_t>();
    std::string accountId = fdp.ConsumeRandomLengthString();
    std::string accessInfo = "{\"accountId\":\"123\"}";
    SocketAccessInfo callerInfo = {
        .userId = userId,
        .localTokenId = tokenId,
        .businessAccountId = const_cast<char*>(accountId.c_str()),
        .extraAccessInfo = const_cast<char*>(accessInfo.c_str()),
    };
    DistributedInput::AccountInfo callerAccountInfo;
    std::string networkId = fdp.ConsumeRandomLengthString();
    DistributedInput::SoftBusPermissionCheck::TransCallerInfo(&callerInfo, callerAccountInfo, networkId);
}

void TransCallerInfoSecondFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t) + sizeof(uint64_t)))) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    uint64_t tokenId = fdp.ConsumeIntegral<uint64_t>();
    std::string accountId = fdp.ConsumeRandomLengthString();
    std::string accessInfo = fdp.ConsumeRandomLengthString();
    SocketAccessInfo callerInfo = {
        .userId = userId,
        .localTokenId = tokenId,
        .businessAccountId = const_cast<char*>(accountId.c_str()),
        .extraAccessInfo = const_cast<char*>(accessInfo.c_str()),
    };
    DistributedInput::AccountInfo callerAccountInfo;
    std::string networkId = fdp.ConsumeRandomLengthString();
    DistributedInput::SoftBusPermissionCheck::TransCallerInfo(&callerInfo, callerAccountInfo, networkId);
}

void FillLocalInfoOneFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    SocketAccessInfo *localInfo = nullptr;
    DistributedInput::SoftBusPermissionCheck::FillLocalInfo(localInfo);
}

void FillLocalInfoSecondFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t) + sizeof(uint64_t)))) {
        return;
    }
    FuzzedDataProvider fdp(data, size);
    int32_t userId = fdp.ConsumeIntegral<int32_t>();
    uint64_t tokenId = fdp.ConsumeIntegral<uint64_t>();
    SocketAccessInfo localInfo = {
        .userId = userId,
        .localTokenId = tokenId,
    };
    DistributedInput::SoftBusPermissionCheck::FillLocalInfo(&localInfo);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::CheckSinkPermissionFuzzTest(data, size);
    OHOS::DistributedHardware::CheckSrcAccessControlFuzzTest(data, size);
    OHOS::DistributedHardware::CheckSinkAccessControlFuzzTest(data, size);
    OHOS::DistributedHardware::GetLocalNetworkIdFuzzTest(data, size);
    OHOS::DistributedHardware::TransCallerInfoOneFuzzTest(data, size);
    OHOS::DistributedHardware::TransCallerInfoSecondFuzzTest(data, size);
    OHOS::DistributedHardware::FillLocalInfoOneFuzzTest(data, size);
    OHOS::DistributedHardware::FillLocalInfoSecondFuzzTest(data, size);
    return 0;
}