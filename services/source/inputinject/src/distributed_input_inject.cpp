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

#include "distributed_input_inject.h"

#include <sstream>

#include "nlohmann/json.hpp"

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
DistributedInputInject::DistributedInputInject()
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    inputNodeManager_ = std::make_unique<DistributedInputNodeManager>();
}

DistributedInputInject::~DistributedInputInject()
{
    DHLOGI("~DistributedInputInject");
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    inputNodeManager_.reset();
    inputNodeManager_ = nullptr;
}

DistributedInputInject &DistributedInputInject::GetInstance()
{
    static DistributedInputInject instance;
    return instance;
}

int32_t DistributedInputInject::RegisterDistributedHardware(const std::string& devId, const std::string& dhId,
    const std::string& parameters)
{
    DHLOGI("RegisterDistributedHardware called, deviceId: %s,  dhId: %s,  parameters: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), SetAnonyId(parameters).c_str());
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("the DistributedInputNodeManager is null\n");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL;
    }
    if (inputNodeManager_->OpenDevicesNode(devId, dhId, parameters) < 0) {
        DHLOGE("create virtual device error\n");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_REGISTER_FAIL;
    }

    std::string srcDevId;
    inputNodeManager_->GetDeviceInfo(srcDevId);
    DHLOGI("RegisterDistributedHardware called, device type = source, source networkId = %s, sink networkId = %s",
           GetAnonyString(srcDevId).c_str(), GetAnonyString(devId).c_str());

    SyncNodeOnlineInfo(srcDevId, devId, dhId, GetNodeDesc(parameters));
    return DH_SUCCESS;
}

int32_t DistributedInputInject::UnregisterDistributedHardware(const std::string& devId, const std::string& dhId)
{
    DHLOGI("UnregisterDistributedHardware called, deviceId: %s,  dhId: %s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("the DistributedInputNodeManager is null\n");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL;
    }
    if (inputNodeManager_->CloseDeviceLocked(dhId) < 0) {
        DHLOGE("delete virtual device error\n");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_UNREGISTER_FAIL;
    }

    std::string srcDevId;
    inputNodeManager_->GetDeviceInfo(srcDevId);

    DHLOGI("UnregisterDistributedHardware called, device = %s, dhId = %s, OnNodeOffLine",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    SyncNodeOfflineInfo(srcDevId, devId, dhId);

    return DH_SUCCESS;
}

int32_t DistributedInputInject::RegisterInputNodeListener(sptr<InputNodeListener> listener)
{
    std::lock_guard<std::mutex> lock(inputNodeListenersMutex_);
    this->inputNodeListeners_.insert(listener);
    return DH_SUCCESS;
}

int32_t DistributedInputInject::UnregisterInputNodeListener(sptr<InputNodeListener> listener)
{
    std::lock_guard<std::mutex> lock(inputNodeListenersMutex_);
    this->inputNodeListeners_.erase(listener);
    return DH_SUCCESS;
}

int32_t DistributedInputInject::GetDhIdsByInputType(const std::string &devId, const uint32_t &inputTypes,
    std::vector<std::string> &dhIds)
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("the inputNodeListener is nullptr");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL;
    }
    std::map<int32_t, std::string> datas;
    inputNodeManager_->GetDevicesInfoByType(devId, inputTypes, datas);
    for (const auto &data_ : datas) {
        dhIds.push_back(data_.second);
    }
    return DH_SUCCESS;
}

int32_t DistributedInputInject::StructTransJson(const InputDevice &pBuf, std::string &strDescriptor)
{
    DHLOGI("[%s] %d, %d, %d, %d, %s.\n", (pBuf.name).c_str(), pBuf.bus, pBuf.vendor, pBuf.product, pBuf.version,
        GetAnonyString(pBuf.descriptor).c_str());
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

    std::ostringstream stream;
    stream << tmpJson.dump();
    strDescriptor = stream.str();
    return DH_SUCCESS;
}

void DistributedInputInject::InputDeviceEventInject(const std::shared_ptr<RawEvent> &rawEvent)
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("the inputNodeListener is nullptr");
        return;
    }
    if (rawEvent == nullptr) {
        DHLOGE("the rawEvent is nullptr");
        return;
    }
    inputNodeManager_->ProcessInjectEvent(rawEvent);
}

