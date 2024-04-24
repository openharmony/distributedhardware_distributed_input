/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "dinput_sink_state.h"

#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "distributed_input_collector.h"
#include "distributed_input_sink_transport.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(DInputSinkState);
DInputSinkState::~DInputSinkState()
{
    Release();
}

int32_t DInputSinkState::Init()
{
    DHLOGI("DInputSinkState Init.");
    touchPadEventFragMgr_ = std::make_shared<TouchPadEventFragmentMgr>();
    return DH_SUCCESS;
}

int32_t DInputSinkState::Release()
{
    DHLOGI("DInputSinkState Release.");
    {
        std::lock_guard<std::mutex> mapLock(operationMutex_);
        dhIdStateMap_.clear();
    }
    ClearDeviceStates();
    return DH_SUCCESS;
}

int32_t DInputSinkState::RecordDhIds(const std::vector<std::string> &dhIds, DhIdState state, const int32_t sessionId)
{
    DHLOGI("RecordDhIds dhIds size = %{public}zu", dhIds.size());
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    for (const auto &dhid : dhIds) {
        DHLOGD("add dhid : %{public}s, state : %{public}d.", GetAnonyString(dhid).c_str(), state);
        dhIdStateMap_[dhid] = state;
    }

    if (state == DhIdState::THROUGH_OUT) {
        SimulateEventInjectToSrc(sessionId, dhIds);
    }
    lastSessionId_ = sessionId;
    return DH_SUCCESS;
}

int32_t DInputSinkState::RemoveDhIds(const std::vector<std::string> &dhIds)
{
    DHLOGI("RemoveDhIds dhIds size = %{public}zu", dhIds.size());
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    for (const auto &dhid : dhIds) {
        DHLOGD("delete dhid : %{public}s", GetAnonyString(dhid).c_str());
        dhIdStateMap_.erase(dhid);
    }
    return DH_SUCCESS;
}

std::shared_ptr<TouchPadEventFragmentMgr> DInputSinkState::GetTouchPadEventFragMgr()
{
    return this->touchPadEventFragMgr_;
}

DhIdState DInputSinkState::GetStateByDhid(const std::string &dhId)
{
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    if (dhIdStateMap_.find(dhId) == dhIdStateMap_.end()) {
        DHLOGE("dhId : %{public}s not exist.", GetAnonyString(dhId).c_str());
        return DhIdState::THROUGH_IN;
    }
    return dhIdStateMap_[dhId];
}

void DInputSinkState::SimulateMouseBtnMouseUpState(const std::string &dhId, const struct RawEvent &event)
{
    DHLOGI("Sinmulate Mouse BTN_MOUSE UP state to source, dhId: %{public}s", GetAnonyString(dhId).c_str());
    int32_t scanId = GetRandomInt32(0, INT32_MAX);
    RawEvent mscScanEv = { event.when, EV_MSC, MSC_SCAN, scanId, dhId, event.path };
    RawEvent btnMouseUpEv = { event.when, EV_KEY, BTN_MOUSE, KEY_UP_STATE, dhId, event.path };
    RawEvent sycReportEv = { event.when, EV_SYN, SYN_REPORT, 0x0, dhId, event.path };

    std::vector<RawEvent> simEvents = { mscScanEv, btnMouseUpEv, sycReportEv };
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsgBatch(lastSessionId_, simEvents);
}

void DInputSinkState::SimulateTouchPadStateReset(const std::vector<RawEvent> &events)
{
    DHLOGI("SimulateTouchPadStateReset events size: %{public}zu", events.size());
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsgBatch(lastSessionId_, events);
}

void DInputSinkState::SimulateEventInjectToSrc(const int32_t sessionId, const std::vector<std::string> &dhIds)
{
    DHLOGI("SimulateEventInject enter, sessionId %{public}d, dhIds size %{public}zu", sessionId, dhIds.size());
    // mouse/keyboard/touchpad/touchscreen event send to remote device if these device pass through.
    if (sessionId == -1) {
        DHLOGE("SimulateEventInjectToSrc SessionId invalid");
        return;
    }

    for (const std::string &dhId : dhIds) {
        SimulateKeyDownEvents(sessionId, dhId);
        SimulateTouchPadEvents(sessionId, dhId);
    }
}

