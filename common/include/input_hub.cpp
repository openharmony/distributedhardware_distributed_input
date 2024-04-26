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

#include "input_hub.h"

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <regex>
#include <securec.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <utility>

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_sink_state.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
const uint32_t SLEEP_TIME_US = 100 * 1000;
const std::string MOUSE_NODE_KEY = "mouse";
const uint32_t SPACELENGTH = 1024;
}

InputHub::InputHub(bool isPluginMonitor) : epollFd_(-1), iNotifyFd_(-1), inputWd_(-1),
    isPluginMonitor_(isPluginMonitor), needToScanDevices_(true), mPendingEventItems{},
    pendingEventCount_(0), pendingEventIndex_(0), pendingINotify_(false), deviceChanged_(false),
    inputTypes_(0), isStartCollectEvent_(false), isStartCollectHandler_(false)
{
    Initialize();
}

InputHub::~InputHub()
{
    Release();
}

int32_t InputHub::Initialize()
{
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ < 0) {
        DHLOGE("Could not create epoll instance: %{public}s", ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_EPOLL_INIT_FAIL;
    }

    if (isPluginMonitor_) {
        DHLOGI("Init InputHub for device plugin monitor");
        iNotifyFd_ = inotify_init();
        inputWd_ = inotify_add_watch(iNotifyFd_, DEVICE_PATH, IN_DELETE | IN_CREATE);
        if (inputWd_ < 0) {
            DHLOGE("Could not register INotify for %{public}s: %{public}s", DEVICE_PATH, ConvertErrNo().c_str());
            return ERR_DH_INPUT_HUB_EPOLL_INIT_FAIL;
        }

        struct epoll_event eventItem = {};
        eventItem.events = EPOLLIN;
        eventItem.data.fd = iNotifyFd_;
        int result = epoll_ctl(epollFd_, EPOLL_CTL_ADD, iNotifyFd_, &eventItem);
        if (result != 0) {
            DHLOGE("Could not add INotify to epoll instance.  errno=%{public}d", errno);
            return ERR_DH_INPUT_HUB_EPOLL_INIT_FAIL;
        }
    } else {
        DHLOGI("Init InputHub for read device events");
    }

    return DH_SUCCESS;
}

int32_t InputHub::Release()
{
    CloseAllDevicesLocked();
    if (epollFd_ != -1) {
        ::close(epollFd_);
        epollFd_ = -1;
    }

    if (iNotifyFd_ != -1) {
        ::close(iNotifyFd_);
        iNotifyFd_ = -1;
    }

    if (isPluginMonitor_) {
        StopCollectInputHandler();
    } else {
        StopCollectInputEvents();
    }

    sharedDHIds_.clear();
    logTimesMap_.clear();
    return DH_SUCCESS;
}

bool InputHub::IsInputNodeNoNeedScan(const std::string &path)
{
    {
        std::lock_guard<std::mutex> deviceLock(devicesMutex_);
        auto iter = devices_.find(path);
        if (iter != devices_.end()) {
            return true;
        }
    }

    if (path.find(MOUSE_NODE_KEY) != std::string::npos) {
        return true;
    }

    return IsSkipDevicePath(path);
}

void InputHub::ScanAndRecordInputDevices()
{
    ScanInputDevices(DEVICE_PATH);

    {
        std::lock_guard<std::mutex> deviceLock(devicesMutex_);
        while (!openingDevices_.empty()) {
            std::unique_ptr<Device> device = std::move(*openingDevices_.rbegin());
            openingDevices_.pop_back();
            DHLOGI("Reporting device opened: path=%{public}s, name=%{public}s\n",
                device->path.c_str(), device->identifier.name.c_str());
            std::string devPath = device->path;
            auto [dev_it, inserted] = devices_.insert_or_assign(device->path, std::move(device));
            if (!inserted) {
                DHLOGI("Device with this path %{public}s exists, replaced. \n", devPath.c_str());
            }
        }
    }
}

size_t InputHub::StartCollectInputEvents(RawEvent *buffer, size_t bufferSize)
{
    size_t count = 0;
    isStartCollectEvent_ = true;
    while (isStartCollectEvent_) {
        ScanAndRecordInputDevices();
        count = GetEvents(buffer, bufferSize);
        if (count > 0) {
            break;
        }

        if (RefreshEpollItem(false) < 0) {
            break;
        }
    }

    // All done, return the number of events we read.
    return count;
}

void InputHub::StopCollectInputEvents()
{
    DHLOGI("Stop Collect Input Events Thread");
    isStartCollectEvent_ = false;
}

size_t InputHub::GetEvents(RawEvent *buffer, size_t bufferSize)
{
    RawEvent* event = buffer;
    size_t capacity = bufferSize;
    while (pendingEventIndex_ < pendingEventCount_) {
        std::lock_guard<std::mutex> my_lock(operationMutex_);
        const struct epoll_event& eventItem = mPendingEventItems[pendingEventIndex_++];
        if (eventItem.data.fd == iNotifyFd_) {
            continue;
        }
        struct input_event readBuffer[bufferSize];
        int32_t readSize = read(eventItem.data.fd, readBuffer, sizeof(struct input_event) * capacity);
        size_t count = ReadInputEvent(readSize, *GetDeviceByFdLocked(eventItem.data.fd));
        Device* device = GetSupportDeviceByFd(eventItem.data.fd);
        if (!device) {
            DHLOGE("Can not find device by fd: %{public}d", eventItem.data.fd);
            continue;
        }
        if (!sharedDHIds_[device->identifier.descriptor]) {
            RecordDeviceChangeStates(device, readBuffer, count);
            DHLOGD("Not in sharing stat, device descriptor: %{public}s",
                GetAnonyString(device->identifier.descriptor).c_str());
            continue;
        }
        if (eventItem.events & EPOLLIN) {
            event += CollectEvent(event, capacity, device, readBuffer, count);

            if (capacity == 0) {
                pendingEventIndex_ -= 1;
                break;
            }
        } else if (eventItem.events & EPOLLHUP) {
            DHLOGI("Removing device %{public}s due to epoll hang-up event.", device->identifier.name.c_str());
            CloseDeviceLocked(*device);
        }
    }
    return event - buffer;
}

bool InputHub::IsCuror(Device *device)
{
    return device->classes & INPUT_DEVICE_CLASS_CURSOR;
}

bool InputHub::IsTouchPad(Device *device)
{
    return ((device->classes & INPUT_DEVICE_CLASS_TOUCH_MT) || (device->classes & INPUT_DEVICE_CLASS_TOUCH)) &&
        IsTouchPad(device->identifier);
}

bool InputHub::IsTouchPad(const InputDevice &inputDevice)
{
    std::string dhName = inputDevice.name;
    transform(dhName.begin(), dhName.end(), dhName.begin(), ::tolower);
    if (dhName.find(DH_TOUCH_PAD) == std::string::npos) {
        return false;
    }
    return true;
}

void InputHub::MatchAndDealEvent(Device *device, const RawEvent &event)
{
    bool isTouchPad = IsTouchPad(device);
    if (!isTouchPad) {
        // Deal Normal key state, such as keys of keyboard or mouse
        DealNormalKeyEvent(device, event);
    } else {
        // Deal TouchPad events
        DealTouchPadEvent(event);
    }
}

void InputHub::DealTouchPadEvent(const RawEvent &event)
{
    auto ret = DInputSinkState::GetInstance().GetTouchPadEventFragMgr()->PushEvent(event.descriptor, event);
    if (ret.first) {
        DInputSinkState::GetInstance().SimulateTouchPadStateReset(ret.second);
    }
}

