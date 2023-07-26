/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "dinput_utils_tool.h"

#include <cstdarg>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>

#include "nlohmann/json.hpp"
#include "securec.h"

#ifndef COMPILE_TEST_MODE
#include "softbus_bus_center.h"
#else
#include "softbus_bus_center_mock.h"
#endif

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_softbus_define.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    constexpr int32_t HEX_TO_UINT8 = 2;
    constexpr size_t INT32_SHORT_ID_LENGTH = 20;
    constexpr size_t INT32_PLAINTEXT_LENGTH = 4;
    constexpr size_t INT32_MIN_ID_LENGTH = 3;
    constexpr int32_t WIDTH = 4;
    constexpr unsigned char MASK = 0x0F;
    constexpr int32_t DOUBLE_TIMES = 2;
    constexpr int32_t INT32_STRING_LENGTH = 40;
    constexpr int32_t MAX_RETRY_COUNT = 10;
    constexpr uint32_t SLEEP_TIME_US = 100 * 1000;
    constexpr uint32_t ERROR_MSG_MAX_LEN = 256;
}
DevInfo GetLocalDeviceInfo()
{
    DevInfo devInfo{"", "", 0};
    auto info = std::make_unique<NodeBasicInfo>();
    auto ret = GetLocalNodeDeviceInfo(DINPUT_PKG_NAME.c_str(), info.get());
    if (ret != 0) {
        DHLOGE("GetLocalNodeDeviceInfo failed, errCode = %d", ret);
        return devInfo;
    }

    devInfo.networkId = info->networkId;
    devInfo.deviceName = info->deviceName;
    devInfo.deviceType = info->deviceTypeId;

    return devInfo;
}

std::string GetLocalNetworkId()
{
    std::string localNetworkId = GetLocalDeviceInfo().networkId;
    if (localNetworkId.empty()) {
        DHLOGE("local networkId is empty!");
    }
    DHLOGI("GetLocalNetworkId, device local networkId is %s", GetAnonyString(localNetworkId).c_str());
    return localNetworkId;
}

std::string GetUUIDBySoftBus(const std::string &networkId)
{
    if (networkId.empty()) {
        return "";
    }
    char uuid[UUID_BUF_LEN] = {0};
    auto ret = GetNodeKeyInfo(DINPUT_PKG_NAME.c_str(), networkId.c_str(), NodeDeviceInfoKey::NODE_KEY_UUID,
        reinterpret_cast<uint8_t *>(uuid), UUID_BUF_LEN);
    return (ret == DH_SUCCESS) ? std::string(uuid) : "";
}

uint64_t GetCurrentTime()
{
    constexpr int32_t usOneSecond = 1000 * 1000;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * usOneSecond + tv.tv_usec;
}

std::string SetAnonyId(const std::string &message)
{
    nlohmann::json jsonObj = nlohmann::json::parse(message, nullptr, false);
    if (jsonObj.is_discarded()) {
        DHLOGE("jsonObj parse failed!");
        return "";
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_DEVICE_ID)) {
        jsonObj[DINPUT_SOFTBUS_KEY_DEVICE_ID] = GetAnonyString(jsonObj[DINPUT_SOFTBUS_KEY_DEVICE_ID]);
    }
    if (IsString(jsonObj, DESCRIPTOR)) {
        jsonObj[DESCRIPTOR] = GetAnonyString(jsonObj[DESCRIPTOR]);
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_INPUT_DATA)) {
        jsonObj[DINPUT_SOFTBUS_KEY_INPUT_DATA] = GetAnonyString(jsonObj[DINPUT_SOFTBUS_KEY_INPUT_DATA]);
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_VECTOR_DHID)) {
        std::string dhidStr = jsonObj[DINPUT_SOFTBUS_KEY_VECTOR_DHID];
        dhidStr.append(".");
        size_t pos = dhidStr.find(".");
        std::string anonyDhidStr = "";
        while (pos != dhidStr.npos) {
            anonyDhidStr += GetAnonyString(dhidStr.substr(0, pos)) + ".";
            dhidStr = dhidStr.substr(pos + 1, dhidStr.size());
            pos = dhidStr.find(".");
        }
        jsonObj[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = anonyDhidStr.substr(0, anonyDhidStr.length() - 1);
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_SRC_DEV_ID)) {
        jsonObj[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = GetAnonyString(jsonObj[DINPUT_SOFTBUS_KEY_SRC_DEV_ID]);
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_SINK_DEV_ID)) {
        jsonObj[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = GetAnonyString(jsonObj[DINPUT_SOFTBUS_KEY_SINK_DEV_ID]);
    }
    return jsonObj.dump();
}