void DInputSinkState::SimulateKeyDownEvents(const int32_t sessionId, const std::string &dhId)
{
    // check if this device is key event
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    auto iter = keyDownStateMap_.find(dhId);
    if (iter == keyDownStateMap_.end()) {
        DHLOGI("The shared Device not has down state key, dhId: %{public}s", GetAnonyString(dhId).c_str());
        return;
    }

    for (const auto &event : iter->second) {
        DHLOGI("Simulate Key event for device path: %{public}s, dhId: %{public}s",
            event.path.c_str(), GetAnonyString(event.descriptor).c_str());
        SimulateKeyDownEvent(sessionId, dhId, event);
    }

    keyDownStateMap_.erase(dhId);
}

void DInputSinkState::SimulateKeyDownEvent(const int32_t sessionId, const std::string &dhId,
    const struct RawEvent &event)
{
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, dhId,
        EV_KEY, event.code, KEY_DOWN_STATE);
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, dhId,
        EV_SYN, SYN_REPORT, 0x0);
}

void DInputSinkState::SimulateTouchPadEvents(const int32_t sessionId, const std::string &dhId)
{
    std::vector<RawEvent> events = this->touchPadEventFragMgr_->GetAndClearEvents(dhId);
    if (events.empty()) {
        return;
    }

    DHLOGI("SimulateTouchPadEvents dhId: %{public}s, event size: %{public}zu", GetAnonyString(dhId).c_str(),
        events.size());
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsgBatch(sessionId, events);
}

bool DInputSinkState::IsDhIdDown(const std::string &dhId)
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    auto iter = keyDownStateMap_.find(dhId);
    return iter != keyDownStateMap_.end();
}

void DInputSinkState::AddKeyDownState(struct RawEvent event)
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    keyDownStateMap_[event.descriptor].push_back(event);
}

void DInputSinkState::RemoveKeyDownState(struct RawEvent event)
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    auto iter = keyDownStateMap_.find(event.descriptor);
    if (iter == keyDownStateMap_.end()) {
        return;
    }

    auto evIter = std::find(keyDownStateMap_[event.descriptor].begin(),
        keyDownStateMap_[event.descriptor].end(), event);
    if (evIter == keyDownStateMap_[event.descriptor].end()) {
        return;
    }

    keyDownStateMap_[event.descriptor].erase(evIter);
    if (keyDownStateMap_[event.descriptor].empty()) {
        keyDownStateMap_.erase(event.descriptor);
    }
}

void DInputSinkState::CheckAndSetLongPressedKeyOrder(struct RawEvent event)
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    auto iter = keyDownStateMap_.find(event.descriptor);
    if (iter == keyDownStateMap_.end()) {
        DHLOGI("Find new pressed key, save it, node id: %{public}s, type: %{public}d, key code: %{public}d, "
            "value: %{public}d", GetAnonyString(event.descriptor).c_str(), event.type, event.code, event.value);
        keyDownStateMap_[event.descriptor].push_back(event);
        return;
    }

    auto evIter = std::find(keyDownStateMap_[event.descriptor].begin(),
        keyDownStateMap_[event.descriptor].end(), event);
    // If not find the cache key on pressing, save it
    if (evIter == keyDownStateMap_[event.descriptor].end()) {
        DHLOGI("Find new pressed key, save it, node id: %{public}s, type: %{public}d, key code: %{public}d, "
            "value: %{public}d", GetAnonyString(event.descriptor).c_str(), event.type, event.code, event.value);
        keyDownStateMap_[event.descriptor].push_back(event);
        return;
    }

    // it is already the last one, just return
    if (evIter == (keyDownStateMap_[event.descriptor].end() - 1)) {
        DHLOGI("Pressed key already last one, node id: %{public}s, type: %{public}d, key code: %{public}d, "
            "value: %{public}d", GetAnonyString(event.descriptor).c_str(), event.type, event.code, event.value);
        return;
    }

    // Ohterwhise, move the key to the last cached position.
    RawEvent backEv = *evIter;
    keyDownStateMap_[event.descriptor].erase(evIter);
    keyDownStateMap_[event.descriptor].push_back(backEv);
    DHLOGI("Find long pressed key: %{public}d, move the cached pressed key: %{public}d to the last position",
        event.code, backEv.code);
}

void DInputSinkState::ClearDeviceStates()
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    keyDownStateMap_.clear();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOSs