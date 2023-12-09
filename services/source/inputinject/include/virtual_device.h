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

#ifndef OHOS_VIRTUAL_DEVICE_H
#define OHOS_VIRTUAL_DEVICE_H

#include <map>
#include <string>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/input.h>
#include "linux/uinput.h"

#include "constants_dinput.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class VirtualDevice {
public:
    explicit VirtualDevice(const InputDevice &event);
    virtual ~VirtualDevice();
    bool DoIoctl(int32_t fd, int32_t request, const uint32_t value);
    bool CreateKey(const InputDevice &inputDevice);
    void SetABSInfo(struct uinput_user_dev &inputUserDev, const InputDevice &inputDevice);
    bool SetPhys(const std::string &deviceName, const std::string &dhId);
    bool SetUp(const InputDevice &inputDevice, const std::string &devId, const std::string &dhId);
    bool InjectInputEvent(const input_event &event);
    void SetNetWorkId(const std::string &netWorkId);
    void SetPath(const std::string &path);
    std::string GetNetWorkId();
    std::string GetPath();
    uint16_t GetClasses();

    int32_t GetDeviceFd();
    uint16_t GetDeviceType();

private:
    int32_t fd_ = -1;
    std::string deviceName_;
    std::string netWorkId_;
    std::string path_;
    const uint16_t busType_;
    const uint16_t vendorId_;
    const uint16_t productId_;
    const uint16_t version_;
    const uint16_t classes_;
    struct uinput_user_dev dev_ {};
    const std::string pid_ = std::to_string(getpid());

private:
    void RecordEventLog(const input_event &event);
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_VIRTUAL_DEVICE_H