void InputHub::DealNormalKeyEvent(Device *device, const RawEvent &event)
{
    if (event.type == EV_KEY && event.value == KEY_DOWN_STATE) {
        DInputSinkState::GetInstance().AddKeyDownState(event);
        RecordChangeEventLog(event);
    }
    if (event.type == EV_KEY && event.value == KEY_UP_STATE) {
        // Deal mouse left keydown reset
        if (IsCuror(device) && event.code == BTN_MOUSE &&
            !DInputSinkState::GetInstance().IsDhIdDown(event.descriptor)) {
            DHLOGI("Find mouse BTN_MOUSE UP state that not down effective at sink side, dhId: %{public}s",
                GetAnonyString(event.descriptor).c_str());
            DInputSinkState::GetInstance().SimulateMouseBtnMouseUpState(event.descriptor, event);
        }
        DInputSinkState::GetInstance().RemoveKeyDownState(event);
        RecordChangeEventLog(event);
    }
    if (event.type == EV_KEY && event.value == KEY_REPEAT) {
        DInputSinkState::GetInstance().CheckAndSetLongPressedKeyOrder(event);
    }
}

void InputHub::RecordDeviceChangeStates(Device *device, struct input_event readBuffer[], const size_t count)
{
    bool isTouchEvent = false;
    if ((device->classes & INPUT_DEVICE_CLASS_TOUCH_MT) || (device->classes & INPUT_DEVICE_CLASS_TOUCH)) {
        if (!IsTouchPad(device->identifier)) {
            isTouchEvent = true;
        }
    }

    for (size_t i = 0; i < count; i++) {
        const struct input_event& iev = readBuffer[i];
        RawEvent event;
        event.when = ProcessEventTimestamp(iev);
        event.type = iev.type;
        event.code = iev.code;
        event.value = iev.value;
        event.path = device->path;
        event.descriptor = isTouchEvent ? touchDescriptor : device->identifier.descriptor;
        MatchAndDealEvent(device, event);
    }
}

size_t InputHub::CollectEvent(RawEvent *buffer, size_t &capacity, Device *device, struct input_event readBuffer[],
    const size_t count)
{
    std::vector<bool> needFilted(capacity, false);
    bool isTouchEvent = false;
    if ((device->classes & INPUT_DEVICE_CLASS_TOUCH_MT) || (device->classes & INPUT_DEVICE_CLASS_TOUCH)) {
        if (!IsTouchPad(device->identifier)) {
            isTouchEvent = true;
            HandleTouchScreenEvent(readBuffer, count, needFilted);
        }
    }

    RawEvent* event = buffer;
    for (size_t i = 0; i < count; i++) {
        if (needFilted[i]) {
            continue;
        }
        const struct input_event& iev = readBuffer[i];
        event->when = ProcessEventTimestamp(iev);
        event->type = iev.type;
        event->code = iev.code;
        event->value = iev.value;
        event->path = device->path;
        event->descriptor = isTouchEvent ? touchDescriptor : device->identifier.descriptor;
        RecordEventLog(event);
        event += 1;
        capacity -= 1;
        if (capacity == 0) {
            break;
        }
    }
    return event - buffer;
}

size_t InputHub::ReadInputEvent(int32_t readSize, Device &device)
{
    size_t count = 0;
    if (readSize == 0 || (readSize < 0 && errno == ENODEV)) {
        // Device was removed before INotify noticed.
        DHLOGE("could not get event, removed? (fd: %{public}d size: %{public}d, errno: %{public}d)\n",
            device.fd, readSize, errno);
        CloseDeviceLocked(device);
    } else if (readSize < 0) {
        if (errno != EAGAIN && errno != EINTR) {
            DHLOGW("could not get event (errno=%{public}d)", errno);
        }
    } else if ((readSize % sizeof(struct input_event)) != 0) {
        DHLOGW("could not get event (wrong size: %{public}d)", readSize);
    } else {
        count = size_t(readSize) / sizeof(struct input_event);
        return count;
    }
    return count;
}

size_t InputHub::DeviceIsExists(InputDeviceEvent *buffer, size_t bufferSize)
{
    InputDeviceEvent* event = buffer;
    size_t capacity = bufferSize;
    // Report any devices that had last been added/removed.
    {
        std::lock_guard<std::mutex> deviceLock(devicesMutex_);
        for (auto it = closingDevices_.begin(); it != closingDevices_.end();) {
            std::unique_ptr<Device> device = std::move(*it);
            DHLOGI("Reporting device closed: id=%{public}s, name=%{public}s",
                device->path.c_str(), device->identifier.name.c_str());
            event->type = DeviceType::DEVICE_REMOVED;
            event->deviceInfo = device->identifier;
            event += 1;
            it = closingDevices_.erase(it);
            if (capacity == 0) {
                break;
            }
            capacity--;
        }
    }

    if (needToScanDevices_) {
        needToScanDevices_ = false;
        ScanInputDevices(DEVICE_PATH);
    }

    {
        std::lock_guard<std::mutex> deviceLock(devicesMutex_);
        while (!openingDevices_.empty()) {
            std::unique_ptr<Device> device = std::move(*openingDevices_.rbegin());
            openingDevices_.pop_back();
            DHLOGI("Reporting device opened: id=%{public}s, name=%{public}s",
                device->path.c_str(), device->identifier.name.c_str());
            event->type = DeviceType::DEVICE_ADDED;
            event->deviceInfo = device->identifier;
            event += 1;

            std::string devPath = device->path;
            auto [dev_it, inserted] = devices_.insert_or_assign(device->path, std::move(device));
            if (!inserted) {
                DHLOGI("Device path %{public}s exists, replaced.", devPath.c_str());
            }
            if (capacity == 0) {
                break;
            }
            capacity--;
        }
    }
    return event - buffer;
}

size_t InputHub::StartCollectInputHandler(InputDeviceEvent *buffer, size_t bufferSize)
{
    size_t count = 0;
    isStartCollectHandler_ = true;
    while (isStartCollectHandler_) {
        count = DeviceIsExists(buffer, bufferSize);
        deviceChanged_ = false;
        GetDeviceHandler();

        if (pendingINotify_ && pendingEventIndex_ >= pendingEventCount_) {
            pendingINotify_ = false;
            ReadNotifyLocked();
            deviceChanged_ = true;
        }

        // Report added or removed devices immediately.
        if (deviceChanged_) {
            continue;
        }
        if (count > 0) {
            break;
        }
        if (RefreshEpollItem(true) < 0) {
            break;
        }
    }

    // All done, return the number of events we read.
    return count;
}

void InputHub::StopCollectInputHandler()
{
    DHLOGI("Stop Collect Input Handler Thread");
    isStartCollectHandler_ = false;
}

void InputHub::GetDeviceHandler()
{
    while (pendingEventIndex_ < pendingEventCount_) {
        std::lock_guard<std::mutex> my_lock(operationMutex_);
        const struct epoll_event& eventItem = mPendingEventItems[pendingEventIndex_++];
        if (eventItem.data.fd == iNotifyFd_) {
            if (eventItem.events & EPOLLIN) {
                pendingINotify_ = true;
            } else {
                DHLOGI("Received unexpected epoll event 0x%08x for INotify.", eventItem.events);
            }
            continue;
        }

        if (eventItem.events & EPOLLHUP) {
            Device* device = GetDeviceByFdLocked(eventItem.data.fd);
            if (!device) {
                DHLOGE("Received unexpected epoll event 0x%{public}08x for unknown fd %{public}d.",
                    eventItem.events, eventItem.data.fd);
                continue;
            }
            DHLOGI("Removing device %{public}s due to epoll hang-up event.", device->identifier.name.c_str());
            deviceChanged_ = true;
            CloseDeviceLocked(*device);
        }
    }
}

int32_t InputHub::RefreshEpollItem(bool isSleep)
{
    pendingEventIndex_ = 0;
    int pollResult = epoll_wait(epollFd_, mPendingEventItems, EPOLL_MAX_EVENTS, EPOLL_WAITTIME);
    if (pollResult == 0) {
        // Timed out.
        pendingEventCount_ = 0;
        return ERR_DH_INPUT_HUB_EPOLL_WAIT_TIMEOUT;
    }

    if (pollResult < 0) {
        // An error occurred.
        pendingEventCount_ = 0;

        // Sleep after errors to avoid locking up the system.
        // Hopefully the error is transient.
        if (errno != EINTR) {
            DHLOGE("poll failed (errno=%{public}d)\n", errno);
            usleep(SLEEP_TIME_US);
        }
    } else {
        // Some events occurred.
        pendingEventCount_ = pollResult;
    }
    if (isSleep) {
        usleep(SLEEP_TIME_US);
    }
    return DH_SUCCESS;
}

std::vector<InputDevice> InputHub::GetAllInputDevices()
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    std::vector<InputDevice> vecDevice;
    for (const auto &[id, device] : devices_) {
        vecDevice.push_back(device->identifier);
    }
    return vecDevice;
}

void InputHub::ScanInputDevices(const std::string &dirName)
{
    std::vector<std::string> inputDevPaths;
    ScanInputDevicesPath(dirName, inputDevPaths);
    for (const auto &tempPath: inputDevPaths) {
        if (IsInputNodeNoNeedScan(tempPath)) {
            continue;
        }
        OpenInputDeviceLocked(tempPath);
    }
}

bool InputHub::IsDeviceRegistered(const std::string &devicePath)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[deviceId, device] : devices_) {
        if (device->path == devicePath) {
            DHLOGI("Device node already registered, node path: %{public}s", device->path.c_str());
            return true; // device was already registered
        }
    }
    return false;
}

