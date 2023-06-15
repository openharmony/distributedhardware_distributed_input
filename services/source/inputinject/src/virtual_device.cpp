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

#include "virtual_device.h"

#include <securec.h>

#include "constants_dinput.h"
#include "dinput_log.h"
#include "hidumper.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    constexpr uint32_t ABS_MIN_POS = 1;
    constexpr uint32_t ABS_MAX_POS = 2;
    constexpr uint32_t ABS_FUZZ_POS = 3;
    constexpr uint32_t ABS_FLAT_POS = 4;
}
VirtualDevice::VirtualDevice(const InputDevice& event) : deviceName_(event.name), busType_(event.bus),
    vendorId_(event.vendor), productId_(event.product), version_(event.version), classes_(event.classes)
{
    DHLOGI("VirtualDevice ctor!");
}

VirtualDevice::~VirtualDevice()
{
    if (fd_ >= 0) {
        ioctl(fd_, UI_DEV_DESTROY);
        close(fd_);
        fd_ = -1;
    }
}

bool VirtualDevice::DoIoctl(int32_t fd, int32_t request, const uint32_t value)
{
    int32_t rc = ioctl(fd, request, value);
    if (rc < 0) {
        DHLOGE("ioctl failed");
        return false;
    }
    return true;
}

bool VirtualDevice::CreateKey(const InputDevice& inputDevice)
{
    auto fun = [this](int32_t uiSet, const std::vector<uint32_t>& list) -> bool {
        for (uint32_t evt_type : list) {
            if (!DoIoctl(fd_, uiSet, evt_type)) {
                DHLOGE("Error setting event type: %u", evt_type);
                return false;
            }
        }
        return true;
    };
    std::map<int32_t, std::vector<uint32_t>> evt_type;
    evt_type[UI_SET_EVBIT] = inputDevice.eventTypes;
    evt_type[UI_SET_KEYBIT] = inputDevice.eventKeys;
    evt_type[UI_SET_PROPBIT] = inputDevice.properties;
    evt_type[UI_SET_ABSBIT] = inputDevice.absTypes;
    evt_type[UI_SET_RELBIT] = inputDevice.relTypes;
    for (auto &it : evt_type) {
        if (!fun(it.first, it.second)) {
            return false;
        }
    }
    return true;
}

void VirtualDevice::SetABSInfo(struct uinput_user_dev& inputUserDev, const InputDevice& inputDevice)
{
    DHLOGI("SetABSInfo!");
    for (const auto item : inputDevice.absInfos) {
        int absCode = item.first;
        std::vector<int32_t> absInfo = item.second;
        DHLOGI("SetABSInfo nodeName: %s, absCode: %d, absMin: %d, absMax: %d, absFuzz: %d, absFlat: %d",
            inputDevice.name.c_str(), absCode, absInfo[ABS_MIN_POS], absInfo[ABS_MAX_POS], absInfo[ABS_FUZZ_POS],
            absInfo[ABS_FLAT_POS]);
        inputUserDev.absmin[absCode] = absInfo[ABS_MIN_POS];
        inputUserDev.absmax[absCode] = absInfo[ABS_MAX_POS];
        inputUserDev.absfuzz[absCode] = absInfo[ABS_FUZZ_POS];
        inputUserDev.absflat[absCode] = absInfo[ABS_FLAT_POS];
    }
}

bool VirtualDevice::SetPhys(const std::string deviceName, std::string dhId)
{
    std::string phys;
    phys.append(deviceName).append(pid_).append("/").append(pid_).append("|")
        .append(netWorkId_).append("|").append(dhId);

    if (ioctl(fd_, UI_SET_PHYS, phys.c_str()) < 0) {
        return false;
    }
    return true;
}

bool VirtualDevice::SetUp(const InputDevice& inputDevice, const std::string& devId, const std::string& dhId)
{
    fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd_ < 0) {
        DHLOGE("Failed to open uinput");
        return false;
    }

    deviceName_ = VIRTUAL_DEVICE_NAME + deviceName_;
    if (deviceName_.size() > MAX_SIZE_OF_DEVICE_NAME) {
        deviceName_ = deviceName_.substr(0, MAX_SIZE_OF_DEVICE_NAME);
    }
    if (strncpy_s(dev_.name, sizeof(dev_.name), deviceName_.c_str(), deviceName_.size()) != 0) {
        return false;
    }
    HiDumper::GetInstance().SaveNodeInfo(devId, deviceName_, dhId);
    dev_.id.bustype = busType_;
    dev_.id.vendor = vendorId_;
    dev_.id.product = productId_;
    dev_.id.version = version_;

    if (!SetPhys(deviceName_, dhId)) {
        DHLOGE("Failed to set PHYS!");
        return false;
    }

    if (!CreateKey(inputDevice)) {
        DHLOGE("Failed to create KeyValue");
        return false;
    }

    SetABSInfo(dev_, inputDevice);

    if (write(fd_, &dev_, sizeof(dev_)) < 0) {
        DHLOGE("Unable to set input device info");
        return false;
    }

    if (ioctl(fd_, UI_DEV_CREATE) < 0) {
        DHLOGE(
            "fd = %d, ioctl(fd_, UI_DEV_CREATE) = %d",
            fd_, ioctl(fd_, UI_DEV_CREATE));
        DHLOGE("Unable to create input device");
        return false;
    }
    DHLOGI("create fd %d", fd_);

    char sysfsDeviceName[16] = {0};
    if (ioctl(fd_, UI_GET_SYSNAME(sizeof(sysfsDeviceName)), sysfsDeviceName) < 0) {
        DHLOGE("Unable to get input device name");
    }
    DHLOGI("get input device name: %s, fd: %d", GetAnonyString(sysfsDeviceName).c_str(), fd_);
    return true;
}

bool VirtualDevice::InjectInputEvent(const input_event& event)
{
    DHLOGI("InjectInputEvent %d", fd_);

    if (write(fd_, &event, sizeof(event)) < static_cast<ssize_t>(sizeof(event))) {
        DHLOGE("could not inject event, removed? (fd: %d", fd_);
        return false;
    }
    RecordEventLog(event);
    DHLOGI("InjectInputEvent end\n");

    return true;
}

void VirtualDevice::SetNetWorkId(const std::string netWorkId)
{
    DHLOGI("SetNetWorkId %s\n", GetAnonyString(netWorkId).c_str());
    netWorkId_ = netWorkId;
}

std::string VirtualDevice::GetNetWorkId()
{
    return netWorkId_;
}

void VirtualDevice::RecordEventLog(const input_event& event)
{
    std::string eventType = "";
    switch (event.type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_REL:
            eventType = "EV_REL";
            break;
        case EV_ABS:
            eventType = "EV_ABS";
            break;
        default:
            eventType = "other type";
            break;
    }
    DHLOGD("4.E2E-Test Source write event into input driver, EventType: %s, Code: %d, Value: %d, Sec: %ld, Sec1: %ld",
        eventType.c_str(), event.code, event.value, event.input_event_sec, event.input_event_usec);
}

int32_t VirtualDevice::GetDeviceFd()
{
    return fd_;
}

uint16_t VirtualDevice::GetDeviceType()
{
    return classes_;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
