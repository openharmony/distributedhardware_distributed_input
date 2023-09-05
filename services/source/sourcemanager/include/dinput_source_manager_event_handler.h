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

#ifndef DINPUT_SOURCE_MANAGER_EVENT_HANDLER_H
#define DINPUT_SOURCE_MANAGER_EVENT_HANDLER_H

#include <cstring>
#include <mutex>
#include <set>

#include <unistd.h>
#include <sys/types.h>

#include "event_handler.h"
#include "ipublisher_listener.h"
#include "publisher_listener_stub.h"
#include "singleton.h"

#include "constants_dinput.h"
#include "dinput_context.h"
#include "dinput_source_manager_callback.h"
#include "dinput_source_trans_callback.h"
#include "distributed_input_source_manager.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputSourceManager;
class DInputSourceManagerEventHandler : public AppExecFwk::EventHandler {
public:
    DInputSourceManagerEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
        DistributedInputSourceManager *manager);
    ~DInputSourceManagerEventHandler() override;

    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
private:
    void NotifyRegisterCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyUnregisterCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyPrepareCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyUnprepareCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyStartCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyStopCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyStartDhidCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyStopDhidCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyKeyStateCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyStartServerCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayPrepareCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayUnprepareCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayStartDhidCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayStopDhidCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayStartTypeCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayStopTypeCallback(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayPrepareRemoteInput(const AppExecFwk::InnerEvent::Pointer &event);
    void NotifyRelayUnprepareRemoteInput(const AppExecFwk::InnerEvent::Pointer &event);

    using SourceEventFunc = void (DInputSourceManagerEventHandler::*)(
        const AppExecFwk::InnerEvent::Pointer &event);
    std::map<int32_t, SourceEventFunc> eventFuncMap_;
    DistributedInputSourceManager *sourceManagerObj_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DINPUT_SOURCE_MANAGER_EVENT_HANDLER_H