int32_t InputHub::OpenInputDeviceLocked(const std::string &devicePath)
{
    if (IsDeviceRegistered(devicePath)) {
        return DH_SUCCESS;
    }

    std::lock_guard<std::mutex> my_lock(operationMutex_);
    DHLOGD("Opening device start: %{public}s", devicePath.c_str());
    int fd = OpenInputDeviceFdByPath(devicePath);
    if (fd == UN_INIT_FD_VALUE) {
        DHLOGE("The fd open failed, devicePath %{public}s.", devicePath.c_str());
        return ERR_DH_INPUT_HUB_OPEN_DEVICEPATH_FAIL;
    }

    // Allocate device. (The device object takes ownership of the fd at this point.)
    std::unique_ptr<Device> device = std::make_unique<Device>(fd, devicePath);

    if (QueryInputDeviceInfo(fd, device) < 0) {
        CloseFd(fd);
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    GenerateDescriptor(device->identifier);
    IncreaseLogTimes(device->identifier.descriptor);
    RecordDeviceLog(devicePath, device->identifier);

    if (MakeDevice(fd, std::move(device)) < 0) {
        CloseFd(fd);
        if (IsNeedPrintLog(device->identifier.descriptor)) {
            DHLOGI("Opening device error: %{public}s", devicePath.c_str());
        }
        return ERR_DH_INPUT_HUB_MAKE_DEVICE_FAIL;
    }

    DHLOGI("Opening device finish: %{public}s", devicePath.c_str());
    return DH_SUCCESS;
}

void InputHub::RecordSkipDevicePath(std::string path)
{
    std::lock_guard<std::mutex> lock(skipDevicePathsMutex_);
    skipDevicePaths_.insert(path);
}

bool InputHub::IsSkipDevicePath(const std::string &path)
{
    std::lock_guard<std::mutex> lock(skipDevicePathsMutex_);
    return skipDevicePaths_.find(path) != skipDevicePaths_.end();
}

void InputHub::IncreaseLogTimes(const std::string& dhId)
{
    if (logTimesMap_.find(dhId) != logTimesMap_.end() && logTimesMap_[dhId] >= INT32_MAX - 1) {
        logTimesMap_[dhId] = 0;
    } else {
        logTimesMap_[dhId]++;
    }
}

bool InputHub::IsNeedPrintLog(const std::string& dhId) const
{
    return logTimesMap_.find(dhId) == logTimesMap_.end() || logTimesMap_.at(dhId) <= MAX_LOG_TIMES;
}

int32_t InputHub::QueryInputDeviceInfo(int fd, std::unique_ptr<Device> &device)
{
    char buffer[INPUT_EVENT_BUFFER_SIZE] = {0};
    // Get device name.
    if (ioctl(fd, EVIOCGNAME(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("Could not get device name for %{public}s", ConvertErrNo().c_str());
    } else {
        buffer[sizeof(buffer) - 1] = '\0';
        device->identifier.name = buffer;
    }
    DHLOGD("QueryInputDeviceInfo deviceName: %{public}s", buffer);
    // If the device is already a virtual device, don't monitor it.
    if (device->identifier.name.find(VIRTUAL_DEVICE_NAME) != std::string::npos) {
        DHLOGE("this is a virtual driver, skip it.");
        RecordSkipDevicePath(device->path);
        return ERR_DH_INPUT_HUB_IS_VIRTUAL_DEVICE;
    }
    // Get device driver version.
    int driverVersion;
    if (ioctl(fd, EVIOCGVERSION, &driverVersion)) {
        DHLOGE("could not get driver version for %{public}s\n", ConvertErrNo().c_str());
        RecordSkipDevicePath(device->path);
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    // Get device identifier.
    struct input_id inputId;
    if (ioctl(fd, EVIOCGID, &inputId)) {
        DHLOGE("could not get device input id for %{public}s\n", ConvertErrNo().c_str());
        RecordSkipDevicePath(device->path);
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    device->identifier.bus = inputId.bustype;
    device->identifier.product = inputId.product;
    device->identifier.vendor = inputId.vendor;
    device->identifier.version = inputId.version;
    // Get device physical physicalPath.
    if (ioctl(fd, EVIOCGPHYS(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("could not get physicalPath for %{public}s\n", ConvertErrNo().c_str());
    } else {
        buffer[sizeof(buffer) - 1] = '\0';
        device->identifier.physicalPath = buffer;
    }
    // Get device unique id.
    if (ioctl(fd, EVIOCGUNIQ(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("could not get idstring for %{public}s\n", ConvertErrNo().c_str());
    } else {
        buffer[sizeof(buffer) - 1] = '\0';
        device->identifier.uniqueId = buffer;
    }

    QueryEventInfo(fd, device);
    return DH_SUCCESS;
}

void InputHub::QueryEventInfo(int fd, std::unique_ptr<Device> &device)
{
    if (device == nullptr) {
        DHLOGE("device is nullptr!");
        return;
    }
    if (IsNeedPrintLog(device->identifier.descriptor)) {
        DHLOGI("QueryEventInfo: devName: %{public}s, dhId: %{public}s!", device->identifier.name.c_str(),
            GetAnonyString(device->identifier.descriptor).c_str());
    }
    struct libevdev *dev = GetLibEvDev(fd);
    if (dev == nullptr) {
        if (IsNeedPrintLog(device->identifier.descriptor)) {
            DHLOGE("dev is nullptr");
        }
        return;
    }
    GetEventTypes(dev, device->identifier);
    GetEventKeys(dev, device->identifier);
    GetABSInfo(dev, device->identifier);
    GetRELTypes(dev, device->identifier);
    GetProperties(dev, device->identifier);

    GetMSCBits(fd, device);
    GetLEDBits(fd, device);
    GetSwitchBits(fd, device);
    GetRepeatBits(fd, device);

    libevdev_free(dev);
}

void InputHub::GetEventMask(int fd, const std::string &eventName, uint32_t type,
    std::size_t arrayLength, uint8_t *whichBitMask) const
{
    int32_t rc = ioctl(fd, EVIOCGBIT(type, arrayLength), whichBitMask);
    if (rc < 0) {
        DHLOGE("Could not get events %{public}s mask: %{public}s", eventName.c_str(), strerror(errno));
    }
}

void InputHub::GetMSCBits(int fd, std::unique_ptr<Device> &device)
{
    uint8_t mscBitmask[NBYTES(MSC_MAX)] {};
    GetEventMask(fd, "msc", EV_MSC, sizeof(mscBitmask), mscBitmask);

    for (uint32_t msc = MSC_SERIAL; msc < MSC_MAX; ++msc) {
        if (TestBit(EV_MSC, device->evBitmask) && TestBit(msc, mscBitmask)) {
            DHLOGI("Get MSC event: %{public}d", msc);
            device->identifier.miscellaneous.push_back(msc);
        }
    }
}

void InputHub::GetLEDBits(int fd, std::unique_ptr<Device> &device)
{
    uint8_t ledBitmask[NBYTES(LED_MAX)] {};
    GetEventMask(fd, "led", EV_LED, sizeof(ledBitmask), ledBitmask);
    for (uint32_t led = LED_NUML; led < LED_MAX; ++led) {
        if (TestBit(EV_LED, device->evBitmask) && TestBit(led, ledBitmask)) {
            DHLOGI("Get LED event: %{public}d", led);
            device->identifier.leds.push_back(led);
        }
    }
}

void InputHub::GetSwitchBits(int fd, std::unique_ptr<Device> &device)
{
    uint8_t switchBitmask[NBYTES(SW_MAX)] {};
    GetEventMask(fd, "switch", EV_SW, sizeof(switchBitmask), switchBitmask);

    for (uint32_t sw = SW_LID; sw < SW_MAX; ++sw) {
        if (TestBit(EV_SW, device->evBitmask) && TestBit(sw, switchBitmask)) {
            DHLOGI("Get Switch event: %{public}d", sw);
            device->identifier.switchs.push_back(sw);
        }
    }
}

void InputHub::GetRepeatBits(int fd, std::unique_ptr<Device> &device)
{
    uint8_t repBitmask[NBYTES(REP_MAX)] {};
    GetEventMask(fd, "repeat", EV_REP, sizeof(repBitmask), repBitmask);

    for (uint32_t rep = REP_DELAY; rep < REP_MAX; ++rep) {
        if (TestBit(EV_REP, device->evBitmask) && TestBit(rep, repBitmask)) {
            DHLOGI("Get Repeat event: %{public}d", rep);
            device->identifier.repeats.push_back(rep);
        }
    }
}

struct libevdev* InputHub::GetLibEvDev(int fd)
{
    struct libevdev *dev = nullptr;
    int rc = 1;
    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        DHLOGE("Failed to init libevdev (%{public}s)", strerror(-rc));
        return nullptr;
    }
    return dev;
}

void InputHub::GetEventTypes(struct libevdev *dev, InputDevice &identifier)
{
    for (uint32_t eventType = 0; eventType < EV_CNT; eventType++) {
        if (!libevdev_has_event_type(dev, eventType)) {
            DHLOGD("The device is not support eventType: %{public}d", eventType);
            continue;
        }
        identifier.eventTypes.push_back(eventType);
    }
}

int32_t InputHub::GetEventKeys(struct libevdev *dev, InputDevice &identifier)
{
    if (!libevdev_has_event_type(dev, EV_KEY)) {
        if (IsNeedPrintLog(identifier.descriptor)) {
            DHLOGE("The device doesn't has EV_KEY type!");
        }
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    for (uint32_t eventKey = 0; eventKey < KEY_CNT; eventKey++) {
        if (!libevdev_has_event_code(dev, EV_KEY, eventKey)) {
            DHLOGD("The device is not support eventKey: %{public}d", eventKey);
            continue;
        }
        identifier.eventKeys.push_back(eventKey);
    }
    return DH_SUCCESS;
}

int32_t InputHub::GetABSInfo(struct libevdev *dev, InputDevice &identifier)
{
    if (!libevdev_has_event_type(dev, EV_ABS)) {
        if (IsNeedPrintLog(identifier.descriptor)) {
            DHLOGE("The device doesn't has EV_ABS type!");
        }
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    DHLOGI("The device has abs info, devName: %{public}s, dhId: %{public}s!",
        identifier.name.c_str(), GetAnonyString(identifier.descriptor).c_str());
    for (uint32_t absType = 0; absType < ABS_CNT; absType++) {
        if (!libevdev_has_event_code(dev, EV_ABS, absType)) {
            DHLOGD("The device is not support absType: %{public}d", absType);
            continue;
        }
        identifier.absTypes.push_back(absType);
        const struct input_absinfo *abs = libevdev_get_abs_info(dev, absType);
        if (abs == nullptr) {
            DHLOGE("absInfo is nullptr!");
            continue;
        }
        identifier.absInfos[absType].push_back(abs->value);
        identifier.absInfos[absType].push_back(abs->minimum);
        identifier.absInfos[absType].push_back(abs->maximum);
        identifier.absInfos[absType].push_back(abs->fuzz);
        identifier.absInfos[absType].push_back(abs->flat);
        identifier.absInfos[absType].push_back(abs->resolution);
    }
    return DH_SUCCESS;
}

int32_t InputHub::GetRELTypes(struct libevdev *dev, InputDevice &identifier)
{
    if (!libevdev_has_event_type(dev, EV_REL)) {
        if (IsNeedPrintLog(identifier.descriptor)) {
            DHLOGE("The device doesn't has EV_REL type!");
        }
        return ERR_DH_INPUT_HUB_QUERY_INPUT_DEVICE_INFO_FAIL;
    }
    for (uint32_t code = 0; code < REL_CNT; code++) {
        if (!libevdev_has_event_code(dev, EV_REL, code)) {
            DHLOGD("The device is not support rel code: %{public}d", code);
            continue;
        }
        identifier.relTypes.push_back(code);
    }
    return DH_SUCCESS;
}

void InputHub::GetProperties(struct libevdev *dev, InputDevice &identifier)
{
    for (uint32_t prop = 0; prop < INPUT_PROP_CNT; prop++) {
        if (libevdev_has_property(dev, prop)) {
            DHLOGI("QueryInputDeviceInfo rel prop: %{public}d", prop);
            identifier.properties.push_back(prop);
        }
    }
}

int32_t InputHub::MakeDevice(int fd, std::unique_ptr<Device> device)
{
    // See if this is a multi-touch touchscreen device.
    if (TestBit(BTN_TOUCH, device->keyBitmask) &&
        TestBit(ABS_MT_POSITION_X, device->absBitmask) &&
        TestBit(ABS_MT_POSITION_Y, device->absBitmask)) {
        QueryLocalTouchScreenInfo(fd, device);
        device->classes |= INPUT_DEVICE_CLASS_TOUCH | INPUT_DEVICE_CLASS_TOUCH_MT;
    } else if (TestBit(BTN_TOUCH, device->keyBitmask) &&
        TestBit(ABS_X, device->absBitmask) &&
        TestBit(ABS_Y, device->absBitmask)) {
        QueryLocalTouchScreenInfo(fd, device);
        device->classes |= INPUT_DEVICE_CLASS_TOUCH;
    }

    // See if this is a cursor device such as a trackball or mouse.
    if (TestBit(BTN_MOUSE, device->keyBitmask)
        && TestBit(REL_X, device->relBitmask)
        && TestBit(REL_Y, device->relBitmask)) {
        device->classes |= INPUT_DEVICE_CLASS_CURSOR;
    }

    // for Linux version 4.14.116, touchpad recognized as mouse and keyboard at same time,
    // need to avoid device->classes to be 0x09, which mmi can't handler.
    // See if this is a keyboard.
    if (device->classes == 0) {
        bool haveKeyboardKeys = ContainsNonZeroByte(device->keyBitmask, 0, SizeofBitArray(BTN_MISC));
        if (haveKeyboardKeys) {
            device->classes |= INPUT_DEVICE_CLASS_KEYBOARD;
        }
    }

    // If the device isn't recognized as something we handle, don't monitor it.
    if (device->classes == 0) {
        if (IsNeedPrintLog(device->identifier.descriptor)) {
            DHLOGI("Dropping device: name='%{public}s'", device->identifier.name.c_str());
        }
        return ERR_DH_INPUT_HUB_MAKE_DEVICE_FAIL;
    }

    if (RegisterDeviceForEpollLocked(*device) != DH_SUCCESS) {
        return ERR_DH_INPUT_HUB_MAKE_DEVICE_FAIL;
    }

    device->identifier.classes = device->classes;

    DHLOGI("inputType=%{public}d", inputTypes_.load());
    DHLOGI("New device: fd=%{public}d, name='%{public}s', classes=0x%{public}x", fd, device->identifier.name.c_str(),
        device->classes);

    AddDeviceLocked(std::move(device));
    return DH_SUCCESS;
}

int32_t InputHub::QueryLocalTouchScreenInfo(int fd, std::unique_ptr<Device> &device)
{
    LocalTouchScreenInfo info = DInputContext::GetInstance().GetLocalTouchScreenInfo();
    device->identifier.classes |= INPUT_DEVICE_CLASS_TOUCH_MT;
    info.localAbsInfo.deviceInfo = device->identifier;

    struct input_absinfo absInfo;
    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &absInfo);
    info.localAbsInfo.absXMin = absInfo.minimum;
    info.localAbsInfo.absXMax = absInfo.maximum;
    info.localAbsInfo.absMtPositionXMin = absInfo.minimum;
    info.localAbsInfo.absMtPositionXMax = absInfo.maximum;
    info.sinkPhyWidth = static_cast<uint32_t>(absInfo.maximum + 1);

    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &absInfo);
    info.localAbsInfo.absYMin = absInfo.minimum;
    info.localAbsInfo.absYMax = absInfo.maximum;
    info.localAbsInfo.absMtPositionYMin = absInfo.minimum;
    info.localAbsInfo.absMtPositionYMax = absInfo.maximum;
    info.sinkPhyHeight = static_cast<uint32_t>(absInfo.maximum + 1);

    ioctl(fd, EVIOCGABS(ABS_PRESSURE), &absInfo);
    info.localAbsInfo.absPressureMin = absInfo.minimum;
    info.localAbsInfo.absPressureMax = absInfo.maximum;
    info.localAbsInfo.absMtPressureMin = absInfo.minimum;
    info.localAbsInfo.absMtPressureMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_TOUCH_MAJOR), &absInfo);
    info.localAbsInfo.absMtTouchMajorMin = absInfo.minimum;
    info.localAbsInfo.absMtTouchMajorMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_TOUCH_MINOR), &absInfo);
    info.localAbsInfo.absMtTouchMinorMin = absInfo.minimum;
    info.localAbsInfo.absMtTouchMinorMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_ORIENTATION), &absInfo);
    info.localAbsInfo.absMtOrientationMin = absInfo.minimum;
    info.localAbsInfo.absMtOrientationMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_BLOB_ID), &absInfo);
    info.localAbsInfo.absMtBlobIdMin = absInfo.minimum;
    info.localAbsInfo.absMtBlobIdMax = absInfo.maximum;

    ioctl(fd, EVIOCGABS(ABS_MT_TRACKING_ID), &absInfo);
    info.localAbsInfo.absMtTrackingIdMin = absInfo.minimum;
    info.localAbsInfo.absMtTrackingIdMax = absInfo.maximum;

    DInputContext::GetInstance().SetLocalTouchScreenInfo(info);
    return DH_SUCCESS;
}

