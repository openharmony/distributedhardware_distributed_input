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

#ifndef I_DISTRIBUTED_SINK_INPUT_H
#define I_DISTRIBUTED_SINK_INPUT_H

#include <string>

#include "iremote_broker.h"
#include "iremote_object.h"

#include "constants_dinput.h"
#include "i_dinput_context.h"
#include "i_distributed_sink_input.h"
#include "i_get_sink_screen_infos_call_back.h"
#include "i_sharing_dhid_listener.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class IDistributedSinkInput : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.DistributedHardware.DistributedInput.IDistributedSinkInput");

    /*
     * Init and Release are IPC interface,
     * which are used for interacting by dhareware and dinput
     */
    virtual int32_t Init() = 0;

    virtual int32_t Release() = 0;

    virtual int32_t RegisterGetSinkScreenInfosCallback(sptr<IGetSinkScreenInfosCallback> callback) = 0;

    /*
     * NotifyStartDScreen and NotifyStopDScreen are RPC interface,
     * which are used for interacting by dinput source and dinput sink
     */
    virtual int32_t NotifyStartDScreen(const SrcScreenInfo &remoteCtrlInfo) = 0;

    virtual int32_t NotifyStopDScreen(const std::string &srcScreenInfoKey) = 0;

    virtual int32_t RegisterSharingDhIdListener(sptr<ISharingDhIdListener> sharingDhIdListener) = 0;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // I_DISTRIBUTED_SINK_INPUT_H
