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

#include "white_list_util.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>

#include "config_policy_utils.h"

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    const char * const SPLIT_LINE = "|";
    const char * const SPLIT_COMMA = ",";
    const char * const WHITELIST_FILE_PATH = "etc/distributedhardware/dinput_business_event_whitelist.cfg";
    const int32_t COMB_KEY_VEC_MIN_LEN = 2;
    const int32_t LAST_KEY_ACTION_LEN = 1;
    const int32_t LAST_KEY_LEN = 1;
    const int32_t MAX_LINE_NUM = 100;
    const int32_t MAX_CHAR_PER_LINE_NUM = 100;
    const int32_t MAX_SPLIT_COMMA_NUM = 4;
    const int32_t MAX_SPLIT_LINE_NUM = 12;
    const int32_t MAX_KEY_CODE_NUM = 4;
}

WhiteListUtil::WhiteListUtil()
{
    DHLOGI("Ctor WhiteListUtil.");
    Init();
}

WhiteListUtil::~WhiteListUtil()
{
    DHLOGI("Dtor WhiteListUtil.");
}

WhiteListUtil &WhiteListUtil::GetInstance(void)
{
    static WhiteListUtil instance;
    return instance;
}

int32_t WhiteListUtil::Init()
{
    char buf[MAX_PATH_LEN] = {0};
    char path[PATH_MAX + 1] = {0x00};
    char *whiteListFilePath = GetOneCfgFile(WHITELIST_FILE_PATH, buf, MAX_PATH_LEN);
    if (strlen(whiteListFilePath) == 0 || strlen(whiteListFilePath) > PATH_MAX ||
        realpath(whiteListFilePath, path) == nullptr) {
        DHLOGE("File connicailization failed.");
        return ERR_DH_INPUT_WHILTELIST_INIT_FAIL;
    }
    std::ifstream inFile(path, std::ios::in | std::ios::binary);
    if (!inFile.is_open()) {
        DHLOGE("WhiteListUtil Init error, file open fail path=%{public}s", path);
        return ERR_DH_INPUT_WHILTELIST_INIT_FAIL;
    }

    TYPE_KEY_CODE_VEC vecKeyCode;
    TYPE_COMBINATION_KEY_VEC vecCombinationKey;
    TYPE_WHITE_LIST_VEC vecWhiteList;
    std::string line;
    std::size_t lineNum = 0;
    while (getline(inFile, line)) {
        if ((++lineNum > MAX_LINE_NUM) || !IsValidLine(line)) {
            DHLOGE("whitelist cfg file has too many lines or too complicated. lineNum is %{public}zu", lineNum);
            break;
        }
        vecKeyCode.clear();
        vecCombinationKey.clear();
        SplitCombinationKey(line, vecKeyCode, vecCombinationKey);

        if (CheckIsNumber(line)) {
            int32_t keyCode = std::stoi(line);
            if (keyCode != 0) {
                vecKeyCode.push_back(keyCode);
            }
            if (!vecKeyCode.empty()) {
                vecCombinationKey.push_back(vecKeyCode);
                vecKeyCode.clear();
            }
        }

        if (!vecCombinationKey.empty()) {
            vecWhiteList.push_back(vecCombinationKey);
            vecCombinationKey.clear();
        }
    }
    inFile.close();
    std::string localNetworkId = GetLocalDeviceInfo().networkId;
    if (!localNetworkId.empty()) {
        SyncWhiteList(localNetworkId, vecWhiteList);
    }
    return DH_SUCCESS;
}

bool WhiteListUtil::IsValidLine(const std::string &line) const
{
    if (line.size() > MAX_CHAR_PER_LINE_NUM) {
        DHLOGE("This line is too long, size is %{public}zu", line.size());
        return false;
    }
    if (std::count(line.begin(), line.end(), SPLIT_COMMA[0]) > MAX_SPLIT_COMMA_NUM) {
        DHLOGE("This line %{public}s has too many SPLIT_COMMA", line.c_str());
        return false;
    }
    if (std::count(line.begin(), line.end(), SPLIT_LINE[0]) > MAX_SPLIT_LINE_NUM) {
        DHLOGE("This line %{public}s has too many SPLIT_LINE", line.c_str());
        return false;
    }
    return true;
}

bool WhiteListUtil::CheckIsNumber(const std::string &str) const
{
    if (str.empty() || str.size() > MAX_KEY_CODE_NUM) {
        DHLOGE("KeyCode size %{public}zu, is zero or too long.", str.size());
        return false;
    }
    for (char const &c : str) {
        if (std::isdigit(c) == 0) {
            DHLOGE("Check KeyCode format fail, %{public}s.", str.c_str());
            return false;
        }
    }
    return true;
}

void WhiteListUtil::ReadLineDataStepOne(std::string &column, TYPE_KEY_CODE_VEC &vecKeyCode,
    TYPE_COMBINATION_KEY_VEC &vecCombinationKey) const
{
    std::size_t pos2 = column.find(SPLIT_LINE);
    while (pos2 != std::string::npos) {
        std::string single = column.substr(0, pos2);
        column = column.substr(pos2 + 1, column.size());
        pos2 = column.find(SPLIT_LINE);

        if (CheckIsNumber(single)) {
            int32_t keyCode = std::stoi(single);
            if (keyCode != 0) {
                vecKeyCode.push_back(keyCode);
            }
        }
    }

    if (CheckIsNumber(column)) {
        int32_t keyCode = std::stoi(column);
        if (keyCode != 0) {
            vecKeyCode.push_back(keyCode);
        }
    }

    if (!vecKeyCode.empty()) {
        vecCombinationKey.push_back(vecKeyCode);
        vecKeyCode.clear();
    }
}