bool IsBoolean(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_boolean();
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsString(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_string();
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsInt32(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_integer() && INT32_MIN <= jsonObj[key] &&
        jsonObj[key] <= INT32_MAX;
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsInt64(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_integer() && INT64_MIN <= jsonObj[key] &&
        jsonObj[key] <= INT64_MAX;
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsUInt16(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT16_MAX;
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsUInt32(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT32_MAX;
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsUInt64(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT64_MAX;
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsArray(const nlohmann::json& jsonObj, const std::string& key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_array();
    if (!res) {
        DHLOGE("The key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

std::string GetNodeDesc(std::string parameters)
{
    nlohmann::json parObj = nlohmann::json::parse(parameters, nullptr, false);
    if (parObj.is_discarded()) {
        DHLOGE("parObj parse failed!");
        return "";
    }
    std::string nodeName = "N/A";
    std::string physicalPath = "N/A";
    int32_t classes = -1;

    if (IsString(parObj, DEVICE_NAME) && IsString(parObj, PHYSICAL_PATH) && IsInt32(parObj, CLASSES)) {
        nodeName = parObj.at(DEVICE_NAME).get<std::string>();
        physicalPath = parObj.at(PHYSICAL_PATH).get<std::string>();
        classes = parObj.at(CLASSES).get<int32_t>();
    }

    return "{ nodeName: " + nodeName + ", physicalPath: " + physicalPath + ", classes: " +
        std::to_string(classes) + " }";
}

std::string GetAnonyString(const std::string &value)
{
    std::string res;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }

    if (strLen <= INT32_SHORT_ID_LENGTH) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        res.append(value, 0, INT32_PLAINTEXT_LENGTH);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }

    return res;
}

std::string GetAnonyInt32(const int32_t value)
{
    char tempBuffer[INT32_STRING_LENGTH] = "";
    int32_t secRet = sprintf_s(tempBuffer, INT32_STRING_LENGTH, "%d", value);
    if (secRet <= 0) {
        std::string nullString("");
        return nullString;
    }
    size_t length = strlen(tempBuffer);
    for (size_t i = 1; i <= length - 1; i++) {
        tempBuffer[i] = '*';
    }
    if (length == 0x01) {
        tempBuffer[0] = '*';
    }

    std::string tempString(tempBuffer);
    return tempString;
}

std::string Sha256(const std::string& in)
{
    unsigned char out[SHA256_DIGEST_LENGTH * HEX_TO_UINT8 + 1] = {0};
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, in.data(), in.size());
    SHA256_Final(&out[SHA256_DIGEST_LENGTH], &ctx);
    // here we translate sha256 hash to hexadecimal. each 8-bit char will be presented by two characters([0-9a-f])
    const char* hexCode = "0123456789abcdef";
    for (int32_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        unsigned char value = out[SHA256_DIGEST_LENGTH + i];
        // uint8_t is 2 digits in hexadecimal.
        out[i * DOUBLE_TIMES] = hexCode[(value >> WIDTH) & MASK];
        out[i * DOUBLE_TIMES + 1] = hexCode[value & MASK];
    }
    out[SHA256_DIGEST_LENGTH * DOUBLE_TIMES] = 0;
    return reinterpret_cast<char*>(out);
}

void CloseFd(int& fd)
{
    if (fd < 0) {
        DHLOGE("No fd need to beclosed.");
        return;
    }
    close(fd);
    fd = -1;
}

int BitIsSet(const unsigned long *array, int bit)
{
    return !!(array[bit / LONG_BITS] & (1LL << (bit % LONG_BITS)));
}

void StringSplitToVector(const std::string& str, const char split, std::vector<std::string>& vecStr)
{
    if (str.empty()) {
        DHLOGE("StringSplitToVector param str is error.");
        return;
    }
    std::string strTmp = str + split;
    size_t pos = strTmp.find(split);
    while (pos != strTmp.npos) {
        std::string matchTmp = strTmp.substr(0, pos);
        vecStr.push_back(matchTmp);
        strTmp = strTmp.substr(pos + 1, strTmp.size());
        pos = strTmp.find(split);
    }
}

int OpenInputDeviceFdByPath(std::string devicePath)
{
    chmod(devicePath.c_str(), S_IWRITE | S_IREAD);
    char* canonicalDevicePath = new char(PATH_MAX);
    if (devicePath.length() == 0 || devicePath.length() > PATH_MAX ||
        realpath(devicePath.c_str(), canonicalDevicePath) == nullptr) {
        DHLOGE("path check fail, error path: %s", devicePath.c_str());
        return -1;
    }
    struct stat s;
    if ((stat(canonicalDevicePath, &s) == 0) && (s.st_mode & S_IFDIR)) {
        DHLOGI("path: %s is a dir.", devicePath.c_str());
        return -1;
    }
    int fd = open(canonicalDevicePath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
    int32_t count = 0;
    while ((fd < 0) && (count < MAX_RETRY_COUNT)) {
        ++count;
        usleep(SLEEP_TIME_US);
        fd = open(canonicalDevicePath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
        DHLOGE("could not open %s, %s; retry %d\n", devicePath.c_str(), ConvertErrNo().c_str(), count);
    }
    if (count >= MAX_RETRY_COUNT) {
        DHLOGE("could not open %s, %s\n", devicePath.c_str(), ConvertErrNo().c_str());
        CloseFd(fd);
        return -1;
    }
    return fd;
}

std::string ConvertErrNo()
{
    char errMsg[ERROR_MSG_MAX_LEN] = {0};
    strerror_r(errno, errMsg, ERROR_MSG_MAX_LEN);
    std::string errNoMsg(errMsg);
    return errNoMsg;
}

void ScanInputDevicesPath(std::string dirName, std::vector<std::string>& vecInputDevPath)
{
    DIR *dir;
    struct dirent *de;
    dir = opendir(dirName.c_str());
    if (dir == nullptr) {
        DHLOGE("error opendir /dev/input :%{public}s\n", ConvertErrNo().c_str());
        return;
    }
    size_t dirNameFirstPos = 0;
    size_t dirNameSecondPos = 1;
    size_t dirNameThirdPos = 2;
    while ((de = readdir(dir))) {
        if (de->d_name[dirNameFirstPos] == '.' && (de->d_name[dirNameSecondPos] == '\0' ||
            (de->d_name[dirNameSecondPos] == '.' && de->d_name[dirNameThirdPos] == '\0'))) {
            continue;
        }
        std::string tmpDevName = dirName + "/" + std::string(de->d_name);
        vecInputDevPath.push_back(tmpDevName);
    }
    closedir(dir);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS