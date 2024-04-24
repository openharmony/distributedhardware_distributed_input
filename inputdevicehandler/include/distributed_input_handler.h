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

#ifndef DISTRIBUTED_INPUT_HANDLER_H
#define DISTRIBUTED_INPUT_HANDLER_H

#include <functional>
#include <map>
#include <mutex>
#include <string>

#include <sys/epoll.h>
#include <linux/input.h>

#include "ihardware_handler.h"
#include "single_instance.h"

#include "constants_dinput.h"
#include "input_hub.h"

#ifndef API_EXPORT
#define API_EXPORT __attribute__((visibility("default")))
#endif

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputHandler : public IHardwareHandler {
DECLARE_SINGLE_INSTANCE_BASE(DistributedInputHandler);
public:
    API_EXPORT int32_t Initialize() override;
    API_EXPORT virtual std::vector<DHItem> QueryMeta() override;
    API_EXPORT virtual std::vector<DHItem> Query() override;
    API_EXPORT virtual std::map<std::string, std::string> QueryExtraInfo() override;
    API_EXPORT bool IsSupportPlugin() override;
    API_EXPORT void RegisterPluginListener(std::shared_ptr<PluginListener> listener) override;
    API_EXPORT void UnRegisterPluginListener() override;

    API_EXPORT void FindDevicesInfoByType(const uint32_t inputTypes, std::map<int32_t, std::string> &datas);
    API_EXPORT void FindDevicesInfoByDhId(std::vector<std::string> dhidsVec, std::map<int32_t, std::string> &datas);
private:
    DistributedInputHandler();
    ~DistributedInputHandler();
    void StructTransJson(const InputDevice &pBuf, std::string &strDescriptor);
    std::shared_ptr<PluginListener> m_listener;
    bool InitCollectEventsThread();
    void NotifyHardWare(int iCnt);

    pthread_t collectThreadID_;
    bool isCollectingEvents_;
    bool isStartCollectEventThread;
    static void *CollectEventsThread(void *param);
    void StartInputMonitorDeviceThread();
    void StopInputMonitorDeviceThread();

    // The event queue.
    static const int inputDeviceBufferSize = 32;
    InputDeviceEvent mEventBuffer[inputDeviceBufferSize] = {};
    std::mutex operationMutex_;
    std::unique_ptr<InputHub> inputHub_;
};

#ifdef __cplusplus
extern "C" {
#endif
API_EXPORT IHardwareHandler* GetHardwareHandler();
#ifdef __cplusplus
}
#endif
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_HANDLER_H
