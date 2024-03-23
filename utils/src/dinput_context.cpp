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

#include "dinput_context.h"

#include "constants.h"

#include "dinput_errcode.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DInputContext);

DInputContext::~DInputContext()
{
    sinkScreenInfoMap_.clear();
    srcScreenInfoMap_.clear();
}

std::string DInputContext::GetScreenInfoKey(const std::string &devId, const uint64_t sourceWinId)
{
    DHLOGI("GetScreenInfoKey screenInfoKey: %{public}s, sourceWinId: %{public}" PRIu64 "",
        GetAnonyString(devId).c_str(), sourceWinId);
    return devId + RESOURCE_SEPARATOR + std::to_string(sourceWinId);
}

int32_t DInputContext::RemoveSinkScreenInfo(const std::string &screenInfoKey)
{
    DHLOGI("RemoveSinkScreenInfo screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    std::lock_guard<std::mutex> lock(sinkMapMutex_);
    sinkScreenInfoMap_.erase(screenInfoKey);
    return DH_SUCCESS;
}

int32_t DInputContext::UpdateSinkScreenInfo(const std::string &screenInfoKey, const SinkScreenInfo &sinkScreenInfo)
{
    DHLOGI("UpdateSinkScreenInfo screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    std::lock_guard<std::mutex> lock(sinkMapMutex_);
    if (sinkScreenInfoMap_.count(screenInfoKey) <= 0) {
        DHLOGE("source window id not exist");
        return ERR_DH_INPUT_CONTEXT_KEY_NOT_EXIST;
    }

    SinkScreenInfo tmp = sinkScreenInfo;
    if (CalculateTransformInfo(tmp) != DH_SUCCESS) {
        DHLOGE("calculate transform infomation failed");
    }

    sinkScreenInfoMap_[screenInfoKey] = tmp;
    return DH_SUCCESS;
}

SinkScreenInfo DInputContext::GetSinkScreenInfo(const std::string &screenInfoKey)
{
    DHLOGI("GetSinkScreenInfo screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    std::lock_guard<std::mutex> lock(sinkMapMutex_);
    if (sinkScreenInfoMap_.count(screenInfoKey) <= 0) {
        DHLOGE("screenInfoKey not exist");
        SinkScreenInfo sinkScreenInfo;
        sinkScreenInfoMap_[screenInfoKey] = sinkScreenInfo;
    }

    return sinkScreenInfoMap_[screenInfoKey];
}

const std::unordered_map<std::string, SinkScreenInfo> &DInputContext::GetAllSinkScreenInfo()
{
    std::lock_guard<std::mutex> lock(sinkMapMutex_);
    return sinkScreenInfoMap_;
}

int32_t DInputContext::RemoveSrcScreenInfo(const std::string &screenInfoKey)
{
    DHLOGI("RemoveSrcScreenInfo screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    std::lock_guard<std::mutex> lock(srcMapMutex_);
    srcScreenInfoMap_.erase(screenInfoKey);
    return DH_SUCCESS;
}

int32_t DInputContext::UpdateSrcScreenInfo(const std::string &screenInfoKey, const SrcScreenInfo &srcScreenInfo)
{
    std::lock_guard<std::mutex> lock(srcMapMutex_);
    DHLOGI("UpdateSrcScreenInfo screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    if (srcScreenInfoMap_.count(screenInfoKey) <= 0) {
        DHLOGE("source window id not exist");
        return ERR_DH_INPUT_CONTEXT_KEY_NOT_EXIST;
    }

    srcScreenInfoMap_[screenInfoKey] = srcScreenInfo;
    return DH_SUCCESS;
}

SrcScreenInfo DInputContext::GetSrcScreenInfo(const std::string &screenInfoKey)
{
    DHLOGI("GetSrcScreenInfo screenInfoKey: %{public}s", GetAnonyString(screenInfoKey).c_str());
    std::lock_guard<std::mutex> lock(srcMapMutex_);
    if (srcScreenInfoMap_.count(screenInfoKey) <= 0) {
        DHLOGE("source window id not exist");
        SrcScreenInfo srcScreenInfo;
        srcScreenInfoMap_[screenInfoKey] = srcScreenInfo;
    }

    return srcScreenInfoMap_[screenInfoKey];
}

void DInputContext::SetLocalTouchScreenInfo(const LocalTouchScreenInfo &localTouchScreenInfo)
{
    std::lock_guard<std::mutex> lock(localTouchScreenInfoMutex_);
    localTouchScreenInfo_ = localTouchScreenInfo;
}

LocalTouchScreenInfo DInputContext::GetLocalTouchScreenInfo()
{
    std::lock_guard<std::mutex> lock(localTouchScreenInfoMutex_);
    return localTouchScreenInfo_;
}

int32_t DInputContext::CalculateTransformInfo(SinkScreenInfo &sinkScreenInfo)
{
    if (sinkScreenInfo.sinkShowHeight == 0 || sinkScreenInfo.sinkShowWidth == 0) {
        DHLOGE("can not calculate transform infomation");
        return ERR_DH_INPUT_CONTEXT_CALCULATE_FAIL;
    }
    TransformInfo transformInfo;
    transformInfo.sinkWinPhyX = static_cast<uint32_t>(sinkScreenInfo.sinkWinShowX /
        static_cast<double>(sinkScreenInfo.sinkShowWidth)) * sinkScreenInfo.sinkPhyWidth;
    transformInfo.sinkWinPhyY = static_cast<uint32_t>(sinkScreenInfo.sinkWinShowY /
        static_cast<double>(sinkScreenInfo.sinkShowHeight)) * sinkScreenInfo.sinkPhyHeight;
    transformInfo.sinkProjPhyWidth = static_cast<uint32_t>((sinkScreenInfo.sinkProjShowWidth /
        static_cast<double>(sinkScreenInfo.sinkShowWidth)) * sinkScreenInfo.sinkPhyWidth);
    transformInfo.sinkProjPhyHeight = static_cast<uint32_t>((sinkScreenInfo.sinkProjShowHeight /
        static_cast<double>(sinkScreenInfo.sinkShowHeight)) * sinkScreenInfo.sinkPhyHeight);
    if (transformInfo.sinkProjPhyWidth == 0 || transformInfo.sinkProjPhyHeight == 0) {
        DHLOGE("can not calculate transform infomation");
        return ERR_DH_INPUT_CONTEXT_CALCULATE_FAIL;
    }

    // coefficient of the sink projection area in the source touch driver
    transformInfo.coeffWidth = static_cast<double>(sinkScreenInfo.srcScreenInfo.sourcePhyWidth /
        static_cast<double>(transformInfo.sinkProjPhyWidth));
    transformInfo.coeffHeight = static_cast<double>(sinkScreenInfo.srcScreenInfo.sourcePhyHeight /
        static_cast<double>(transformInfo.sinkProjPhyHeight));

    DHLOGI("CalculateTransformInfo sinkWinPhyX = %{public}d, sinkWinPhyY = %{public}d, sinkProjPhyWidth = %{public}d, "
        "sinkProjPhyHeight = %{public}d, coeffWidth = %{public}f, coeffHeight = %{public}f", transformInfo.sinkWinPhyX,
        transformInfo.sinkWinPhyY, transformInfo.sinkProjPhyWidth, transformInfo.sinkProjPhyHeight,
        transformInfo.coeffWidth, transformInfo.coeffHeight);
    sinkScreenInfo.transformInfo = transformInfo;
    return DH_SUCCESS;
}

std::shared_ptr<DistributedHardwareFwkKit> DInputContext::GetDHFwkKit()
{
    std::lock_guard<std::mutex> lock(dhFwkKitMutex_);
    if (dhFwkKit_ == nullptr) {
        dhFwkKit_ = std::make_shared<DistributedHardwareFwkKit>();
    }
    return dhFwkKit_;
}

sptr<IRemoteObject> DInputContext::GetRemoteObject(const int32_t saId)
{
    DHLOGI("GetDScreenSrcSA start");
    {
        std::lock_guard<std::mutex> lock(remoteObjectsMutex_);
        if (remoteObjects_.find(saId) != remoteObjects_.end()) {
            DHLOGI("dScreenSrcSA get from cache!");
            return remoteObjects_[saId];
        }
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("GetSystemAbilityManager fail!");
        return nullptr;
    }
    auto remoteObject = samgr->GetSystemAbility(saId);
    if (remoteObject == nullptr) {
        DHLOGE("GetSystemAbility remoteObject is nullptr");
        return nullptr;
    }
    return remoteObject;
}

void DInputContext::AddRemoteObject(const int32_t saId, const sptr<IRemoteObject> &remoteObject)
{
    std::lock_guard<std::mutex> lock(remoteObjectsMutex_);
    remoteObjects_[saId] = remoteObject;
}

void DInputContext::RemoveRemoteObject(const int32_t saId)
{
    std::lock_guard<std::mutex> lock(remoteObjectsMutex_);
    remoteObjects_.erase(saId);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS