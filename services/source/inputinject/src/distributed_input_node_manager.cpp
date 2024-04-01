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

#include "distributed_input_node_manager.h"

#include <cinttypes>
#include <cstring>

#include <pthread.h>

#include "softbus_bus_center.h"

#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_softbus_define.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    constexpr int32_t RETRY_MAX_TIMES = 3;
    constexpr uint32_t SLEEP_TIME_US = 10 * 1000;
}
DistributedInputNodeManager::DistributedInputNodeManager() : isInjectThreadCreated_(false),
    isInjectThreadRunning_(false), virtualTouchScreenFd_(UN_INIT_FD_VALUE)
{
    DHLOGI("DistributedInputNodeManager ctor");
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(true);
    callBackHandler_ = std::make_shared<DistributedInputNodeManager::DInputNodeManagerEventHandler>(runner, this);
}

DistributedInputNodeManager::~DistributedInputNodeManager()
{
    DHLOGI("DistributedInputNodeManager dtor");
    isInjectThreadCreated_.store(false);
    isInjectThreadRunning_.store(false);
    if (eventInjectThread_.joinable()) {
        eventInjectThread_.join();
    }
    {
        std::lock_guard<std::mutex> lock(virtualDeviceMapMutex_);
        virtualDeviceMap_.clear();
    }
    DHLOGI("destructor end");
}

int32_t DistributedInputNodeManager::OpenDevicesNode(const std::string &devId, const std::string &dhId,
    const std::string &parameters)
{
    if (devId.size() > DEV_ID_LENGTH_MAX || devId.empty() || dhId.size() > DH_ID_LENGTH_MAX || dhId.empty() ||
        parameters.size() > STRING_MAX_SIZE || parameters.empty()) {
        DHLOGE("Params is invalid!");
        return ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL;
    }
    InputDevice event;
    ParseInputDeviceJson(parameters, event);
    if (CreateHandle(event, devId, dhId) < 0) {
        DHLOGE("Can not create virtual node!");
        return ERR_DH_INPUT_SERVER_SOURCE_OPEN_DEVICE_NODE_FAIL;
    }
    return DH_SUCCESS;
}

void DistributedInputNodeManager::ParseInputDeviceJson(const std::string &str, InputDevice &pBuf)
{
    nlohmann::json inputDeviceJson = nlohmann::json::parse(str, nullptr, false);
    if (inputDeviceJson.is_discarded()) {
        DHLOGE("recMsg parse failed!");
        return;
    }
    ParseInputDevice(inputDeviceJson, pBuf);
}

void DistributedInputNodeManager::ParseInputDevice(const nlohmann::json &inputDeviceJson, InputDevice &pBuf)
{
    ParseInputDeviceBasicInfo(inputDeviceJson, pBuf);
    ParseInputDeviceEvents(inputDeviceJson, pBuf);
}

void DistributedInputNodeManager::ParseInputDeviceBasicInfo(const nlohmann::json &inputDeviceJson, InputDevice &pBuf)
{
    if (IsString(inputDeviceJson, DEVICE_NAME)) {
        pBuf.name = inputDeviceJson[DEVICE_NAME].get<std::string>();
    }
    if (IsString(inputDeviceJson, PHYSICAL_PATH)) {
        pBuf.physicalPath = inputDeviceJson[PHYSICAL_PATH].get<std::string>();
    }
    if (IsString(inputDeviceJson, UNIQUE_ID)) {
        pBuf.uniqueId = inputDeviceJson[UNIQUE_ID].get<std::string>();
    }
    if (IsUInt16(inputDeviceJson, BUS)) {
        pBuf.bus = inputDeviceJson[BUS].get<uint16_t>();
    }
    if (IsUInt16(inputDeviceJson, VENDOR)) {
        pBuf.vendor = inputDeviceJson[VENDOR].get<uint16_t>();
    }
    if (IsUInt16(inputDeviceJson, PRODUCT)) {
        pBuf.product = inputDeviceJson[PRODUCT].get<uint16_t>();
    }
    if (IsUInt16(inputDeviceJson, VERSION)) {
        pBuf.version = inputDeviceJson[VERSION].get<uint16_t>();
    }
    if (IsString(inputDeviceJson, DESCRIPTOR)) {
        pBuf.descriptor = inputDeviceJson[DESCRIPTOR].get<std::string>();
    }
    if (IsUInt32(inputDeviceJson, CLASSES)) {
        pBuf.classes = inputDeviceJson[CLASSES].get<uint32_t>();
    }
}

