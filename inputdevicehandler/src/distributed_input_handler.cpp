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

#include "distributed_input_handler.h"

#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/sha.h>
#include <sys/inotify.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "nlohmann/json.hpp"

#include "constants_dinput.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_softbus_define.h"
#include "dinput_utils_tool.h"
#include "softbus_bus_center.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DistributedInputHandler);
DistributedInputHandler::DistributedInputHandler()
    : collectThreadID_(-1), isCollectingEvents_(false), isStartCollectEventThread(false)
{
    inputHub_ = std::make_unique<InputHub>(true);
    this->m_listener = nullptr;
}

DistributedInputHandler::~DistributedInputHandler()
{
    StopInputMonitorDeviceThread();
}

void DistributedInputHandler::StructTransJson(const InputDevice &pBuf, std::string &strDescriptor)
{
    DHLOGI("[%{public}s] %{public}d, %{public}d, %{public}d, %{public}d, %{public}s.\n", (pBuf.name).c_str(),
        pBuf.bus, pBuf.vendor, pBuf.product, pBuf.version, GetAnonyString(pBuf.descriptor).c_str());
    nlohmann::json tmpJson;
    tmpJson[DEVICE_NAME] = pBuf.name;
    tmpJson[PHYSICAL_PATH] = pBuf.physicalPath;
    tmpJson[UNIQUE_ID] = pBuf.uniqueId;
    tmpJson[BUS] = pBuf.bus;
    tmpJson[VENDOR] = pBuf.vendor;
    tmpJson[PRODUCT] = pBuf.product;
    tmpJson[VERSION] = pBuf.version;
    tmpJson[DESCRIPTOR] = pBuf.descriptor;
    tmpJson[CLASSES] = pBuf.classes;
    tmpJson[EVENT_TYPES] = pBuf.eventTypes;
    tmpJson[EVENT_KEYS] = pBuf.eventKeys;
    tmpJson[ABS_TYPES] = pBuf.absTypes;
    tmpJson[ABS_INFOS] = pBuf.absInfos;
    tmpJson[REL_TYPES] = pBuf.relTypes;
    tmpJson[PROPERTIES] = pBuf.properties;

    tmpJson[MISCELLANEOUS] = pBuf.miscellaneous;
    tmpJson[LEDS] = pBuf.leds;
    tmpJson[REPEATS] = pBuf.repeats;
    tmpJson[SWITCHS] = pBuf.switchs;

    std::ostringstream stream;
    stream << tmpJson.dump();
    strDescriptor = stream.str();
    DHLOGI("Record InputDevice json info: %{public}s", strDescriptor.c_str());
    return;
}

int32_t DistributedInputHandler::Initialize()
{
    if (!isStartCollectEventThread) {
        InitCollectEventsThread();
        isStartCollectEventThread = true;
    }
    return DH_SUCCESS;
}

void DistributedInputHandler::FindDevicesInfoByType(const uint32_t inputTypes, std::map<int32_t, std::string> &datas)
{
    if (inputHub_ != nullptr) {
        inputHub_->GetDevicesInfoByType(inputTypes, datas);
    }
}

void DistributedInputHandler::FindDevicesInfoByDhId(
    std::vector<std::string> dhidsVec, std::map<int32_t, std::string> &datas)
{
    if (inputHub_ != nullptr) {
        inputHub_->GetDevicesInfoByDhId(dhidsVec, datas);
    }
}

std::vector<DHItem> DistributedInputHandler::QueryMeta()
{
    return {};
}

std::vector<DHItem> DistributedInputHandler::Query()
{
    std::vector<DHItem> retInfos;

    if (inputHub_ != nullptr) {
        std::vector<InputDevice> vecInput = inputHub_->GetAllInputDevices();
        for (auto iter : vecInput) {
            DHItem item;
            item.dhId = iter.descriptor;
            item.subtype = "input";
            StructTransJson(iter, item.attrs);
            retInfos.push_back(item);
        }
    }

    return retInfos;
}

std::map<std::string, std::string> DistributedInputHandler::QueryExtraInfo()
{
    std::map<std::string, std::string> ret;
    return ret;
}

bool DistributedInputHandler::IsSupportPlugin()
{
    return true;
}

void DistributedInputHandler::RegisterPluginListener(std::shared_ptr<PluginListener> listener)
{
    this->m_listener = listener;
}

void DistributedInputHandler::UnRegisterPluginListener()
{
    this->m_listener = nullptr;
}

bool DistributedInputHandler::InitCollectEventsThread()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    isCollectingEvents_ = true;
    collectThreadID_ = -1;
    int32_t ret = pthread_create(&collectThreadID_, &attr, CollectEventsThread, this);
    if (ret != 0) {
        DHLOGE("DistributedInputHandler::InitCollectEventsThread create thread failed:%{public}d \n", ret);
        pthread_attr_destroy(&attr);
        collectThreadID_ = -1;
        isCollectingEvents_ = false;
        return false;
    }
    return true;
}

void *DistributedInputHandler::CollectEventsThread(void *param)
{
    int32_t ret = pthread_setname_np(pthread_self(), COLLECT_EVENT_THREAD_NAME);
    if (ret != 0) {
        DHLOGE("CollectEventsThread setname failed.");
    }
    DistributedInputHandler *pThis = reinterpret_cast<DistributedInputHandler *>(param);
    pThis->StartInputMonitorDeviceThread();
    DHLOGI("DistributedInputHandler::CollectEventsThread exist!");
    return nullptr;
}

void DistributedInputHandler::StartInputMonitorDeviceThread()
{
    if (inputHub_ == nullptr) {
        DHLOGE("inputHub_ not initialized");
        return;
    }
    while (isCollectingEvents_) {
        size_t count = inputHub_->StartCollectInputHandler(mEventBuffer, inputDeviceBufferSize);
        if (count > 0) {
            DHLOGI("Count: %{public}zu", count);
            for (size_t iCnt = 0; iCnt < count; iCnt++) {
                NotifyHardWare(iCnt);
            }
        } else {
            continue;
        }
    }
    isCollectingEvents_ = false;
    DHLOGI("DistributedInputHandler::StartCollectEventsThread exit!");
}

void DistributedInputHandler::NotifyHardWare(int iCnt)
{
    switch (mEventBuffer[iCnt].type) {
        case DeviceType::DEVICE_ADDED:
            if (this->m_listener != nullptr) {
                std::string hdInfo;
                StructTransJson(mEventBuffer[iCnt].deviceInfo, hdInfo);
                std::string subtype = "input";
                this->m_listener->PluginHardware(mEventBuffer[iCnt].deviceInfo.descriptor, hdInfo, subtype);
            }
            break;
        case DeviceType::DEVICE_REMOVED:
            if (this->m_listener != nullptr) {
                this->m_listener->UnPluginHardware(mEventBuffer[iCnt].deviceInfo.descriptor);
            }
            break;
        default:
            break;
    }
}

void DistributedInputHandler::StopInputMonitorDeviceThread()
{
    isCollectingEvents_ = false;
    isStartCollectEventThread = false;
    inputHub_->StopCollectInputHandler();
    if (collectThreadID_ != (pthread_t)(-1)) {
        DHLOGI("DistributedInputHandler::Wait collect thread exit");
        pthread_join(collectThreadID_, NULL);
        collectThreadID_ = (pthread_t)(-1);
    }
    DHLOGI("DistributedInputHandler::StopInputMonitorDeviceThread exit!");
}

IHardwareHandler* GetHardwareHandler()
{
    return &DistributedInputHandler::GetInstance();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