std::string InputHub::StringPrintf(const char *format, ...) const
{
    char space[SPACELENGTH] = {0};
    va_list ap;
    va_start(ap, format);
    std::string result;
    int32_t ret = vsnprintf_s(space, sizeof(space), sizeof(space) - 1, format, ap);
    if (ret >= DH_SUCCESS && static_cast<size_t>(ret) < sizeof(space)) {
        result = space;
    } else {
        va_end(ap);
        return "the buffer is overflow!";
    }
    va_end(ap);
    return result;
}

void InputHub::GenerateDescriptor(InputDevice &identifier) const
{
    std::string rawDescriptor;
    rawDescriptor += StringPrintf(":%04x:%04x:", identifier.vendor,
        identifier.product);
    // add handling for USB devices to not uniqueify kbs that show up twice
    if (!identifier.uniqueId.empty()) {
        rawDescriptor += "uniqueId:";
        rawDescriptor += identifier.uniqueId;
    }
    if (!identifier.physicalPath.empty()) {
        rawDescriptor += "physicalPath:";
        rawDescriptor += identifier.physicalPath;
    }
    if (!identifier.name.empty()) {
        rawDescriptor += "name:";
        std::string name = identifier.name;
        rawDescriptor += regex_replace(name, std::regex(" "), "");
    }

    identifier.descriptor = DH_ID_PREFIX + Sha256(rawDescriptor);
    if (IsNeedPrintLog(identifier.descriptor)) {
        DHLOGI("Created descriptor: raw=%{public}s, cooked=%{public}s", rawDescriptor.c_str(),
            GetAnonyString(identifier.descriptor).c_str());
    }
}

int32_t InputHub::RegisterDeviceForEpollLocked(const Device &device)
{
    int32_t result = RegisterFdForEpoll(device.fd);
    if (result != DH_SUCCESS) {
        DHLOGE("Could not add input device fd to epoll for device, path: %{public}s", device.path.c_str());
        return result;
    }
    return result;
}

int32_t InputHub::RegisterFdForEpoll(int fd)
{
    struct epoll_event eventItem = {};
    eventItem.events = EPOLLIN | EPOLLWAKEUP;
    eventItem.data.fd = fd;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &eventItem)) {
        DHLOGE("Could not add fd to epoll instance: %{public}s", ConvertErrNo().c_str());
        return -errno;
    }
    return DH_SUCCESS;
}

void InputHub::AddDeviceLocked(std::unique_ptr<Device> device)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    openingDevices_.push_back(std::move(device));
}

void InputHub::CloseDeviceLocked(Device &device)
{
    DHLOGI("Removed device: path=%{public}s name=%{public}s fd=%{public}d classes=0x%{public}x",
        device.path.c_str(), device.identifier.name.c_str(), device.fd, device.classes);

    UnregisterDeviceFromEpollLocked(device);
    device.Close();
    {
        std::lock_guard<std::mutex> devicesLock(devicesMutex_);
        closingDevices_.push_back(std::move(devices_[device.path]));
        devices_.erase(device.path);
    }
}

void InputHub::CloseDeviceForAllLocked(Device &device)
{
    DHLOGI("Removed device: path=%{public}s name=%{public}s fd=%{public}d classes=0x%{public}x",
        device.path.c_str(), device.identifier.name.c_str(), device.fd, device.classes);

    UnregisterDeviceFromEpollLocked(device);
    device.Close();
    closingDevices_.push_back(std::move(devices_[device.path]));
    devices_.erase(device.path);
}

int32_t InputHub::UnregisterDeviceFromEpollLocked(const Device &device) const
{
    if (device.HasValidFd()) {
        int32_t result = UnregisterFdFromEpoll(device.fd);
        if (result != DH_SUCCESS) {
            DHLOGE("Could not remove input device fd from epoll for device, path: %{public}s", device.path.c_str());
            return result;
        }
    }
    return DH_SUCCESS;
}

int32_t InputHub::UnregisterFdFromEpoll(int fd) const
{
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr)) {
        DHLOGE("Could not remove fd from epoll instance: %{public}s", ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_UNREGISTER_FD_FAIL;
    }
    return DH_SUCCESS;
}

int32_t InputHub::ReadNotifyLocked()
{
    size_t res;
    char eventBuf[512];
    struct inotify_event *event;

    DHLOGI("readNotify nfd: %{public}d\n", iNotifyFd_);
    res = static_cast<size_t>(read(iNotifyFd_, eventBuf, sizeof(eventBuf)));
    if (res < sizeof(*event)) {
        if (errno == EINTR) {
            return DH_SUCCESS;
        }
        DHLOGE("could not get event, %{public}s\n", ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_GET_EVENT_FAIL;
    }

    {
        size_t eventSize = 0;
        size_t eventPos = 0;
        while (res >= sizeof(*event)) {
            event = reinterpret_cast<struct inotify_event *>(eventBuf + eventPos);
            JudgeDeviceOpenOrClose(*event);
            eventSize = sizeof(*event) + event->len;
            res -= eventSize;
            eventPos += eventSize;
        }
    }
    return DH_SUCCESS;
}

void InputHub::JudgeDeviceOpenOrClose(const inotify_event &event)
{
    if (event.len) {
        if (event.wd == inputWd_) {
            std::string filename = std::string(DEVICE_PATH) + "/" + event.name;
            if (event.mask & IN_CREATE) {
                OpenInputDeviceLocked(filename);
            } else {
                DHLOGI("Removing device '%{public}s' due to inotify event\n", filename.c_str());
                CloseDeviceByPathLocked(filename);
            }
        } else {
            DHLOGI("Unexpected inotify event, wd = %i", event.wd);
        }
    }
}

void InputHub::CloseDeviceByPathLocked(const std::string &devicePath)
{
    Device* device = GetDeviceByPathLocked(devicePath);
    if (device) {
        CloseDeviceLocked(*device);
        return;
    }
    DHLOGI("Remove device: %{public}s not found, device may already have been removed.", devicePath.c_str());
}

void InputHub::CloseAllDevicesLocked()
{
    DHLOGI("Close All Devices");
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    while (!devices_.empty()) {
        CloseDeviceForAllLocked(*(devices_.begin()->second));
    }
}

InputHub::Device* InputHub::GetDeviceByPathLocked(const std::string &devicePath)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->path == devicePath) {
            return device.get();
        }
    }
    return nullptr;
}

InputHub::Device* InputHub::GetDeviceByFdLocked(int fd)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->fd == fd) {
            return device.get();
        }
    }
    return nullptr;
}

InputHub::Device* InputHub::GetSupportDeviceByFd(int fd)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device != nullptr && device->fd == fd) {
            return device.get();
        }
    }
    return nullptr;
}

bool InputHub::ContainsNonZeroByte(const uint8_t *array, uint32_t startIndex, uint32_t endIndex)
{
    const uint8_t* end = array + endIndex;
    array += startIndex;
    while (array != end) {
        if (*(array++) != 0) {
            return true;
        }
    }
    return false;
}

int64_t InputHub::ProcessEventTimestamp(const input_event &event)
{
    const int64_t inputEventTime = event.input_event_sec * 1000000000LL + event.input_event_usec * 1000LL;
    return inputEventTime;
}

bool InputHub::TestBit(uint32_t bit, const uint8_t *array)
{
    constexpr int units = 8;
    return (array)[(bit) / units] & (1 << ((bit) % units));
}

uint32_t InputHub::SizeofBitArray(uint32_t bit)
{
    constexpr int round = 7;
    constexpr int divisor = 8;
    return ((bit) + round) / divisor;
}

void InputHub::SaveAffectDhId(bool isEnable, const std::string &dhId, AffectDhIds &affDhIds)
{
    if (isEnable) {
        affDhIds.sharingDhIds.push_back(dhId);
    } else {
        affDhIds.noSharingDhIds.push_back(dhId);
    }
}

AffectDhIds InputHub::SetSupportInputType(bool enabled, const uint32_t &inputTypes)
{
    AffectDhIds affDhIds;
    inputTypes_ = inputTypes;
    DHLOGI("SetSupportInputType: inputTypes=0x%x,", inputTypes_.load());
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->classes & inputTypes_) {
            device->isShare = enabled;
            DHLOGW("ByType dhid:%{public}s, isshare:%{public}d",
                GetAnonyString(device->identifier.descriptor).c_str(), enabled);
            SaveAffectDhId(enabled, device->identifier.descriptor, affDhIds);
        }
    }

    return affDhIds;
}

AffectDhIds InputHub::SetSharingDevices(bool enabled, std::vector<std::string> dhIds)
{
    AffectDhIds affDhIds;
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    DHLOGI("SetSharingDevices start");
    for (auto dhId : dhIds) {
        DHLOGI("SetSharingDevices dhId: %{public}s, size: %{public}zu, enabled: %{public}d",
            GetAnonyString(dhId).c_str(), devices_.size(), enabled);
        sharedDHIds_[dhId] = enabled;
        for (const auto &[id, device] : devices_) {
            DHLOGI("deviceName %{public}s ,dhId: %{public}s ", device->identifier.name.c_str(),
                GetAnonyString(device->identifier.descriptor).c_str());
            if (device->identifier.descriptor == dhId) {
                device->isShare = enabled;
                DHLOGW("dhid:%{public}s, isshare:%{public}d",
                    GetAnonyString(device->identifier.descriptor).c_str(), enabled);
                SaveAffectDhId(enabled, device->identifier.descriptor, affDhIds);
                break;
            }
        }
    }
    DHLOGI("SetSharingDevices end");
    return affDhIds;
}

std::vector<std::string> InputHub::GetSharingDevices()
{
    std::vector<std::string> sharingDevices;
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->isShare) {
            DHLOGI("Find sharing dhid: %{public}s", GetAnonyString(device->identifier.descriptor).c_str());
            sharingDevices.push_back(device->identifier.descriptor);
        }
    }
    return sharingDevices;
}

void InputHub::GetSharedMousePathByDhId(const std::vector<std::string> &dhIds, std::string &sharedMousePath,
    std::string &sharedMouseDhId)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    DHLOGI("GetSharedMousePathByDhId: devices_.size:%{public}zu,", devices_.size());
    for (const auto &dhId : dhIds) {
        for (const auto &[id, device] : devices_) {
            if (device == nullptr) {
                DHLOGE("device is nullptr");
                continue;
            }
            DHLOGI("descriptor:%{public}s, isShare[%{public}d], type[%{public}d]",
                GetAnonyString(device->identifier.descriptor).c_str(), device->isShare, device->classes);
            if ((device->identifier.descriptor == dhId) && ((device->classes & INPUT_DEVICE_CLASS_CURSOR) != 0 ||
                (device->classes & INPUT_DEVICE_CLASS_TOUCH) != 0 ||
                ((device->classes & INPUT_DEVICE_CLASS_TOUCH_MT) != 0 && IsTouchPad(device->identifier)))) {
                sharedMouseDhId = dhId;
                sharedMousePath = device->path;
                return; // return First shared mouse
            }
        }
    }
}

void InputHub::GetSharedKeyboardPathsByDhIds(const std::vector<std::string> &dhIds,
    std::vector<std::string> &sharedKeyboardPaths, std::vector<std::string> &sharedKeyboardDhIds)
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    DHLOGI("GetSharedKeyboardPathsByDhIds: devices_.size:%{public}zu,", devices_.size());
    for (const auto &dhId : dhIds) {
        for (const auto &[id, device] : devices_) {
            if (device == nullptr) {
                DHLOGE("device is nullptr");
                continue;
            }
            DHLOGI("descriptor:%{public}s, isShare[%{public}d], type[%{public}d]",
                GetAnonyString(device->identifier.descriptor).c_str(), device->isShare, device->classes);
            if ((device->identifier.descriptor == dhId) &&
                ((device->classes & INPUT_DEVICE_CLASS_KEYBOARD) != 0)) {
                sharedKeyboardDhIds.push_back(dhId);
                sharedKeyboardPaths.push_back(device->path);
            }
        }
    }
}

void InputHub::GetDevicesInfoByType(const uint32_t inputTypes, std::map<int32_t, std::string> &datas)
{
    uint32_t dhType = 0;

    if ((inputTypes & static_cast<uint32_t>(DInputDeviceType::MOUSE)) != 0) {
        dhType |= INPUT_DEVICE_CLASS_CURSOR;
    }

    if ((inputTypes & static_cast<uint32_t>(DInputDeviceType::KEYBOARD)) != 0) {
        dhType |= INPUT_DEVICE_CLASS_KEYBOARD;
    }

    if ((inputTypes & static_cast<uint32_t>(DInputDeviceType::TOUCHSCREEN)) != 0) {
        dhType |= INPUT_DEVICE_CLASS_TOUCH | INPUT_DEVICE_CLASS_TOUCH_MT;
    }

    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[id, device] : devices_) {
        if (device->classes & dhType) {
            datas.insert(std::pair<int32_t, std::string>(device->fd, device->identifier.descriptor));
        }
    }
}

void InputHub::GetDevicesInfoByDhId(std::vector<std::string> dhidsVec, std::map<int32_t, std::string> &datas)
{
    for (auto dhId : dhidsVec) {
        std::lock_guard<std::mutex> deviceLock(devicesMutex_);
        for (const auto &[id, device] : devices_) {
            if (device->identifier.descriptor == dhId) {
                datas.insert(std::pair<int32_t, std::string>(device->fd, dhId));
            }
        }
    }
}

bool InputHub::IsAllDevicesStoped()
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    for (const auto &[dhId, isShared] : sharedDHIds_) {
        DHLOGI("the dhId: %{public}s, isShared: %{public}d", GetAnonyString(dhId).c_str(), isShared);
        if (isShared) {
            return false;
        }
    }
    return true;
}

void InputHub::RecordDeviceLog(const std::string &devicePath, const InputDevice &identifier)
{
    if (IsNeedPrintLog(identifier.descriptor)) {
        DHLOGI("add device: %{public}s\n", devicePath.c_str());
        DHLOGI("  bus:        %{public}04x\n"
               "  vendor      %{public}04x\n"
               "  product     %{public}04x\n"
               "  version     %{public}04x\n",
               identifier.bus, identifier.vendor, identifier.product, identifier.version);
        DHLOGI("  name:       \"%{public}s\"\n", identifier.name.c_str());
        DHLOGI("  physicalPath:   \"%{public}s\"\n", identifier.physicalPath.c_str());
        DHLOGI("  unique id:  \"%{public}s\"\n", identifier.uniqueId.c_str());
        DHLOGI("  descriptor: \"%{public}s\"\n", GetAnonyString(identifier.descriptor).c_str());
    }
}

void InputHub::RecordChangeEventLog(const RawEvent &event)
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
        case EV_SYN:
            eventType = "EV_SYN";
            break;
        default:
            eventType = "other type " + std::to_string(event.type);
            break;
    }
    DHLOGI("0.E2E-Test Sink collect change event, EventType: %{public}s, Code: %{public}d, Value: %{public}d, "
        "Path: %{public}s, descriptor: %{public}s, When: %{public}" PRId64 "", eventType.c_str(), event.code,
        event.value, event.path.c_str(), GetAnonyString(event.descriptor).c_str(), event.when);
}

void InputHub::RecordEventLog(const RawEvent *event)
{
    std::string eventType = "";
    switch (event->type) {
        case EV_KEY:
            eventType = "EV_KEY";
            break;
        case EV_REL:
            eventType = "EV_REL";
            break;
        case EV_ABS:
            eventType = "EV_ABS";
            break;
        case EV_SYN:
            eventType = "EV_SYN";
            break;
        default:
            eventType = "other type " + std::to_string(event->type);
            break;
    }
    DHLOGD("1.E2E-Test Sink collect event, EventType: %{public}s, Code: %{public}d, Value: %{public}d, "
        "Path: %{public}s, descriptor: %{public}s, When: %{public}" PRId64 "", eventType.c_str(), event->code,
        event->value, event->path.c_str(), GetAnonyString(event->descriptor).c_str(), event->when);
}

void InputHub::HandleTouchScreenEvent(struct input_event readBuffer[], const size_t count,
    std::vector<bool> &needFilted)
{
    std::vector<std::pair<size_t, size_t>> absIndexs;
    int32_t firstIndex = -1;
    int32_t lastIndex = -1;

    for (size_t i = 0; i < count; i++) {
        struct input_event& iev = readBuffer[i];
        if ((iev.type == EV_ABS) && (iev.code == ABS_X || iev.code == ABS_MT_POSITION_X)) {
            firstIndex = static_cast<int32_t>(i);
        } else if (iev.type  == EV_SYN) {
            lastIndex = static_cast<int32_t>(i);
        }
        if ((firstIndex >= 0) && (lastIndex > firstIndex)) {
            absIndexs.emplace_back(std::make_pair((size_t)firstIndex, (size_t)lastIndex));
        }
    }

    AbsInfo absInfo = {
        .absX = 0,
        .absY = 0,
        .absXIndex = -1,
        .absYIndex = -1,
    };
    for (auto iter : absIndexs) {
        absInfo.absXIndex = -1;
        absInfo.absYIndex = -1;

        for (size_t j = iter.first; j <= iter.second; j++) {
            struct input_event &iev = readBuffer[j];
            if (iev.code == ABS_X || iev.code == ABS_MT_POSITION_X) {
                absInfo.absX = static_cast<uint32_t>(iev.value);
                absInfo.absXIndex = static_cast<int32_t>(j);
            }
            if (iev.code == ABS_Y || iev.code == ABS_MT_POSITION_Y) {
                absInfo.absY = static_cast<uint32_t>(iev.value);
                absInfo.absYIndex = static_cast<int32_t>(j);
            }
        }

        if ((absInfo.absXIndex < 0) || (absInfo.absYIndex < 0)) {
            for (size_t j = iter.first; j <= iter.second; j++) {
                needFilted[j] = true;
            }
            continue;
        }
        if (!CheckTouchPointRegion(readBuffer, absInfo)) {
            for (size_t j = iter.first; j <= iter.second; j++) {
                needFilted[j] = true;
            }
        }
    }
}

bool InputHub::CheckTouchPointRegion(struct input_event readBuffer[], const AbsInfo &absInfo)
{
    auto sinkInfos = DInputContext::GetInstance().GetAllSinkScreenInfo();

    for (const auto &[id, sinkInfo] : sinkInfos) {
        auto info = sinkInfo.transformInfo;
        if ((absInfo.absX >= info.sinkWinPhyX) && (absInfo.absX <= (info.sinkWinPhyX + info.sinkProjPhyWidth))
            && (absInfo.absY >= info.sinkWinPhyY) && (absInfo.absY <= (info.sinkWinPhyY + info.sinkProjPhyHeight))) {
            touchDescriptor = sinkInfo.srcScreenInfo.sourcePhyId;
            readBuffer[absInfo.absXIndex].value = (absInfo.absX - info.sinkWinPhyX) * info.coeffWidth;
            readBuffer[absInfo.absYIndex].value = (absInfo.absY - info.sinkWinPhyY) * info.coeffHeight;
            return true;
        }
    }
    return false;
}

std::vector<InputHub::Device*> InputHub::CollectTargetDevices()
{
    std::lock_guard<std::mutex> deviceLock(devicesMutex_);
    std::vector<InputHub::Device*> tarVec;
    for (const auto &dev : devices_) {
        if (((dev.second->classes & INPUT_DEVICE_CLASS_TOUCH) != 0) ||
            ((dev.second->classes & INPUT_DEVICE_CLASS_TOUCH_MT) != 0) ||
            ((dev.second->classes & INPUT_DEVICE_CLASS_CURSOR) != 0) ||
            ((dev.second->classes & INPUT_DEVICE_CLASS_KEYBOARD) != 0)) {
            DHLOGI("Find target devs need check stat, path: %{public}s, name: %{public}s",
                dev.first.c_str(), dev.second->identifier.name.c_str());
            tarVec.push_back(dev.second.get());
        }
    }

    return tarVec;
}

void InputHub::SavePressedKeyState(const InputHub::Device *dev, int32_t keyCode)
{
    struct RawEvent event = {
        .type = EV_KEY,
        .code = keyCode,
        .value = KEY_DOWN_STATE,
        .descriptor = dev->identifier.descriptor,
        .path = dev->path
    };
    DInputSinkState::GetInstance().AddKeyDownState(event);
    DHLOGI("Find Pressed key: %{public}d, device path: %{public}s, dhId: %{public}s", keyCode, dev->path.c_str(),
        GetAnonyString(dev->identifier.descriptor).c_str());
}

bool InputHub::IsLengthExceeds(const unsigned long *keyState, const unsigned long len, int keyIndex)
{
    if (len < (keyIndex / LONG_BITS) + 1) {
        DHLOGE("Length exceeds for key index: %{public}d", keyIndex);
        return true;
    }
    return false;
}

void InputHub::CheckTargetKeyState(const InputHub::Device *dev, const unsigned long *keyState, const unsigned long len)
{
    // If device is a mouse, record the mouse pressed key.
    if ((dev->classes & INPUT_DEVICE_CLASS_CURSOR) != 0) {
        if (IsLengthExceeds(keyState, len, BTN_LEFT)) {
            return;
        }
        int mouseLeftBtnState = BitIsSet(keyState, BTN_LEFT);
        if (mouseLeftBtnState != 0) {
            SavePressedKeyState(dev, BTN_LEFT);
        }
        if (IsLengthExceeds(keyState, len, BTN_RIGHT)) {
            return;
        }
        int mouseRightBtnState = BitIsSet(keyState, BTN_RIGHT);
        if (mouseRightBtnState != 0) {
            SavePressedKeyState(dev, BTN_RIGHT);
        }
        if (IsLengthExceeds(keyState, len, BTN_MIDDLE)) {
            return;
        }
        int mouseMidBtnState = BitIsSet(keyState, BTN_MIDDLE);
        if (mouseMidBtnState != 0) {
            SavePressedKeyState(dev, BTN_MIDDLE);
        }
    }

    // If device is a keyboard, record all the pressed keys.
    if ((dev->classes & INPUT_DEVICE_CLASS_KEYBOARD) != 0) {
        for (int32_t keyIndex = 0; keyIndex < KEY_MAX; keyIndex++) {
            if (IsLengthExceeds(keyState, len, keyIndex)) {
                return;
            }
            if (BitIsSet(keyState, keyIndex) != 0) {
                SavePressedKeyState(dev, keyIndex);
            }
        }
    }

    // If device is a touchscreen or touchpad, record the touch event.
    if ((dev->classes & INPUT_DEVICE_CLASS_TOUCH) != 0 || (dev->classes & INPUT_DEVICE_CLASS_TOUCH_MT) != 0) {
        if (IsLengthExceeds(keyState, len, BTN_TOUCH)) {
            return;
        }
        int btnTouchState = BitIsSet(keyState, BTN_TOUCH);
        if (btnTouchState != 0) {
            SavePressedKeyState(dev, BTN_TOUCH);
        }
    }
}


void InputHub::CheckTargetDevicesState(std::vector<InputHub::Device*> targetDevices)
{
    uint32_t count = 0;
    unsigned long keyState[NLONGS(KEY_CNT)] = { 0 };
    for (const auto *dev : targetDevices) {
        while (true) {
            if (count > READ_RETRY_MAX) {
                break;
            }
            // Query all key state
            int rc = ioctl(dev->fd, EVIOCGKEY(sizeof(keyState)), keyState);
            if (rc < 0) {
                DHLOGE("read all key state failed, rc=%{public}d", rc);
                count += 1;
                std::this_thread::sleep_for(std::chrono::milliseconds(READ_SLEEP_TIME_MS));
                continue;
            }
            CheckTargetKeyState(dev, keyState, NLONGS(KEY_CNT));
            break;
        }
    }
}

void InputHub::RecordDeviceStates()
{
    DHLOGI("Start Record keys states");
    ScanAndRecordInputDevices();
    std::vector<InputHub::Device*> tarDevices = CollectTargetDevices();
    DHLOGI("Check target states device num: %{public}zu", tarDevices.size());
    CheckTargetDevicesState(tarDevices);
    DHLOGI("Finish Record Keys states");
}

void InputHub::ClearDeviceStates()
{
    DHLOGI("Clear Device state");
    DInputSinkState::GetInstance().ClearDeviceStates();
}

void InputHub::ClearSkipDevicePaths()
{
    DHLOGI("Clear Skip device path");
    std::lock_guard<std::mutex> lock(skipDevicePathsMutex_);
    skipDevicePaths_.clear();
}

InputHub::Device::Device(int fd, const std::string &path)
    : next(nullptr), fd(fd), path(path), identifier({}), classes(0), enabled(false),
      isShare(false), isVirtual(fd < 0) {
    // Figure out the kinds of events the device reports.
    DHLOGI("Ctor Device for get event mask, fd: %{public}d, path: %{public}s", fd, path.c_str());
    ioctl(fd, EVIOCGBIT(0, sizeof(evBitmask)), evBitmask);
    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBitmask)), keyBitmask);
    ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBitmask)), absBitmask);
    ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relBitmask)), relBitmask);
}

InputHub::Device::~Device()
{
    Close();
}

void InputHub::Device::Close()
{
    if (fd >= 0) {
        ::close(fd);
        fd = UN_INIT_FD_VALUE;
    }
}

int32_t InputHub::Device::Enable()
{
    char canonicalPath[PATH_MAX + 1] = {0x00};

    if (path.length() == 0 || path.length() > PATH_MAX || realpath(path.c_str(), canonicalPath) == nullptr) {
        DHLOGE("path check fail, error path: %{public}s", path.c_str());
        return ERR_DH_INPUT_HUB_DEVICE_ENABLE_FAIL;
    }
    fd = open(canonicalPath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
    if (fd < 0) {
        DHLOGE("could not open %{public}s, %{public}s\n", path.c_str(), ConvertErrNo().c_str());
        return ERR_DH_INPUT_HUB_DEVICE_ENABLE_FAIL;
    }
    enabled = true;
    return DH_SUCCESS;
}

int32_t InputHub::Device::Disable()
{
    Close();
    enabled = false;
    return DH_SUCCESS;
}

bool InputHub::Device::HasValidFd() const
{
    return !isVirtual && enabled;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
