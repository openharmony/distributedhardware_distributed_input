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

#ifndef DISTRIBUTED_INPUT_KIT_H
#define DISTRIBUTED_INPUT_KIT_H

#include <string>
#include <vector>

#include "constants_dinput.h"
#include "dinput_log.h"
#include "distributed_input_client.h"
#include "i_prepare_d_input_call_back.h"
#include "i_register_d_input_call_back.h"
#include "i_start_d_input_call_back.h"
#include "i_stop_d_input_call_back.h"
#include "i_start_stop_d_inputs_call_back.h"
#include "i_start_stop_result_call_back.h"
#include "i_unregister_d_input_call_back.h"
#include "i_unprepare_d_input_call_back.h"
#include "i_session_state_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputKit {
public:

    static int32_t PrepareRemoteInput(const std::string &sinkId, sptr<IPrepareDInputCallback> callback);
    static int32_t UnprepareRemoteInput(const std::string &sinkId, sptr<IUnprepareDInputCallback> callback);

    static int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IPrepareDInputCallback> callback);
    static int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IUnprepareDInputCallback> callback);

    static int32_t StartRemoteInput(
        const std::string &sinkId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback);
    static int32_t StopRemoteInput(
        const std::string &sinkId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback);

    static int32_t StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback);
    static int32_t StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback);

    static int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStartDInputCallback> callback);
    static int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStopDInputCallback> callback);

    static int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback);
    static int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback);

    static bool IsNeedFilterOut(const std::string &sinkId, const BusinessEvent &event);
    static bool IsTouchEventNeedFilterOut(const TouchScreenEvent &event);

    static DInputServerType IsStartDistributedInput(const uint32_t &inputType);
    /*
     * check is dhId sharing to other devices
     * true: dhId sharing to other device
     * false: dhId NOT sharing to other device
     */
    static bool IsStartDistributedInput(const std::string &dhId);

    static int32_t RegisterSimulationEventListener(sptr<ISimulationEventListener> listener);
    static int32_t UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener);

    static int32_t RegisterSessionStateCb(sptr<ISessionStateCallback> callback);
    static int32_t UnregisterSessionStateCb();
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_KIT_H
