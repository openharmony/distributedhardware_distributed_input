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

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
/*
 * This enumeration class represents the two states of the peropheral:
 * THROUGH_IN : The state indicates the peripheral takes effect on the local device.
 * THROUGH_OUT : The state indicates that the peripheral takes effect at the remote device.
*/
enum class DhidState {
    THROUGH_IN = 0,
    THROUGH_OUT,
};

class DInputState {
public:
    static DInputState &GetInstance()
    {
        static DInputState instance;
        return instance;
    };

    int32_t Init();
    int32_t Release();
    int32_t RecordDhids(const std::vector<std::string> &dhids, DhidState state, const int32_t &sessionId);
    int32_t RemoveDhids(const std::vector<std::string> &dhids);
    DhidState GetStateByDhid(std::string &dhid);

private:
    ~DInputState();

    void CreateSpecialEventInjectThread(const int32_t &sessionId, const std::vector<std::string> &dhids);
    void CheckKeyboardState(std::string &dhid, std::string &keyboardNodePath,
        std::vector<uint32_t> &keyboardPressedKeys, int &fd);
    void SpecEventInject(const int32_t &sessionId, std::vector<std::string> dhids);
    bool IsDhidExist(const std::string &dhid);
    void RecordEventLog(const input_event &event);
    void WriteEventToDev(int &fd, const input_event &event);
    void CheckMouseKeyState(const int32_t &sessionId, const std::string &mouseNodePath,
        const std::string &mouseNodeDhId);

private:
    std::mutex operationMutex_;
    std::map<std::string, DhidState> dhidStateMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_INPUT_STATE_BASE_H