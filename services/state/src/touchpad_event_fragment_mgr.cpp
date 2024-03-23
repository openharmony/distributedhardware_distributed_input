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

#include "touchpad_event_fragment_mgr.h"

#include <linux/input.h>

#include "dinput_log.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
bool TouchPadEventFragmentMgr::IsPositionEvent(const RawEvent &event)
{
    if (event.type == EV_ABS && (event.code == ABS_MT_POSITION_X || event.code == ABS_MT_POSITION_Y ||
        event.code == ABS_X || event.code == ABS_Y)) {
        return true;
    }

    return false;
}

bool TouchPadEventFragmentMgr::IsSynEvent(const RawEvent &event)
{
    return event.type == EV_SYN && event.code == SYN_REPORT;
}

bool TouchPadEventFragmentMgr::IsWholeTouchFragments(const std::vector<TouchPadEventFragment> &events)
{
    return events.front().IsTouchPadOptStart() && events.back().IsTouchPadOptFinish();
}

std::pair<bool, std::vector<RawEvent>> TouchPadEventFragmentMgr::PushEvent(const std::string &dhId,
    const RawEvent &event)
{
    if (IsPositionEvent(event)) {
        return {false, {}};
    }
    std::lock_guard<std::mutex> lock(fragmentsMtx_);
    if (fragments_.find(dhId) == fragments_.end()) {
        fragments_[dhId] = {{}};
    }

    fragments_[dhId].back().PushEvent(event);
    if (IsSynEvent(event)) {
        return DealSynEvent(dhId);
    }
    return {false, {}};
}

std::pair<bool, std::vector<RawEvent>> TouchPadEventFragmentMgr::DealSynEvent(const std::string &dhId)
{
    if (fragments_[dhId].back().IsTouchPadOptFinish()) {
        bool needSim = false;
        std::vector<RawEvent> allEvents = {};
        if (!IsWholeTouchFragments(fragments_[dhId])) {
            // If not whole touch events, this means the down event occurs on the other device,
            // so we need simulate the up actions to the other side to reset the touchpad states.
            for (auto &frag : fragments_[dhId]) {
                std::vector<RawEvent> fragEvents = frag.GetEvents();
                allEvents.insert(allEvents.end(), fragEvents.begin(), fragEvents.end());
            }
            needSim = true;
            DHLOGI("Find NOT Whole touchpad events need send back, dhId: %{public}s", GetAnonyString(dhId).c_str());
        }
        fragments_[dhId].clear();
        fragments_[dhId].push_back({});
        return {needSim, allEvents};
    }

    if (fragments_[dhId].back().IsShouldDrop()) {
        fragments_[dhId].pop_back();
    }
    fragments_[dhId].push_back({});
    return {false, {}};
}

void TouchPadEventFragmentMgr::Clear(const std::string &dhId)
{
    std::lock_guard<std::mutex> lock(fragmentsMtx_);
    fragments_.erase(dhId);
}

std::vector<RawEvent> TouchPadEventFragmentMgr::GetAndClearEvents(const std::string &dhId)
{
    std::lock_guard<std::mutex> lock(fragmentsMtx_);
    std::vector<RawEvent> allEvents;
    if (fragments_.find(dhId) == fragments_.end()) {
        return {};
    }

    for (auto &frag : fragments_[dhId]) {
        std::vector<RawEvent> fragEvents = frag.GetEvents();
        allEvents.insert(allEvents.end(), fragEvents.begin(), fragEvents.end());
    }

    fragments_.erase(dhId);
    return allEvents;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS