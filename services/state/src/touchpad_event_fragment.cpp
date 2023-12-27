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

#include "touchpad_event_fragment.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
namespace {
    constexpr size_t MIN_EVENT_FRAG_SIZE = 2;
}
bool TouchPadEventFragment::IsShouldDrop()
{
    return events.size() == MIN_EVENT_FRAG_SIZE;
}

bool TouchPadEventFragment::IsTouchPadOptFinish() const
{
    bool isFinish = false;
    for (const auto &ev : events) {
        if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == KEY_UP_STATE) {
            isFinish = true;
            break;
        }
    }

    return isFinish;
}

bool TouchPadEventFragment::IsTouchPadOptStart() const
{
    bool isStart = false;
    for (const auto &ev : events) {
        if (ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == KEY_DOWN_STATE) {
            isStart = true;
            break;
        }
    }

    return isStart;
}

void TouchPadEventFragment::PushEvent(const RawEvent &event)
{
    this->events.push_back(event);
}

std::vector<RawEvent> TouchPadEventFragment::GetEvents()
{
    return this->events;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS