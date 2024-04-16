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

#ifndef INPUT_HUB_H
#define INPUT_HUB_H

#include <atomic>
#include <mutex>
#include <map>
#include <memory>
#include <set>
#include <unordered_map>

#include <libevdev/libevdev.h>
#include <linux/input.h>
#include <sys/epoll.h>
#include <sys/inotify.h>

#include "constants_dinput.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
struct AffectDhIds {
    std::vector<std::string> sharingDhIds;
    std::vector<std::string> noSharingDhIds;
};

inline constexpr size_t BITS_PER_UINT8 { 8 };
inline constexpr size_t NBYTES(size_t nbits)
{
    return (nbits + BITS_PER_UINT8 - 1) / BITS_PER_UINT8;
}

class InputHub {
public:
    struct Device  {
        Device* next;
        int fd; // may be -1 if device is closed
        const std::string path;
        InputDevice identifier;
        uint32_t classes;
        uint8_t evBitmask[NBYTES(EV_MAX)] {};
        uint8_t keyBitmask[NBYTES(KEY_MAX)] {};
        uint8_t absBitmask[NBYTES(ABS_MAX)] {};
        uint8_t relBitmask[NBYTES(REL_MAX)] {};

        Device(int fd, const std::string &path);
        ~Device();
        void Close();
        bool enabled; // initially true
        bool isShare;
        int32_t Enable();
        int32_t Disable();
        bool HasValidFd() const;
        const bool isVirtual; // set if fd < 0 is passed to constructor
    };

    struct AbsInfo {
        uint32_t absX;
        uint32_t absY;
        int32_t absXIndex;
        int32_t absYIndex;
    };

    explicit InputHub(bool isPluginMonitor);
    ~InputHub();
    size_t StartCollectInputEvents(RawEvent *buffer, size_t bufferSize);
    size_t StartCollectInputHandler(InputDeviceEvent *buffer, size_t bufferSize);
    void StopCollectInputEvents();
    void StopCollectInputHandler();
    size_t DeviceIsExists(InputDeviceEvent *buffer, size_t bufferSize);
    std::vector<InputDevice> GetAllInputDevices();
    // return efftive dhids
    AffectDhIds SetSupportInputType(bool enabled, const uint32_t &inputTypes);
    // return efftive dhids
    AffectDhIds SetSharingDevices(bool enabled, std::vector<std::string> dhIds);
    std::vector<std::string> GetSharingDevices();
    void GetDevicesInfoByType(const uint32_t inputTypes, std::map<int32_t, std::string> &datas);
    void GetDevicesInfoByDhId(std::vector<std::string> dhidsVec, std::map<int32_t, std::string> &datas);
    void GetSharedMousePathByDhId(const std::vector<std::string> &dhIds, std::string &sharedMousePath,
        std::string &sharedMouseDhId);
    void GetSharedKeyboardPathsByDhIds(const std::vector<std::string> &dhIds,
        std::vector<std::string> &sharedKeyboardPaths, std::vector<std::string> &sharedKeyboardDhIds);
    bool IsAllDevicesStoped();
    void ScanInputDevices(const std::string &dirName);

    void RecordDeviceStates();
    void CheckTargetDevicesState(std::vector<Device*> targetDevices);
    bool IsLengthExceeds(const unsigned long *keyState, const unsigned long len, int keyIndex);
    void CheckTargetKeyState(const InputHub::Device *dev, const unsigned long *keyState, const unsigned long len);
    void SavePressedKeyState(const Device *dev, int32_t keyCode);
    void ClearDeviceStates();
    void ClearSkipDevicePaths();
    /*
     * Scan the input device node and save info.
     */
    void ScanAndRecordInputDevices();

private:
    int32_t Initialize();
    int32_t Release();

    size_t GetEvents(RawEvent *buffer, size_t bufferSize);
    size_t ReadInputEvent(int32_t readSize, Device &device);
    void GetDeviceHandler();
    int32_t RefreshEpollItem(bool isSleep);

    int32_t OpenInputDeviceLocked(const std::string &devicePath);
    int32_t QueryInputDeviceInfo(int fd, std::unique_ptr<Device> &device);
    void QueryEventInfo(int fd, std::unique_ptr<Device> &device);
    struct libevdev* GetLibEvDev(int fd);
    void GetEventTypes(struct libevdev *dev, InputDevice &identifier);
    int32_t GetEventKeys(struct libevdev *dev, InputDevice &identifier);
    int32_t GetABSInfo(struct libevdev *dev, InputDevice &identifier);
    int32_t GetRELTypes(struct libevdev *dev, InputDevice &identifier);
    void GetProperties(struct libevdev *dev, InputDevice &identifier);

    void GetMSCBits(int fd, std::unique_ptr<Device> &device);
    void GetLEDBits(int fd, std::unique_ptr<Device> &device);
    void GetSwitchBits(int fd, std::unique_ptr<Device> &device);
    void GetRepeatBits(int fd, std::unique_ptr<Device> &device);

    void GetEventMask(int fd, const std::string &eventName, uint32_t type,
        std::size_t arrayLength, uint8_t *whichBitMask) const;

    int32_t MakeDevice(int fd, std::unique_ptr<Device> device);
    void GenerateDescriptor(InputDevice &identifier) const;
    std::string StringPrintf(const char *format, ...) const;

    int32_t RegisterFdForEpoll(int fd);
    int32_t RegisterDeviceForEpollLocked(const Device &device);
    void AddDeviceLocked(std::unique_ptr<Device> device);
    void CloseDeviceLocked(Device &device);
    void CloseDeviceForAllLocked(Device &device);
    int32_t UnregisterDeviceFromEpollLocked(const Device &device) const;
    int32_t UnregisterFdFromEpoll(int fd) const;
    int32_t ReadNotifyLocked();
    void CloseDeviceByPathLocked(const std::string &devicePath);
    void CloseAllDevicesLocked();
    void JudgeDeviceOpenOrClose(const inotify_event &event);
    Device* GetDeviceByPathLocked(const std::string &devicePath);
    Device* GetDeviceByFdLocked(int fd);
    Device* GetSupportDeviceByFd(int fd);
    bool IsDeviceRegistered(const std::string &devicePath);

    bool ContainsNonZeroByte(const uint8_t *array, uint32_t startIndex, uint32_t endIndex);
    int64_t ProcessEventTimestamp(const input_event &event);
    bool IsCuror(Device *device);
    bool IsTouchPad(const InputDevice &inputDevice);
    bool IsTouchPad(Device *device);

    /*
     * this macro is used to tell if "bit" is set in "array"
     * it selects a byte from the array, and does a boolean AND
     * operation with a byte that only has the relevant bit set.
     * eg. to check for the 12th bit, we do (array[1] & 1<<4)
     */
    bool TestBit(uint32_t bit, const uint8_t *array);
    /* this macro computes the number of bytes needed to represent a bit array of the specified size */
    uint32_t SizeofBitArray(uint32_t bit);
    void RecordEventLog(const RawEvent *event);
    // Record Event log that will change the key state.
    void RecordChangeEventLog(const RawEvent &event);
    void RecordDeviceLog(const std::string &devicePath, const InputDevice &identifier);
    void HandleTouchScreenEvent(struct input_event readBuffer[], const size_t count, std::vector<bool> &needFilted);
    int32_t QueryLocalTouchScreenInfo(int fd, std::unique_ptr<Device> &device);
    bool CheckTouchPointRegion(struct input_event readBuffer[], const AbsInfo &absInfo);
    size_t CollectEvent(RawEvent *buffer, size_t &capacity, Device *device, struct input_event readBuffer[],
        const size_t count);
    /*
     * isEnable: true for sharing dhid, false for no sharing dhid
     */
    void SaveAffectDhId(bool isEnable, const std::string &dhId, AffectDhIds &affDhIds);
    /*
     * Record Mouse/KeyBoard/TouchPad state such as key down.
     */
    void RecordDeviceChangeStates(Device *device, struct input_event readBuffer[], const size_t count);
    void MatchAndDealEvent(Device *device, const RawEvent &event);
    void DealTouchPadEvent(const RawEvent &event);
    void DealNormalKeyEvent(Device *device, const RawEvent &event);

    /*
     * Check is this node has been scaned for collecting info.
     * Return: True for scaned and cached. False for not scaned.
     */
    bool IsInputNodeNoNeedScan(const std::string &path);

    /*
     * Some input device should simulate state for pass through, such as Mouse, KeyBoard, TouchPad, etc.
     * Before we prepare the pass through, we need check and get the key states of these devices.
     */
    std::vector<Device*> CollectTargetDevices();

    void RecordSkipDevicePath(std::string path);
    bool IsSkipDevicePath(const std::string &path);
    void IncreaseLogTimes(const std::string& dhId);
    bool IsNeedPrintLog(const std::string& dhId) const;

private:
    int epollFd_;
    int iNotifyFd_;
    int inputWd_;
    /*
     * true: for just monitor device plugin/unplugin;
     * false: for read device events.
     */
    bool isPluginMonitor_;

    std::vector<std::unique_ptr<Device>> openingDevices_;
    std::vector<std::unique_ptr<Device>> closingDevices_;
    std::unordered_map<std::string, std::unique_ptr<Device>> devices_;
    std::mutex devicesMutex_;

    std::mutex skipDevicePathsMutex_;
    std::set<std::string> skipDevicePaths_;

    std::atomic<bool> needToScanDevices_;
    std::string touchDescriptor;

    // The array of pending epoll events and the index of the next event to be handled.
    struct epoll_event mPendingEventItems[EPOLL_MAX_EVENTS];
    std::atomic<int32_t> pendingEventCount_;
    std::atomic<int32_t> pendingEventIndex_;
    std::atomic<bool> pendingINotify_;
    std::mutex operationMutex_;

    std::atomic<bool> deviceChanged_;
    std::atomic<uint32_t> inputTypes_;
    std::atomic<bool> isStartCollectEvent_;
    std::atomic<bool> isStartCollectHandler_;
    std::unordered_map<std::string, bool> sharedDHIds_;
    std::unordered_map<std::string, int32_t> logTimesMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // INPUT_HUB_H