void WhiteListUtil::SplitCombinationKey(std::string &line, TYPE_KEY_CODE_VEC &vecKeyCode,
    TYPE_COMBINATION_KEY_VEC &vecCombinationKey) const
{
    std::size_t pos1 = line.find(SPLIT_COMMA);
    while (pos1 != std::string::npos) {
        std::string column = line.substr(0, pos1);
        line = line.substr(pos1 + 1, line.size());
        pos1 = line.find(SPLIT_COMMA);
        vecKeyCode.clear();
        ReadLineDataStepOne(column, vecKeyCode, vecCombinationKey);
    }
}

int32_t WhiteListUtil::SyncWhiteList(const std::string &deviceId, const TYPE_WHITE_LIST_VEC &vecWhiteList)
{
    DHLOGI("deviceId=%{public}s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    mapDeviceWhiteList_[deviceId] = vecWhiteList;
    for (auto combKeys : vecWhiteList) {
        GetCombKeysHash(combKeys, combKeysHashMap_[deviceId]);
    }

    return DH_SUCCESS;
}

void WhiteListUtil::GetCombKeysHash(TYPE_COMBINATION_KEY_VEC combKeys, std::unordered_set<std::string> &targetSet)
{
    if (combKeys.size() < COMB_KEY_VEC_MIN_LEN) {
        DHLOGE("white list item length invalid");
        return;
    }

    TYPE_KEY_CODE_VEC lastKeyAction = combKeys.back();
    if (lastKeyAction.size() != LAST_KEY_ACTION_LEN) {
        DHLOGE("last key action invalid");
        return;
    }
    combKeys.pop_back();
    TYPE_KEY_CODE_VEC lastKey = combKeys.back();
    if (lastKey.size() != LAST_KEY_LEN) {
        DHLOGE("last key invalid");
        return;
    }
    combKeys.pop_back();

    std::unordered_set<std::string> hashSets;
    WhiteListItemHash hash;
    GetAllComb(combKeys, hash, combKeys.size(), hashSets);

    for (const auto &hashSet : hashSets) {
        targetSet.insert(hashSet + std::to_string(lastKey[0]) + std::to_string(lastKeyAction[0]));
    }
}

void WhiteListUtil::GetAllComb(TYPE_COMBINATION_KEY_VEC vecs, WhiteListItemHash hash,
    int32_t targetLen, std::unordered_set<std::string> &hashSets)
{
    for (size_t i = 0; i < vecs.size(); i++) {
        TYPE_KEY_CODE_VEC nowVec = vecs[i];
        for (int32_t code : nowVec) {
            std::string hashStr = hash.hash + std::to_string(code);
            WhiteListItemHash newHash(hashStr, hash.len + 1);
            TYPE_COMBINATION_KEY_VEC leftVecs = vecs;
            leftVecs.erase(leftVecs.begin() + i);
            GetAllComb(leftVecs, newHash, targetLen, hashSets);
        }
    }

    if (hash.len == targetLen) {
        hashSets.insert(hash.hash);
    }
}

int32_t WhiteListUtil::ClearWhiteList(const std::string &deviceId)
{
    DHLOGI("deviceId=%{public}s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    mapDeviceWhiteList_.erase(deviceId);
    return DH_SUCCESS;
}

int32_t WhiteListUtil::ClearWhiteList(void)
{
    std::lock_guard<std::mutex> lock(mutex_);
    TYPE_DEVICE_WHITE_LIST_MAP().swap(mapDeviceWhiteList_);
    return DH_SUCCESS;
}

int32_t WhiteListUtil::GetWhiteList(const std::string &deviceId, TYPE_WHITE_LIST_VEC &vecWhiteList)
{
    DHLOGI("GetWhiteList start, deviceId=%{public}s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    TYPE_DEVICE_WHITE_LIST_MAP::const_iterator iter = mapDeviceWhiteList_.find(deviceId);
    if (iter != mapDeviceWhiteList_.end()) {
        vecWhiteList = iter->second;
        DHLOGI("GetWhiteList success, deviceId=%{public}s", GetAnonyString(deviceId).c_str());
        return DH_SUCCESS;
    }

    DHLOGI("GetWhiteList fail, deviceId=%{public}s", GetAnonyString(deviceId).c_str());
    return ERR_DH_INPUT_WHILTELIST_GET_WHILTELIST_FAIL;
}

std::string WhiteListUtil::GetBusinessEventHash(const BusinessEvent &event)
{
    std::string hash = "";
    for (const auto &p : event.pressedKeys) {
        hash += std::to_string(p);
    }
    hash += std::to_string(event.keyCode);
    hash += std::to_string(event.keyAction);

    return hash;
}

bool WhiteListUtil::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    DHLOGI("IsNeedFilterOut start, deviceId=%{public}s", GetAnonyString(deviceId).c_str());

    std::lock_guard<std::mutex> lock(mutex_);
    if (combKeysHashMap_.empty()) {
        DHLOGE("IsNeedFilterOut error, white list is empty!");
        return false;
    }

    auto iter = combKeysHashMap_.find(deviceId);
    if (iter == combKeysHashMap_.end()) {
        DHLOGE("IsNeedFilterOut error, not find by deviceId!");
        return false;
    }

    std::string hash = GetBusinessEventHash(event);
    DHLOGI("Searched business event hash: %{public}s", hash.c_str());

    return combKeysHashMap_[deviceId].find(hash) != combKeysHashMap_[deviceId].end();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
