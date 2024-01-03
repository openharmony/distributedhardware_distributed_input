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

#ifndef DISTRIBUTED_INPUT_TOUCHPAD_EVENT_FRAGMENT_MGR_H
#define DISTRIBUTED_INPUT_TOUCHPAD_EVENT_FRAGMENT_MGR_H

#include "touchpad_event_fragment.h"

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "constants_dinput.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class TouchPadEventFragmentMgr {
public:
    TouchPadEventFragmentMgr() : fragments_({}) {}

    /**
     * @brief Push the touchpad event
     *
     * @param dhId the device dhId where event from
     * @param event the event
     * @return std::pair<bool, std::vector<RawEvent>>
     *     true for NOT whole touchpad event that need simulate back to the other device.
     *     false for DO Nothing.
     */
    std::pair<bool, std::vector<RawEvent>> PushEvent(const std::string &dhId, const RawEvent &event);
    void Clear(const std::string &dhId);
    std::vector<RawEvent> GetAndClearEvents(const std::string &dhId);

private:
    bool IsPositionEvent(const RawEvent &event);
    bool IsSynEvent(const RawEvent &event);
    bool IsWholeTouchFragments(const std::vector<TouchPadEventFragment> &fragments);
    std::pair<bool, std::vector<RawEvent>> DealSynEvent(const std::string &dhId);
private:
    std::mutex fragmentsMtx_;
    // record the event fragments for the dhid. { dhId, { events }}
    std::map<std::string, std::vector<TouchPadEventFragment>> fragments_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_INPUT_TOUCHPAD_EVENT_FRAGMENT_MGR_H