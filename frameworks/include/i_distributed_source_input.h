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
 * See the License for the specific language governing DistributedInputs and
 * limitations under the License.
 */

#ifndef I_DISTRIBUTED_SOURCE_INPUT_H
#define I_DISTRIBUTED_SOURCE_INPUT_H

#include <string>
#include <vector>

#include "iremote_broker.h"
#include "iremote_object.h"

#include "constants_dinput.h"
#include "i_add_white_list_infos_call_back.h"
#include "i_del_white_list_infos_call_back.h"
#include "i_prepare_d_input_call_back.h"
#include "i_register_d_input_call_back.h"
#include "i_start_d_input_call_back.h"
#include "i_stop_d_input_call_back.h"
#include "i_start_stop_d_inputs_call_back.h"
#include "i_start_stop_result_call_back.h"
#include "i_unprepare_d_input_call_back.h"
#include "i_unregister_d_input_call_back.h"
#include "i_input_node_listener.h"
#include "i_simulation_event_listener.h"
#include "i_session_state_callback.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class IDistributedSourceInput : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.DistributedHardware.DistributedInput.IDistributedSourceInput");

    virtual int32_t Init() = 0;

    virtual int32_t Release() = 0;

    virtual int32_t RegisterDistributedHardware(
        const std::string &devId, const std::string &dhId, const std::string &parameters,
        sptr<IRegisterDInputCallback> callback) = 0;

    virtual int32_t UnregisterDistributedHardware(
        const std::string &devId, const std::string &dhId,
        sptr<IUnregisterDInputCallback> callback) = 0;

    virtual int32_t PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> callback) = 0;

    virtual int32_t UnprepareRemoteInput(const std::string &deviceId, sptr<IUnprepareDInputCallback> callback) = 0;

    virtual int32_t StartRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback) = 0;

    virtual int32_t StopRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback) = 0;

    virtual int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStartDInputCallback> callback) = 0;

    virtual int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStopDInputCallback> callback) = 0;

    virtual int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IPrepareDInputCallback> callback) = 0;

    virtual int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IUnprepareDInputCallback> callback) = 0;

    virtual int32_t StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback) = 0;

    virtual int32_t StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback) = 0;

    virtual int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback) = 0;

    virtual int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId,
        const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> callback) = 0;

    virtual int32_t RegisterAddWhiteListCallback(sptr<IAddWhiteListInfosCallback> addWhiteListCallback) = 0;
    virtual int32_t RegisterDelWhiteListCallback(sptr<IDelWhiteListInfosCallback> delWhiteListCallback) = 0;

    virtual int32_t RegisterSimulationEventListener(sptr<ISimulationEventListener> listener) = 0;
    virtual int32_t UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener) = 0;
    virtual int32_t RegisterSessionStateCb(sptr<ISessionStateCallback> callback) = 0;
    virtual int32_t UnregisterSessionStateCb() = 0;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // I_DISTRIBUTED_SOURCE_INPUT_H
