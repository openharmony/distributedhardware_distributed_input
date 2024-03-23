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

#include "distributed_input_client.h"

#include "nlohmann/json.hpp"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "white_list_util.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
std::shared_ptr<DistributedInputClient> DistributedInputClient::instance = std::make_shared<DistributedInputClient>();

DistributedInputClient::DistributedInputClient() {}

DistributedInputClient &DistributedInputClient::GetInstance()
{
    return *instance.get();
}

void DistributedInputClient::RegisterDInputCb::OnResult(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    auto iter = DistributedInputClient::GetInstance().dHardWareFwkRstInfos.begin();
    for (; iter != DistributedInputClient::GetInstance().dHardWareFwkRstInfos.end(); ++iter) {
        if (iter->devId == devId && iter->dhId == dhId) {
            iter->callback->OnRegisterResult(devId, dhId, status, "");
            DistributedInputClient::GetInstance().dHardWareFwkRstInfos.erase(iter);
            return;
        }
    }
}

void DistributedInputClient::UnregisterDInputCb::OnResult(
    const std::string &devId, const std::string &dhId, const int32_t &status)
{
    auto iter = DistributedInputClient::GetInstance().dHardWareFwkUnRstInfos.begin();
    for (; iter != DistributedInputClient::GetInstance().dHardWareFwkUnRstInfos.end(); ++iter) {
        if (iter->devId == devId && iter->dhId == dhId) {
            iter->callback->OnUnregisterResult(devId, dhId, status, "");
            DistributedInputClient::GetInstance().dHardWareFwkUnRstInfos.erase(iter);
            return;
        }
    }
}

void DistributedInputClient::AddWhiteListInfosCb::OnResult(const std::string &deviceId, const std::string &strJson)
{
    nlohmann::json inputData = nlohmann::json::parse(strJson, nullptr, false);
    if (inputData.is_discarded()) {
        DHLOGE("InputData parse failed!");
        return;
    }
    if (!inputData.is_array()) {
        DHLOGE("inputData not vector!");
        return;
    }
    size_t jsonSize = inputData.size();
    DHLOGI("AddWhiteListInfosCb OnResult json size:%{public}zu.\n", jsonSize);
    TYPE_WHITE_LIST_VEC vecWhiteList = inputData;
    WhiteListUtil::GetInstance().SyncWhiteList(deviceId, vecWhiteList);
}

void DistributedInputClient::DelWhiteListInfosCb::OnResult(const std::string &deviceId)
{
    WhiteListUtil::GetInstance().ClearWhiteList(deviceId);
}

int32_t DistributedInputClient::InitSource()
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::InitSink()
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::ReleaseSource()
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::ReleaseSink()
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::string &parameters, const std::shared_ptr<RegisterCallback> &callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::shared_ptr<UnregisterCallback> &callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::PrepareRemoteInput(
    const std::string &deviceId, sptr<IPrepareDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::PrepareRemoteInput(
    const std::string &srcId, const std::string &sinkId, sptr<IPrepareDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::UnprepareRemoteInput(
    const std::string &deviceId, sptr<IUnprepareDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::UnprepareRemoteInput(
    const std::string &srcId, const std::string &sinkId, sptr<IUnprepareDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StartRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StartRemoteInput(
    const std::string &sinkId, const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StopRemoteInput(
    const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StopRemoteInput(
    const std::string &sinkId, const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStartDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const uint32_t &inputTypes, sptr<IStopDInputCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StartRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::StopRemoteInput(const std::string &srcId, const std::string &sinkId,
    const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback)
{
    return DH_SUCCESS;
}

bool DistributedInputClient::IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
{
    return true;
}

bool DistributedInputClient::IsTouchEventNeedFilterOut(const TouchScreenEvent &event)
{
    auto sinkInfos = DInputContext::GetInstance().GetAllSinkScreenInfo();

    for (const auto &[id, sinkInfo] : sinkInfos) {
        auto info = sinkInfo.transformInfo;
        DHLOGI("event.absX:%{public}d, info.sinkWinPhyX:%{public}d, info.sinkProjPhyWidth:%{public}d\n", event.absX,
            info.sinkWinPhyX, info.sinkProjPhyWidth);
        if ((event.absX >= info.sinkWinPhyX) && (event.absX <= (info.sinkWinPhyX + info.sinkProjPhyWidth))
            && (event.absY >= info.sinkWinPhyY)  && (event.absY <= (info.sinkWinPhyY + info.sinkProjPhyHeight))) {
            return true;
        }
    }
    return false;
}

bool DistributedInputClient::IsJsonData(std::string strData) const
{
    return true;
}

bool DistributedInputClient::IsStartDistributedInput(const std::string &dhId)
{
    return true;
}

int32_t DistributedInputClient::RegisterSimulationEventListener(sptr<ISimulationEventListener> listener)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::UnregisterSimulationEventListener(sptr<ISimulationEventListener>  listener)
{
    return DH_SUCCESS;
}

int32_t DistributedInputClient::RegisterSessionStateCb(sptr<ISessionStateCallback> callback)
{
    (void)callback;
    return DH_SUCCESS;
}

int32_t DistributedInputClient::UnregisterSessionStateCb()
{
    return DH_SUCCESS;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