void DistributedInputNodeManager::ParseInputDeviceEvents(const nlohmann::json &inputDeviceJson, InputDevice &pBuf)
{
    if (IsArray(inputDeviceJson, EVENT_TYPES)) {
        pBuf.eventTypes = inputDeviceJson[EVENT_TYPES].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, EVENT_KEYS)) {
        pBuf.eventKeys = inputDeviceJson[EVENT_KEYS].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, ABS_TYPES)) {
        pBuf.absTypes = inputDeviceJson[ABS_TYPES].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, ABS_INFOS)) {
        pBuf.absInfos = inputDeviceJson[ABS_INFOS].get<std::map<uint32_t, std::vector<int32_t>>>();
    }
    if (IsArray(inputDeviceJson, REL_TYPES)) {
        pBuf.relTypes = inputDeviceJson[REL_TYPES].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, PROPERTIES)) {
        pBuf.properties = inputDeviceJson[PROPERTIES].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, MISCELLANEOUS)) {
        pBuf.miscellaneous = inputDeviceJson[MISCELLANEOUS].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, LEDS)) {
        pBuf.leds = inputDeviceJson[LEDS].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, REPEATS)) {
        pBuf.repeats = inputDeviceJson[REPEATS].get<std::vector<uint32_t>>();
    }
    if (IsArray(inputDeviceJson, SWITCHS)) {
        pBuf.switchs = inputDeviceJson[SWITCHS].get<std::vector<uint32_t>>();
    }
}

void DistributedInputNodeManager::ScanSinkInputDevices(const std::string &devId, const std::string &dhId)
{
    DHLOGI("ScanSinkInputDevices enter, deviceId: %{public}s, dhId: %{public}s.",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    std::vector<std::string> vecInputDevPath;
    ScanInputDevicesPath(DEVICE_PATH, vecInputDevPath);
    for (auto &tempPath: vecInputDevPath) {
        if (MatchAndSavePhysicalPath(tempPath, devId, dhId)) {
            DHLOGI("Set physical path success");
            break;
        }
    }
}

void DistributedInputNodeManager::DInputNodeManagerEventHandler::ProcessEvent(
    const AppExecFwk::InnerEvent::Pointer &event)
{
    DHLOGI("ProcessEvent enter.");
    auto iter = eventFuncMap_.find(event->GetInnerEventId());
    if (iter == eventFuncMap_.end()) {
        DHLOGE("Event Id %{public}d is undefined.", event->GetInnerEventId());
        return;
    }
    nodeMgrFunc &func = iter->second;
    (this->*func)(event);
}

DistributedInputNodeManager::DInputNodeManagerEventHandler::DInputNodeManagerEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, DistributedInputNodeManager *manager)
    : AppExecFwk::EventHandler(runner)
{
    eventFuncMap_[DINPUT_NODE_MANAGER_SCAN_ALL_NODE] = &DInputNodeManagerEventHandler::ScanAllNode;

    nodeManagerObj_ = manager;
}

DistributedInputNodeManager::DInputNodeManagerEventHandler::~DInputNodeManagerEventHandler()
{
    eventFuncMap_.clear();
    nodeManagerObj_ = nullptr;
}