void DistributedInputInject::SyncNodeOnlineInfo(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId, const std::string &sinkNodeDesc)
{
    std::lock_guard<std::mutex> lock(inputNodeListenersMutex_);
    DHLOGI("SyncVirNodeOnlineInfo, srcId: %s, sinkId: %s, dhId: %s", GetAnonyString(srcDevId).c_str(),
        GetAnonyString(sinkDevId).c_str(), GetAnonyString(sinkNodeId).c_str());
    for (const auto &listener : inputNodeListeners_) {
        listener->OnNodeOnLine(srcDevId, sinkDevId, sinkNodeId, sinkNodeDesc);
    }
}

void DistributedInputInject::SyncNodeOfflineInfo(const std::string &srcDevId,
    const std::string &sinkDevId, const std::string &sinkNodeId)
{
    std::lock_guard<std::mutex> lock(inputNodeListenersMutex_);
    DHLOGI("SyncVirNodeOfflineInfo, srcId: %s, sinkId: %s, dhId: %s", GetAnonyString(srcDevId).c_str(),
        GetAnonyString(sinkDevId).c_str(), GetAnonyString(sinkNodeId).c_str());
    for (const auto &listener : inputNodeListeners_) {
        listener->OnNodeOffLine(srcDevId, sinkDevId, sinkNodeId);
    }
}

int32_t DistributedInputInject::RegisterDistributedEvent(RawEvent *buffer, size_t bufferSize)
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("the DistributedInputNodeManager is null");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL;
    }
    DHLOGI("RegisterDistributedEvent start %zu\n", bufferSize);
    for (size_t i = 0; i < bufferSize; i++) {
        inputNodeManager_->ReportEvent(buffer[i]);
    }
    return DH_SUCCESS;
}

void DistributedInputInject::StartInjectThread()
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ != nullptr) {
        inputNodeManager_->StartInjectThread();
    }
}

void DistributedInputInject::StopInjectThread()
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ != nullptr) {
        inputNodeManager_->StopInjectThread();
    }
}

std::string DistributedInputInject::GenerateVirtualTouchScreenDHId(const uint64_t sourceWinId,
    const uint32_t sourceWinWidth, const uint32_t sourceWinHeight)
{
    std::string uniqueInfo = GetLocalNetworkId() + std::to_string(sourceWinId) +
        std::to_string(sourceWinWidth) + std::to_string(sourceWinHeight);
    return DH_ID_PREFIX + Sha256(uniqueInfo);
}

int32_t DistributedInputInject::CreateVirtualTouchScreenNode(const std::string& devId, const std::string& dhId,
    const uint64_t srcWinId, const uint32_t sourcePhyWidth, const uint32_t sourcePhyHeight)
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("inputNodeManager is nullptr");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL;
    }
    return inputNodeManager_->CreateVirtualTouchScreenNode(devId, dhId, srcWinId, sourcePhyWidth, sourcePhyHeight);
}

int32_t DistributedInputInject::RemoveVirtualTouchScreenNode(const std::string& dhId)
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("inputNodeManager is nullptr");
        return ERR_DH_INPUT_SERVER_SOURCE_INJECT_NODE_MANAGER_IS_NULL;
    }
    return inputNodeManager_->RemoveVirtualTouchScreenNode(dhId);
}

int32_t DistributedInputInject::GetVirtualTouchScreenFd()
{
    std::lock_guard<std::mutex> lock(inputNodeManagerMutex_);
    if (inputNodeManager_ == nullptr) {
        DHLOGE("inputNodeManager is nullptr");
        return UN_INIT_FD_VALUE;
    }
    return inputNodeManager_->GetVirtualTouchScreenFd();
}

void DistributedInputInject::GetVirtualKeyboardPathsByDhIds(const std::vector<std::string> &dhIds,
    std::vector<std::string> &shareDhidsPaths, std::vector<std::string> &shareDhIds)
{
    if (inputNodeManager_ == nullptr) {
        DHLOGE("inputNodeManager is nullptr");
        return;
    }
    inputNodeManager_->GetVirtualKeyboardPathsByDhIds(dhIds, shareDhidsPaths, shareDhIds);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS