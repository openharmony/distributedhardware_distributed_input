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

#ifndef OHOS_DINPUT_IPC_INTERFACE_CODE_H
#define OHOS_DINPUT_IPC_INTERFACE_CODE_H

#include <cstdint>

namespace OHOS {
namespace DistributedHardware {
/* SAID: 4809 */
enum class IDInputSourceInterfaceCode : uint32_t {
    INIT = 0xf001U,
    RELEASE = 0xf002U,
    REGISTER_REMOTE_INPUT = 0xf003U,
    UNREGISTER_REMOTE_INPUT = 0xf004U,
    PREPARE_REMOTE_INPUT = 0xf005U,
    UNPREPARE_REMOTE_INPUT = 0xf006U,
    START_REMOTE_INPUT = 0xf007U,
    STOP_REMOTE_INPUT = 0xf008U,
    PREPARE_RELAY_REMOTE_INPUT = 0xf00aU,
    UNPREPARE_RELAY_REMOTE_INPUT = 0xf00bU,
    START_RELAY_TYPE_REMOTE_INPUT = 0xf00cU,
    STOP_RELAY_TYPE_REMOTE_INPUT = 0xf00dU,
    START_DHID_REMOTE_INPUT = 0xf00eU,
    STOP_DHID_REMOTE_INPUT = 0xf00fU,
    START_RELAY_DHID_REMOTE_INPUT = 0xf010U,
    STOP_RELAY_DHID_REMOTE_INPUT = 0xf011U,
    REGISTER_ADD_WHITE_LIST_CB_REMOTE_INPUT = 0xf013U,
    REGISTER_DEL_WHITE_LIST_CB_REMOTE_INPUT = 0xf014U,
    REGISTER_SIMULATION_EVENT_LISTENER = 0xf017U,
    UNREGISTER_SIMULATION_EVENT_LISTENER = 0xf018U,
    SYNC_NODE_INFO_REMOTE_INPUT = 0xf019U,
    REGISTER_SESSION_STATE_CB = 0xf01aU,
    UNREGISTER_SESSION_STATE_CB = 0xf01bU,
};

/* SAID: 4810 */
enum class IDInputSinkInterfaceCode : uint32_t {
    INIT = 0xf011U,
    RELEASE = 0xf012U,
    NOTIFY_START_DSCREEN = 0xf013U,
    NOTIFY_STOP_DSCREEN = 0xf014U,
    REGISTER_SHARING_DHID_LISTENER = 0xf015U,
    GET_SINK_SCREEN_INFOS = 0xf016U,
};
} // namespace DistributedHardware
} // namespace OHOS
#endif