void DistributedInputNodeManager::DInputNodeManagerEventHandler::ScanAllNode(
    const AppExecFwk::InnerEvent::Pointer &event)
{
    DHLOGI("ScanAllNode enter.");
    std::shared_ptr<nlohmann::json> dataMsg = event->GetSharedObject<nlohmann::json>();
    auto it = dataMsg->begin();
    nlohmann::json innerMsg = *(it);
    std::string devId = innerMsg[INPUT_NODE_DEVID];
    std::string devicedhId = innerMsg[INPUT_NODE_DHID];
    nodeManagerObj_->ScanSinkInputDevices(devId, devicedhId);
}

void DistributedInputNodeManager::NotifyNodeMgrScanVirNode(const std::string &devId, const std::string &dhId)
{
    DHLOGI("NotifyNodeMgrScanVirNode enter.");
    std::shared_ptr<nlohmann::json> jsonArrayMsg = std::make_shared<nlohmann::json>();
    nlohmann::json tmpJson;
    tmpJson[INPUT_NODE_DEVID] = devId;
    tmpJson[INPUT_NODE_DHID] = dhId;
    jsonArrayMsg->push_back(tmpJson);
    AppExecFwk::InnerEvent::Pointer msgEvent = AppExecFwk::InnerEvent::Get(
        DINPUT_NODE_MANAGER_SCAN_ALL_NODE, jsonArrayMsg, 0);
    callBackHandler_->SendEvent(msgEvent, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

bool DistributedInputNodeManager::IsVirtualDev(int fd)
{
    char buffer[INPUT_EVENT_BUFFER_SIZE] = {0};
    std::string deviceName;
    if (ioctl(fd, EVIOCGNAME(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("Could not get device name for %{public}s.", ConvertErrNo().c_str());
        return false;
    }
    buffer[sizeof(buffer) - 1] = '\0';
    deviceName = buffer;

    DHLOGD("IsVirtualDev deviceName: %{public}s", buffer);
    if (deviceName.find(VIRTUAL_DEVICE_NAME) == std::string::npos) {
        DHLOGD("This is not a virtual device, fd %{public}d, deviceName: %{public}s.", fd, deviceName.c_str());
        return false;
    }
    return true;
}

bool DistributedInputNodeManager::GetDevDhUniqueIdByFd(int fd, DhUniqueID &dhUnqueId, std::string &physicalPath)
{
    char buffer[INPUT_EVENT_BUFFER_SIZE] = {0};
    if (ioctl(fd, EVIOCGPHYS(sizeof(buffer) - 1), &buffer) < 1) {
        DHLOGE("Could not get device physicalPath for %{public}s.", ConvertErrNo().c_str());
        return false;
    }
    buffer[sizeof(buffer) - 1] = '\0';
    physicalPath = buffer;

    DHLOGD("GetDevDhUniqueIdByFd physicalPath %{public}s.", physicalPath.c_str());
    std::vector<std::string> phyPathVec;
    SplitStringToVector(physicalPath, VIR_NODE_SPLIT_CHAR, phyPathVec);
    if (phyPathVec.size() != VIR_NODE_PHY_LEN) {
        DHLOGE("The physical path is invalid");
        return false;
    }
    std::string devId = phyPathVec[VIR_NODE_PHY_DEVID_IDX];
    std::string dhId = phyPathVec[VIR_NODE_PHY_DHID_IDX];
    if (devId.empty() || dhId.empty()) {
        DHLOGE("Get dev deviceid and dhid failed.");
        return false;
    }
    dhUnqueId.first = devId;
    dhUnqueId.second = dhId;
    DHLOGD("Device deviceid: %{public}s, dhId %{public}s.", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    return true;
}

void DistributedInputNodeManager::SetPathForVirDev(const DhUniqueID &dhUniqueId, const std::string &devicePath)
{
    std::lock_guard<std::mutex> lock(virtualDeviceMapMutex_);
    auto iter = virtualDeviceMap_.begin();
    while (iter != virtualDeviceMap_.end()) {
        DHLOGD("Check Virtual device, deviceId %{public}s, dhid %{public}s.", GetAnonyString(iter->first.first).c_str(),
            GetAnonyString(iter->first.second).c_str());
        if (iter->first == dhUniqueId) {
            DHLOGD("Found the virtual device, set path :%{public}s", devicePath.c_str());
            iter->second->SetPath(devicePath);
            break;
        }
        iter++;
    }
}

bool DistributedInputNodeManager::MatchAndSavePhysicalPath(const std::string &devicePath,
    const std::string &devId, const std::string &dhId)
{
    DHLOGI("Opening input device path: %{public}s", devicePath.c_str());
    DhUniqueID curDhUniqueId;
    std::string physicalPath;
    int fd = OpenInputDeviceFdByPath(devicePath);
    if (fd == UN_INIT_FD_VALUE) {
        DHLOGE("The fd open failed, devicePath %{public}s.", devicePath.c_str());
        return false;
    }
    if (!IsVirtualDev(fd)) {
        DHLOGE("The dev not virtual, devicePath %{public}s.", devicePath.c_str());
        CloseFd(fd);
        return false;
    }
    if (!GetDevDhUniqueIdByFd(fd, curDhUniqueId, physicalPath)) {
        DHLOGE("Get unique id failed, device path %{public}s.", devicePath.c_str());
        CloseFd(fd);
        return false;
    }

    DHLOGD("This opening node deviceId: %{public}s, dhId: %{public}s, to match node deviceId: %{public}s, "
        "dhId: %{public}s", GetAnonyString(curDhUniqueId.first).c_str(), GetAnonyString(curDhUniqueId.second).c_str(),
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());

    if (curDhUniqueId.first != devId || curDhUniqueId.second != dhId) {
        DHLOGW("It is not the target vir node, skip it.");
        CloseFd(fd);
        return false;
    }

    SetPathForVirDev(curDhUniqueId, devicePath);
    CloseFd(fd);
    return true;
}

void DistributedInputNodeManager::GetVirtualKeyboardPaths(const std::vector<DhUniqueID> &dhUniqueIds,
    std::vector<std::string> &virKeyboardPaths)
{
    std::lock_guard<std::mutex> lock(virtualDeviceMapMutex_);
    for (const auto &dhUniqueId : dhUniqueIds) {
        auto iter = virtualDeviceMap_.begin();
        while (iter != virtualDeviceMap_.end()) {
            if (iter->second == nullptr) {
                DHLOGE("device is nullptr");
                continue;
            }
            if ((iter->first == dhUniqueId) &&
                ((iter->second->GetClasses() & INPUT_DEVICE_CLASS_KEYBOARD) != 0)) {
                DHLOGI("Found vir keyboard path %{public}s, deviceId %{public}s, dhid %{public}s",
                    iter->second->GetPath().c_str(), GetAnonyString(dhUniqueId.first).c_str(),
                    GetAnonyString(dhUniqueId.second).c_str());
                virKeyboardPaths.push_back(iter->second->GetPath());
            }
            iter++;
        }
    }
}

int32_t DistributedInputNodeManager::CreateHandle(const InputDevice &inputDevice, const std::string &devId,
    const std::string &dhId)
{
    std::unique_lock<std::mutex> my_lock(operationMutex_);
    std::unique_ptr<VirtualDevice> virtualDevice = std::make_unique<VirtualDevice>(inputDevice);

    virtualDevice->SetNetWorkId(devId);

    if (!virtualDevice->SetUp(inputDevice, devId, dhId)) {
        DHLOGE("could not create new virtual device\n");
        for (int32_t i = 0; i < RETRY_MAX_TIMES; ++i) {
            if (virtualDevice->SetUp(inputDevice, devId, dhId)) {
                DHLOGI("Create new virtual success");
                AddDeviceLocked(devId, inputDevice.descriptor, std::move(virtualDevice));
                return DH_SUCCESS;
            }
            usleep(SLEEP_TIME_US);
        }
        return ERR_DH_INPUT_SERVER_SOURCE_CREATE_HANDLE_FAIL;
    }
    AddDeviceLocked(devId, inputDevice.descriptor, std::move(virtualDevice));
    return DH_SUCCESS;
}

int32_t DistributedInputNodeManager::CreateVirtualTouchScreenNode(const std::string &devId, const std::string &dhId,
    const uint64_t srcWinId, const uint32_t sourcePhyWidth, const uint32_t sourcePhyHeight)
{
    std::unique_lock<std::mutex> my_lock(operationMutex_);
    std::unique_ptr<VirtualDevice> device;
    LocalAbsInfo info = DInputContext::GetInstance().GetLocalTouchScreenInfo().localAbsInfo;
    DHLOGI("CreateVirtualTouchScreenNode, dhId: %{public}s, sourcePhyWidth: %{public}d, sourcePhyHeight: %{public}d",
        GetAnonyString(dhId).c_str(), sourcePhyWidth, sourcePhyHeight);
    device = std::make_unique<VirtualDevice>(info.deviceInfo);
    if (!device->SetUp(info.deviceInfo, devId, dhId)) {
        DHLOGE("Virtual touch Screen setUp fail, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId).c_str(),
            GetAnonyString(dhId).c_str());
        return ERR_DH_INPUT_SERVER_SOURCE_CREATE_HANDLE_FAIL;
    }
    virtualTouchScreenFd_ = device->GetDeviceFd();
    AddDeviceLocked(devId, dhId, std::move(device));
    DHLOGI("CreateVirtualTouchScreenNode end, dhId: %{public}s", GetAnonyString(dhId).c_str());
    return DH_SUCCESS;
}

int32_t DistributedInputNodeManager::RemoveVirtualTouchScreenNode(const std::string &devId, const std::string &dhId)
{
    return CloseDeviceLocked(devId, dhId);
}

int32_t DistributedInputNodeManager::GetVirtualTouchScreenFd()
{
    return virtualTouchScreenFd_;
}

void DistributedInputNodeManager::AddDeviceLocked(const std::string &networkId, const std::string &dhId,
    std::unique_ptr<VirtualDevice> device)
{
    DHLOGI("AddDeviceLocked deviceId=%{public}s, dhId=%{public}s",
        GetAnonyString(networkId).c_str(), GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(virtualDeviceMapMutex_);
    auto [dev_it, inserted] = virtualDeviceMap_.insert_or_assign(
        {networkId, dhId}, std::move(device));
    if (!inserted) {
        DHLOGI("Device exists, deviceId=%{public}s, dhId=%{public}s, replaced. \n",
            GetAnonyString(networkId).c_str(), GetAnonyString(dhId).c_str());
    }
}

int32_t DistributedInputNodeManager::CloseDeviceLocked(const std::string &devId, const std::string &dhId)
{
    DHLOGI("CloseDeviceLocked called, deviceId=%{public}s, dhId=%{public}s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(virtualDeviceMapMutex_);
    DhUniqueID dhUniqueId = {devId, dhId};
    std::map<DhUniqueID, std::unique_ptr<VirtualDevice>>::iterator iter = virtualDeviceMap_.find(dhUniqueId);
    if (iter != virtualDeviceMap_.end()) {
        DHLOGI("CloseDeviceLocked called success, deviceId=%{public}s, dhId=%{public}s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        virtualDeviceMap_.erase(iter);
        return DH_SUCCESS;
    }
    DHLOGE("CloseDeviceLocked called failure, deviceId=%{public}s, dhId=%{public}s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    return ERR_DH_INPUT_SERVER_SOURCE_CLOSE_DEVICE_FAIL;
}

int32_t DistributedInputNodeManager::GetDevice(const std::string &devId, const std::string &dhId,
    VirtualDevice *&device)
{
    std::lock_guard<std::mutex> lock(virtualDeviceMapMutex_);
    auto iter = virtualDeviceMap_.find({devId, dhId});
    if (iter != virtualDeviceMap_.end()) {
        device = iter->second.get();
        return DH_SUCCESS;
    }
    return ERR_DH_INPUT_SERVER_SOURCE_GET_DEVICE_FAIL;
}

void DistributedInputNodeManager::StartInjectThread()
{
    if (isInjectThreadCreated_.load()) {
        DHLOGI("InjectThread has been created.");
        return;
    }
    DHLOGI("InjectThread does not created");
    isInjectThreadCreated_.store(true);
    isInjectThreadRunning_.store(true);
    eventInjectThread_ = std::thread(&DistributedInputNodeManager::InjectEvent, this);
}

void DistributedInputNodeManager::StopInjectThread()
{
    if (!isInjectThreadCreated_.load()) {
        DHLOGI("InjectThread does not created, and not need to stop.");
    }
    DHLOGI("InjectThread has been created, and soon will be stopped.");
    isInjectThreadRunning_.store(false);
    isInjectThreadCreated_.store(false);
    conditionVariable_.notify_all();
    if (eventInjectThread_.joinable()) {
        eventInjectThread_.join();
    }
}

void DistributedInputNodeManager::ReportEvent(const std::string &devId, const std::vector<RawEvent> &events)
{
    std::lock_guard<std::mutex> lockGuard(injectThreadMutex_);
    injectQueue_.push({devId, events});
    conditionVariable_.notify_all();
}

void DistributedInputNodeManager::InjectEvent()
{
    int32_t ret = pthread_setname_np(pthread_self(), EVENT_INJECT_THREAD_NAME);
    if (ret != 0) {
        DHLOGE("InjectEvent setname failed.");
    }
    DHLOGD("start");
    while (isInjectThreadRunning_.load()) {
        EventBatch events;
        {
            std::unique_lock<std::mutex> waitEventLock(injectThreadMutex_);
            conditionVariable_.wait(waitEventLock,
                [this]() { return !isInjectThreadRunning_.load() || !injectQueue_.empty(); });
            if (injectQueue_.empty()) {
                continue;
            }
            events = injectQueue_.front();
            injectQueue_.pop();
        }

        ProcessInjectEvent(events);
    }
}

void DistributedInputNodeManager::RegisterInjectEventCb(sptr<ISessionStateCallback> callback)
{
    DHLOGI("RegisterInjectEventCb");
    SessionStateCallback_ = callback;
}

void DistributedInputNodeManager::UnregisterInjectEventCb()
{
    DHLOGI("UnregisterInjectEventCb");
    SessionStateCallback_ = nullptr;
}

void DistributedInputNodeManager::RunInjectEventCallback(const std::string &dhId, const uint32_t injectEvent)
{
    DHLOGI("RunInjectEventCallback start.");
    if (SessionStateCallback_ == nullptr) {
        DHLOGE("RunSessionStateCallback SessionStateCallback_ is null.");
        return;
    }
    SessionStateCallback_->OnResult(dhId, DINPUT_INJECT_EVENT_FAIL);
}

void DistributedInputNodeManager::ProcessInjectEvent(const EventBatch &events)
{
    std::string deviceId = events.first;
    for (const auto &rawEvent : events.second) {
        std::string dhId = rawEvent.descriptor;
        struct input_event event = {
            .type = rawEvent.type,
            .code = rawEvent.code,
            .value = rawEvent.value
        };
        DHLOGI("InjectEvent deviceId: %{public}s, dhId: %{public}s, eventType: %{public}d, eventCode: %{public}d, "
            "eventValue: %{public}d, when: %{public}" PRId64"", GetAnonyString(deviceId).c_str(),
            GetAnonyString(dhId).c_str(), event.type, event.code, event.value, rawEvent.when);
        VirtualDevice* device = nullptr;
        if (GetDevice(deviceId, dhId, device) < 0) {
            DHLOGE("could not find the device");
            return;
        }
        if (device != nullptr) {
            device->InjectInputEvent(event);
        }
    }
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
