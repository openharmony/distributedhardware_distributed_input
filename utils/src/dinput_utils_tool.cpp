/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <climits>
#include <cstdarg>
#include <cstdio>
#include <random>

#include <dirent.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/sha.h>

#include "nlohmann/json.hpp"
#include "securec.h"

#include "softbus_bus_center.h"

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
    constexpr uint32_t ERROR_MSG_MAX_LEN = 256;
    constexpr int32_t MAX_RETRY_COUNT = 3;
    constexpr uint32_t SLEEP_TIME_US = 10 * 1000;
    constexpr char DHID_SPLIT = '.';
}
DevInfo GetLocalDeviceInfo()
{
    DevInfo devInfo{"", "", 0};
    auto info = std::make_unique<NodeBasicInfo>();
    auto ret = GetLocalNodeDeviceInfo(DINPUT_PKG_NAME.c_str(), info.get());
    if (ret != 0) {
        DHLOGE("GetLocalNodeDeviceInfo failed, errCode = %{public}d", ret);
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
    DHLOGI("GetLocalNetworkId, device local networkId is %{public}s", GetAnonyString(localNetworkId).c_str());
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

uint64_t GetCurrentTimeUs()
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
        if (anonyDhidStr.length() >= 1) {
            jsonObj[DINPUT_SOFTBUS_KEY_VECTOR_DHID] = anonyDhidStr.substr(0, anonyDhidStr.length() - 1);
        }
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_SRC_DEV_ID)) {
        jsonObj[DINPUT_SOFTBUS_KEY_SRC_DEV_ID] = GetAnonyString(jsonObj[DINPUT_SOFTBUS_KEY_SRC_DEV_ID]);
    }
    if (IsString(jsonObj, DINPUT_SOFTBUS_KEY_SINK_DEV_ID)) {
        jsonObj[DINPUT_SOFTBUS_KEY_SINK_DEV_ID] = GetAnonyString(jsonObj[DINPUT_SOFTBUS_KEY_SINK_DEV_ID]);
    }
    return jsonObj.dump();
}

bool IsBoolean(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_boolean();
}

bool IsString(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_string();
}

bool IsInt32(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_number_integer() && INT32_MIN <= jsonObj[key] &&
        jsonObj[key] <= INT32_MAX;
}

bool IsInt64(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_number_integer() && INT64_MIN <= jsonObj[key] &&
        jsonObj[key] <= INT64_MAX;
}

bool IsUInt16(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT16_MAX;
}

bool IsUInt32(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT32_MAX;
}

bool IsUInt64(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT64_MAX;
}

bool IsArray(const nlohmann::json &jsonObj, const std::string &key)
{
    return jsonObj.contains(key) && jsonObj[key].is_array();
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
    if (length < 1) {
        DHLOGE("tempBuffer length error.");
        return "";
    }
    for (size_t i = 1; i <= length - 1; i++) {
        tempBuffer[i] = '*';
    }
    if (length == 0x01) {
        tempBuffer[0] = '*';
    }

    std::string tempString(tempBuffer);
    return tempString;
}

std::string Sha256(const std::string &in)
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

void CloseFd(int fd)
{
    if (fd < 0) {
        DHLOGE("No fd need to beclosed.");
        return;
    }
    close(fd);
    fd = UN_INIT_FD_VALUE;
}

int BitIsSet(const unsigned long *array, int bit)
{
    return !!(array[bit / LONG_BITS] & (1LL << (bit % LONG_BITS)));
}

void SplitStringToVector(const std::string &str, const char split, std::vector<std::string> &vecStr)
{
    if (str.empty()) {
        DHLOGE("SplitStringToVector param str is error.");
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

int OpenInputDeviceFdByPath(const std::string &devicePath)
{
    chmod(devicePath.c_str(), S_IWRITE | S_IREAD);
    char canonicalDevicePath[PATH_MAX] = {0x00};
    if (devicePath.length() == 0 || devicePath.length() >= PATH_MAX ||
        realpath(devicePath.c_str(), canonicalDevicePath) == nullptr) {
        DHLOGE("path check fail, error path: %{public}s", devicePath.c_str());
        return -1;
    }
    struct stat s;
    if ((stat(canonicalDevicePath, &s) == 0) && (s.st_mode & S_IFDIR)) {
        DHLOGI("path: %{public}s is a dir.", devicePath.c_str());
        return -1;
    }
    int fd = open(canonicalDevicePath, O_RDWR | O_CLOEXEC);
    int32_t count = 0;
    while ((fd < 0) && (count < MAX_RETRY_COUNT)) {
        ++count;
        usleep(SLEEP_TIME_US);
        fd = open(canonicalDevicePath, O_RDWR | O_CLOEXEC);
        DHLOGE("could not open the path: %{public}s, errno: %{public}s; retry: %{public}d", devicePath.c_str(),
            ConvertErrNo().c_str(), count);
    }
    if (count >= MAX_RETRY_COUNT) {
        DHLOGE("could not open the path: %{public}s, errno: %{public}s.", devicePath.c_str(), ConvertErrNo().c_str());
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

void ScanInputDevicesPath(const std::string &dirName, std::vector<std::string> &vecInputDevPath)
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

void RecordEventLog(const input_event &event)
{
    std::string eventType = "";
    switch (event.type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_SYN:
            eventType = "EV_SYN";
            break;
        default:
            eventType = "other type " + std::to_string(event.type);
            break;
    }
    DHLOGD("5.E2E-Test Source write event into input driver, EventType: %{public}s, Code: %{public}d, "
        "Value: %{public}d", eventType.c_str(), event.code, event.value);
}

void WriteEventToDevice(const int fd, const input_event &event)
{
    if (write(fd, &event, sizeof(event)) < static_cast<ssize_t>(sizeof(event))) {
        DHLOGE("could not inject event, fd: %{public}d", fd);
        return;
    }
    RecordEventLog(event);
}

void ResetVirtualDevicePressedKeys(const std::vector<std::string> &nodePaths)
{
    unsigned long keyState[NLONGS(KEY_CNT)] = { 0 };
    for (const auto &path : nodePaths) {
        DHLOGI("Check and reset key state, path: %{public}s", path.c_str());
        std::vector<uint32_t> pressedKeys;
        int fd = OpenInputDeviceFdByPath(path);
        if (fd == -1) {
            DHLOGE("Open virtual keyboard node failed, path: %{public}s", path.c_str());
            continue;
        }

        int rc = ioctl(fd, EVIOCGKEY(sizeof(keyState)), keyState);
        if (rc < 0) {
            DHLOGE("Read all key state failed, rc: %{public}d, path: %{public}s", rc, path.c_str());
            continue;
        }

        for (int32_t keyIndex = 0; keyIndex < KEY_MAX; keyIndex++) {
            if (BitIsSet(keyState, keyIndex)) {
                DHLOGI("key index: %{public}d pressed.", keyIndex);
                pressedKeys.push_back(keyIndex);
            }
        }

        if (pressedKeys.empty()) {
            continue;
        }

        struct input_event event = {
            .type = EV_KEY,
            .code = 0,
            .value = KEY_UP_STATE
        };
        for (auto &code : pressedKeys) {
            event.type = EV_KEY;
            event.code = code;
            WriteEventToDevice(fd, event);
            event.type = EV_SYN;
            event.code = 0;
            WriteEventToDevice(fd, event);
        }
    }
}

std::string GetString(const std::vector<std::string> &vec)
{
    std::string retStr = "[";
    if (vec.size() < 1) {
        DHLOGE("vec size error.");
        return "";
    }
    for (uint32_t i = 0; i < vec.size(); i++) {
        if (i != (vec.size() - 1)) {
            retStr += vec[i] + ", ";
        } else {
            retStr += vec[i];
        }
    }
    retStr += "]";
    return retStr;
}

int32_t GetRandomInt32(int32_t randMin, int32_t randMax)
{
    std::default_random_engine engine(time(nullptr));

    std::uniform_int_distribution<int> distribution(randMin, randMax);
    return distribution(engine);
}

std::string JointDhIds(const std::vector<std::string> &dhids)
{
    if (dhids.size() == 0) {
        return "";
    }
    auto dotFold = [](std::string a, std::string b) {return std::move(a) + DHID_SPLIT + std::move(b);};
    return std::accumulate(std::next(dhids.begin()), dhids.end(), dhids[0], dotFold);
}

std::vector<std::string> SplitDhIdString(const std::string &dhIdsString)
{
    std::vector<std::string> dhIdsVec;
    SplitStringToVector(dhIdsString, DHID_SPLIT, dhIdsVec);
    return dhIdsVec;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS