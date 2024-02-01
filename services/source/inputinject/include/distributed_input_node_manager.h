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

#ifndef DISTRIBUTED_INPUT_NODE_MANAGER_H
#define DISTRIBUTED_INPUT_NODE_MANAGER_H

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "event_handler.h"
#include "nlohmann/json.hpp"

#include "constants_dinput.h"
#include "input_hub.h"
#include "i_session_state_callback.h"
#include "virtual_device.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
constexpr uint32_t DINPUT_NODE_MANAGER_SCAN_ALL_NODE = 1;
constexpr uint32_t DINPUT_INJECT_EVENT_FAIL = 2;
const std::string INPUT_NODE_DEVID = "devId";
const std::string INPUT_NODE_DHID = "dhId";
/**
 * @brief the unique id for the input peripheral.
 * left is device networkid, right is dhId in that device.
 */
using DhUniqueID = std::pair<std::string, std::string>;
/**
 * @brief a batch events form one device
 * left: device networid where these events from
 * right: the event batch
 */
using EventBatch = std::pair<std::string, std::vector<RawEvent>>;
class DistributedInputNodeManager {
public:
    DistributedInputNodeManager();
    ~DistributedInputNodeManager();

    int32_t OpenDevicesNode(const std::string &devId, const std::string &dhId, const std::string &parameters);

    int32_t GetDevice(const std::string &devId, const std::string &dhId, VirtualDevice *&device);
    void ReportEvent(const std::string &devId, const std::vector<RawEvent> &events);
    int32_t CloseDeviceLocked(const std::string &devId, const std::string &dhId);
    void StartInjectThread();
    void StopInjectThread();
    int32_t CreateVirtualTouchScreenNode(const std::string &devId, const std::string &dhId, const uint64_t srcWinId,
        const uint32_t sourcePhyWidth, const uint32_t sourcePhyHeight);
    int32_t RemoveVirtualTouchScreenNode(const std::string &devId, const std::string &dhId);
    int32_t GetVirtualTouchScreenFd();

    void ProcessInjectEvent(const EventBatch &events);

    /**
     * @brief Get the Virtual Keyboard Paths By Dh Ids object
     *
     * @param dhKeys list for device identify({networkId, dhId})
     * @param virKeyboardPaths the matched keyboard device
     * @param virKeyboardDhIds matched keyboard device identify
     */
    void GetVirtualKeyboardPaths(const std::vector<DhUniqueID> &dhUniqueIds,
        std::vector<std::string> &virKeyboardPaths);
    void NotifyNodeMgrScanVirNode(const std::string &devId, const std::string &dhId);
    void RegisterInjectEventCb(sptr<ISessionStateCallback> callback);
    void UnregisterInjectEventCb();

    class DInputNodeManagerEventHandler : public AppExecFwk::EventHandler {
    public:
        DInputNodeManagerEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
            DistributedInputNodeManager *manager);
        ~DInputNodeManagerEventHandler() override;

        void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    private:
        void ScanAllNode(const AppExecFwk::InnerEvent::Pointer &event);

        using nodeMgrFunc = void (DInputNodeManagerEventHandler::*)(
            const AppExecFwk::InnerEvent::Pointer &event);
        std::map<int32_t, nodeMgrFunc> eventFuncMap_;
        DistributedInputNodeManager *nodeManagerObj_;
    };

private:
    void AddDeviceLocked(const std::string &networkId, const std::string &dhId, std::unique_ptr<VirtualDevice> device);
    int32_t CreateHandle(const InputDevice &inputDevice, const std::string &devId, const std::string &dhId);
    void ParseInputDeviceJson(const std::string &str, InputDevice &pBuf);
    void ParseInputDevice(const nlohmann::json &inputDeviceJson, InputDevice &pBuf);
    void ParseInputDeviceBasicInfo(const nlohmann::json &inputDeviceJson, InputDevice &pBuf);
    void ParseInputDeviceEvents(const nlohmann::json &inputDeviceJson, InputDevice &pBuf);
    void InjectEvent();

    void ScanSinkInputDevices(const std::string &devId, const std::string &dhId);
    bool MatchAndSavePhysicalPath(const std::string &devicePath, const std::string &devId, const std::string &dhId);
    bool IsVirtualDev(int fd);
    bool GetDevDhUniqueIdByFd(int fd, DhUniqueID &dhUnqueId, std::string &physicalPath);
    void SetPathForVirDev(const DhUniqueID &dhUniqueId, const std::string &devicePath);
    void RunInjectEventCallback(const std::string &dhId, const uint32_t injectEvent);

    /* the key is {networkId, dhId}, and the value is virtualDevice */
    std::map<DhUniqueID, std::unique_ptr<VirtualDevice>> virtualDeviceMap_;
    std::mutex virtualDeviceMapMutex_;
    std::atomic<bool> isInjectThreadCreated_;
    std::atomic<bool> isInjectThreadRunning_;
    std::mutex operationMutex_;
    std::thread eventInjectThread_;
    std::mutex injectThreadMutex_;
    std::condition_variable conditionVariable_;
    std::queue<EventBatch> injectQueue_;
    int32_t virtualTouchScreenFd_;
    std::once_flag callOnceFlag_;
    std::shared_ptr<DInputNodeManagerEventHandler> callBackHandler_;
    sptr<ISessionStateCallback> SessionStateCallback_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_INJECT_H