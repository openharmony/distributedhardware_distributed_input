/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "dinput_state.h"

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
IMPLEMENT_SINGLE_INSTANCE(DInputState);
DInputState::~DInputState()
{
    Release();
}

int32_t DInputState::Init()
{
    DHLOGI("DInputState Init.");
    return DH_SUCCESS;
}

int32_t DInputState::Release()
{
    DHLOGI("DInputState Release.");
    {
        std::lock_guard<std::mutex> mapLock(operationMutex_);
        dhIdStateMap_.clear();
    }
    ClearDeviceStates();
    {
        std::lock_guard<std::mutex> lock(absPosMtx_);
        absPositionsMap_.clear();
    }
    return DH_SUCCESS;
}

int32_t DInputState::RecordDhIds(const std::vector<std::string> &dhIds, DhIdState state, const int32_t sessionId)
{
    DHLOGI("RecordDhIds dhIds size = %zu", dhIds.size());
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    for (const auto &dhid : dhIds) {
        DHLOGD("add dhid : %s, state : %d.", GetAnonyString(dhid).c_str(), state);
        dhIdStateMap_[dhid] = state;
    }

    if (state == DhIdState::THROUGH_OUT) {
        SimulateEventInjectToSrc(sessionId, dhIds);
    }
    return DH_SUCCESS;
}

int32_t DInputState::RemoveDhIds(const std::vector<std::string> &dhIds)
{
    DHLOGI("RemoveDhIds dhIds size = %zu", dhIds.size());
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    for (const auto &dhid : dhIds) {
        DHLOGD("delete dhid : %s", GetAnonyString(dhid).c_str());
        dhIdStateMap_.erase(dhid);
    }
    return DH_SUCCESS;
}

DhIdState DInputState::GetStateByDhid(const std::string &dhId)
{
    std::lock_guard<std::mutex> mapLock(operationMutex_);
    if (dhIdStateMap_.find(dhId) == dhIdStateMap_.end()) {
        DHLOGE("dhId : %s not exist.", GetAnonyString(dhId).c_str());
        return DhIdState::THROUGH_IN;
    }
    return dhIdStateMap_[dhId];
}

void DInputState::SimulateEventInjectToSrc(const int32_t sessionId, const std::vector<std::string> &dhIds)
{
    DHLOGI("SimulateEventInject enter, sessionId %d, dhIds size %d", sessionId, dhIds.size());
    // mouse/keyboard/touchpad/touchscreen event send to remote device if these device pass through.
    if (sessionId == -1) {
        DHLOGE("SimulateEventInjectToSrc SessionId invalid");
        return;
    }

    for (const std::string &dhId : dhIds) {
        // check if this device is key event
        std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
        auto iter = keyDownStateMap_.find(dhId);
        if (iter == keyDownStateMap_.end()) {
            DHLOGI("The shared Device not has down state key, dhId: %s", dhId.c_str());
            continue;
        }

        for (const auto &event : iter->second) {
            if (event.code == BTN_TOUCH) {
                DHLOGI("Simulate Touch Down event for device path: %s, dhId: %s",
                    event.path.c_str(), event.descriptor.c_str());
                SimulateBtnTouchEvent(sessionId, dhId, event);
            } else {
                DHLOGI("Simulate Key event for device path: %s, dhId: %s",
                    event.path.c_str(), event.descriptor.c_str());
                SimulateNormalEvent(sessionId, dhId, event);
            }
        }

        keyDownStateMap_.erase(dhId);
    }
}

void DInputState::SimulateBtnTouchEvent(const int32_t sessionId, const std::string &dhId, const struct RawEvent &event)
{
    int32_t simTrackingId = GetRandomInt32();
    std::vector<struct RawEvent> simEvents;
    RawEvent touchTrackingIdEv = { event.when, EV_ABS, ABS_MT_TRACKING_ID, simTrackingId, dhId, event.path };
    simEvents.push_back(touchTrackingIdEv);

    std::pair<int32_t, int32_t> absPos = GetAndClearABSPosition(event.descriptor);
    if (absPos.first != -1) {
        RawEvent absMTXEv = { event.when, EV_ABS, ABS_MT_POSITION_X, absPos.first, dhId, event.path };
        simEvents.push_back(absMTXEv);
    }
    if (absPos.second != -1) {
        RawEvent absMTYEv = { event.when, EV_ABS, ABS_MT_POSITION_Y, absPos.second, dhId, event.path };
        simEvents.push_back(absMTYEv);
    }

    RawEvent keyDownEv = { event.when, EV_KEY, event.code, KEY_DOWN_STATE, dhId, event.path };
    simEvents.push_back(keyDownEv);
    RawEvent fingerEv = { event.when, EV_KEY, BTN_TOOL_FINGER, KEY_DOWN_STATE, dhId, event.path };
    simEvents.push_back(fingerEv);

    if (absPos.first != -1) {
        RawEvent absXEv = { event.when, EV_ABS, ABS_X, absPos.first, dhId, event.path };
        simEvents.push_back(absXEv);
    }
    if (absPos.second != -1) {
        RawEvent absYEv = { event.when, EV_ABS, ABS_Y, absPos.second, dhId, event.path };
        simEvents.push_back(absYEv);
    }

    RawEvent mscEv = { event.when, EV_MSC, MSC_TIMESTAMP, 0x0, dhId, event.path };
    simEvents.push_back(mscEv);
    RawEvent sycReportEv = { event.when, EV_SYN, SYN_REPORT, 0x0, dhId, event.path };
    simEvents.push_back(sycReportEv);

    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsgBatch(sessionId, simEvents);
}

void DInputState::SimulateNormalEvent(const int32_t sessionId, const std::string &dhId, const struct RawEvent &event)
{
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, dhId,
        EV_KEY, event.code, KEY_DOWN_STATE);
    DistributedInputSinkTransport::GetInstance().SendKeyStateNodeMsg(sessionId, dhId,
        EV_SYN, SYN_REPORT, 0x0);
}

void DInputState::RefreshABSPosition(const std::string &dhId, int32_t absX, int32_t absY)
{
    std::lock_guard<std::mutex> lock(absPosMtx_);
    if (absX != -1) {
        absPositionsMap_[dhId].first = absX;
    }

    if (absY != -1) {
        absPositionsMap_[dhId].second = absY;
    }
}

std::pair<int32_t, int32_t> DInputState::GetAndClearABSPosition(const std::string &dhId)
{
    std::lock_guard<std::mutex> lock(absPosMtx_);
    std::pair<int32_t, int32_t> absPos = { -1, -1 };
    if (absPositionsMap_.find(dhId) == absPositionsMap_.end()) {
        return absPos;
    }

    absPos = absPositionsMap_[dhId];
    absPositionsMap_.erase(dhId);
    return absPos;
}

void DInputState::AddKeyDownState(struct RawEvent event)
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    keyDownStateMap_[event.descriptor].push_back(event);
}

void DInputState::RemoveKeyDownState(struct RawEvent event)
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

void DInputState::CheckAndSetLongPressedKeyOrder(struct RawEvent event)
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    auto iter = keyDownStateMap_.find(event.descriptor);
    if (iter == keyDownStateMap_.end()) {
        return;
    }

    auto evIter = std::find(keyDownStateMap_[event.descriptor].begin(),
        keyDownStateMap_[event.descriptor].end(), event);
    // If not find the cache key on pressing, or it is already the last one, just return
    if (evIter == keyDownStateMap_[event.descriptor].end() ||
        evIter == (keyDownStateMap_[event.descriptor].end() - 1)) {
        return;
    }

    // Ohterwhise, move the key to the last cached position.
    RawEvent backEv = *evIter;
    keyDownStateMap_[event.descriptor].erase(evIter);
    keyDownStateMap_[event.descriptor].push_back(backEv);
    DHLOGI("Find long pressed key: %d, move the cached pressed key: %d to the last position", event.code, backEv.code);
}

void DInputState::ClearDeviceStates()
{
    std::lock_guard<std::mutex> mapLock(keyDownStateMapMtx_);
    keyDownStateMap_.clear();
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOSs