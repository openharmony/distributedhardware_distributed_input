/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef WHITE_LIST_UTIL_H
#define WHITE_LIST_UTIL_H

#include <mutex>
#include <unordered_set>

#include "constants_dinput.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
using TYPE_KEY_CODE_VEC = std::vector<int32_t>;
using TYPE_COMBINATION_KEY_VEC = std::vector<TYPE_KEY_CODE_VEC>;
using TYPE_WHITE_LIST_VEC = std::vector<TYPE_COMBINATION_KEY_VEC>;
using TYPE_DEVICE_WHITE_LIST_MAP = std::map<std::string, TYPE_WHITE_LIST_VEC>;

struct WhiteListItemHash {
    std::string hash;

    // the keys num
    int32_t len;

    WhiteListItemHash() : hash(""), len(0) {}

    WhiteListItemHash(std::string &hash, int32_t len) : hash(hash), len(len) {}
};

class WhiteListUtil {
public:
    static WhiteListUtil &GetInstance(void);
    int32_t SyncWhiteList(const std::string &deviceId, const TYPE_WHITE_LIST_VEC &vecWhiteList);
    int32_t ClearWhiteList(const std::string &deviceId);
    int32_t ClearWhiteList(void);
    int32_t GetWhiteList(const std::string &deviceId, TYPE_WHITE_LIST_VEC &vecWhiteList);

    /*
     * check is event in white list of deviceId
     *
     * Return:
     *   true: event in white list of this device
     *   false: event not in white list of this device, or white list of this device not exist
     */
    bool IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event);
private:
    int32_t Init();
    WhiteListUtil();
    ~WhiteListUtil();
    WhiteListUtil(const WhiteListUtil &other) = delete;
    const WhiteListUtil &operator=(const WhiteListUtil &other) = delete;
    void ReadLineDataStepOne(std::string &column, TYPE_KEY_CODE_VEC &vecKeyCode,
        TYPE_COMBINATION_KEY_VEC &vecCombinationKey) const;
    void GetCombKeysHash(TYPE_COMBINATION_KEY_VEC combKeys, std::unordered_set<std::string> &targetSet);
    void GetAllComb(TYPE_COMBINATION_KEY_VEC vecs, WhiteListItemHash hash,
        int32_t targetLen, std::unordered_set<std::string> &hashSets);
    std::string GetBusinessEventHash(const BusinessEvent &event);
    bool IsValidLine(const std::string &line) const;
    bool CheckIsNumber(const std::string &str) const;
    void SplitCombinationKey(std::string &line, TYPE_KEY_CODE_VEC &vecKeyCode,
        TYPE_COMBINATION_KEY_VEC &vecCombinationKey) const;
private:
    TYPE_DEVICE_WHITE_LIST_MAP mapDeviceWhiteList_;
    std::map<std::string, std::unordered_set<std::string>> combKeysHashMap_;
    std::mutex mutex_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // WHITE_LIST_UTIL_H
