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

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <map>
#include <mutex>
#include <pthread.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
enum class DhidState {
    INIT = 0,
    THROUGH_IN,
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
    int32_t AddDhids(const std::vector<std::string> &dhids);
    int32_t DeleteDhids(const std::vector<std::string> &dhids);
    int32_t SwitchState(const std::vector<std::string> &dhids, DhidState state);
    DhidState GetStateByDhid(std::string &dhid);

private:
    ~DInputState();

    void CreateKeyUpInjectThread(const std::vector<std::string> &dhids);
    void CheckKeyState(std::string &dhid, std::string &keyboardNodePath);
    void UpInject(int fd, std::vector<uint32_t> &keyboardPressedKeys, std::string &dhid);
    void KeyUpInject(std::vector<std::string> &shareDhidsPaths, std::vector<std::string> &shareDhIds);
    bool IsExistDhid(const std::string &dhid);
    void RecordEventLog(const input_event& event);

private:
    std::mutex operationMutex_;
    std::map<std::string, DhidState> dhidStateMap_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_INPUT_STATE_BASE_H