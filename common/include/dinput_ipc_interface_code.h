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
enum class IAddWhiteListInfosCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4809 */
enum class IDelWhiteListInfosCBInterfaceCode : uint32_t {
    RESULT = 0
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

/* SAID: 4809 */
enum class IDInputSourceInterfaceCode : uint32_t {
    INIT = 0xf001,
    RELEASE = 0xf002,
    REGISTER_REMOTE_INPUT = 0xf003,
    UNREGISTER_REMOTE_INPUT = 0xf004,
    PREPARE_REMOTE_INPUT = 0xf005,
    UNPREPARE_REMOTE_INPUT = 0xf006,
    START_REMOTE_INPUT = 0xf007,
    STOP_REMOTE_INPUT = 0xf008,
    PREPARE_RELAY_REMOTE_INPUT = 0xf00a,
    UNPREPARE_RELAY_REMOTE_INPUT = 0xf00b,
    START_RELAY_TYPE_REMOTE_INPUT = 0xf00c,
    STOP_RELAY_TYPE_REMOTE_INPUT = 0xf00d,
    START_DHID_REMOTE_INPUT = 0xf00e,
    STOP_DHID_REMOTE_INPUT = 0xf00f,
    START_RELAY_DHID_REMOTE_INPUT = 0xf010,
    STOP_RELAY_DHID_REMOTE_INPUT = 0xf011,
    REGISTER_ADD_WHITE_LIST_CB_REMOTE_INPUT = 0xf013,
    REGISTER_DEL_WHITE_LIST_CB_REMOTE_INPUT = 0xf014,
    REGISTER_NODE_LISTENER = 0xf015,
    UNREGISTER_NODE_LISTENER = 0xf016,
    REGISTER_SIMULATION_EVENT_LISTENER = 0xf017,
    UNREGISTER_SIMULATION_EVENT_LISTENER = 0xf018,
    SYNC_NODE_INFO_REMOTE_INPUT = 0xf019,
};

/* SAID: 4810 */
enum class IGetSinkScreenInfosCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4810 */
enum class InputNodeListenerInterfaceCode : uint32_t {
    RESULT_ON = 0, 
    RESULT_OFF = 1
};

/* SAID: 4809 */
enum class IPrepareDInputCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4809 */
enum class IRegisterDInputCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4809 */
enum class ISharingDhIdListenerInterfaceCode : uint32_t {
    SHARING = 0,
    NO_SHARING = 1
};

/* SAID: 4809 */
enum class ISimulationEventListenerInterfaceCode : uint32_t {
    RESULT_ON = 0
};

/* SAID: 4809 */
enum class IStartDInputCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4809 */
enum class IStartStopDInputsCBInterfaceCode : uint32_t {
    RESULT_STRING = 0
};

/* SAID: 4809 */
enum class IStartStopResultCBInterfaceCode : uint32_t {
    RESULT_START = 0,
    RESULT_STOP = 1
};

/* SAID: 4809 */
enum class IStopDInputCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4809 */
enum class IUnprepareDInputCBInterfaceCode : uint32_t {
    RESULT = 0
};

/* SAID: 4809 */
enum class IUnregisterDInputCBInterfaceCode : uint32_t {
    RESULT = 0
};
} // namespace DistributedHardware
} // namespace OHOS
#endif