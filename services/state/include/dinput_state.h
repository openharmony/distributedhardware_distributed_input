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

#ifndef DISTRIBUTED_INPUT_STATE_BASE_H
#define DISTRIBUTED_INPUT_STATE_BASE_H

#include <map>
#include <mutex>
#include <string>
#include <linux/input.h>

#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
/*
 * This enumeration class represents the two states of the peropheral:
 * THROUGH_IN : The state indicates the peripheral takes effect on the local device.
 * THROUGH_OUT : The state indicates that the peripheral takes effect at the remote device.
*/
enum class DhIdState {
    THROUGH_IN = 0,
    THROUGH_OUT,
};

class DInputState {
    DECLARE_SINGLE_INSTANCE_BASE(DInputState);
public:
    int32_t Init();
    int32_t Release();
    int32_t RecordDhIds(const std::vector<std::string> &dhIds, DhIdState state, const int32_t sessionId);
    int32_t RemoveDhIds(const std::vector<std::string> &dhIds);
    DhIdState GetStateByDhid(const std::string &dhId);

private:
    DInputState() = default;
    ~DInputState();

    void CreateSpecialEventInjectThread(const int32_t sessionId, const std::vector<std::string> &dhIds);
    void CheckKeyboardState(const std::string &dhId, const std::string &keyboardNodePath,
        std::vector<uint32_t> &pressedKeys, int &fd);
    void SpecEventInject(const int32_t sessionId, const std::vector<std::string> &dhIds);
    void RecordEventLog(const input_event &event);
    void WriteEventToDev(const int fd, const input_event &event);
    void SyncMouseKeyState(const int32_t sessionId, const std::string &mouseNodePath,
        const std::string &mouseNodeDhId);

private:
    std::mutex operationMutex_;
    std::map<std::string, DhIdState> dhIdStateMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_INPUT_STATE_BASE_H