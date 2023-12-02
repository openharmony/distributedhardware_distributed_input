/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_INJECT_H
#define DISTRIBUTED_INPUT_INJECT_H

#include <mutex>
#include <set>
#include <string>

#include "constants_dinput.h"
#include "distributed_input_handler.h"
#include "distributed_input_node_manager.h"
#include "i_input_node_listener.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputInject {
public:
    static DistributedInputInject &GetInstance();
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &parameters);
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId);
    int32_t RegisterDistributedEvent(const std::string &devId, const std::vector<RawEvent> &events);
    int32_t StructTransJson(const InputDevice &pBuf, std::string &strDescriptor);
    void StartInjectThread();
    void StopInjectThread();
    std::string GenerateVirtualTouchScreenDHId(const uint64_t sourceWinId, const uint32_t sourceWinWidth,
        const uint32_t sourceWinHeight);
    int32_t CreateVirtualTouchScreenNode(const std::string &devId, const std::string &dhId, const uint64_t srcWinId,
        const uint32_t sourcePhyWidth, const uint32_t sourcePhyHeight);
    int32_t RemoveVirtualTouchScreenNode(const std::string &devId, const std::string &dhId);
    int32_t GetVirtualTouchScreenFd();
    int32_t RegisterInjectEventCb(sptr<ISessionStateCallback> callback);
    int32_t UnregisterInjectEventCb();

    void NotifyNodeMgrScanVirNode(const std::string &devId, const std::string &dhId);
    void GetVirtualKeyboardPaths(const std::string &devId, const std::vector<std::string> &dhIds,
        std::vector<std::string> &virKeyboardPaths);
private:
    DistributedInputInject();
    ~DistributedInputInject();

    std::unique_ptr<DistributedInputNodeManager> inputNodeManager_;
    std::mutex inputNodeManagerMutex_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_INJECT_H