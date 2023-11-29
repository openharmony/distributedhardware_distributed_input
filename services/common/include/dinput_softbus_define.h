/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_SOFTBUS_DEFINE_H
#define DISTRIBUTED_INPUT_SOFTBUS_DEFINE_H

#include <string>
#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
    struct InnerMsgData {
        std::string deviceId;
        std::string dataStr;
        InnerMsgData(std::string devId, std::string data) : deviceId(devId), dataStr(data) {};
    };

    struct SwitchStateData {
        int32_t sessionId;
        bool switchState;
        SwitchStateData(int32_t session, bool state) : sessionId(session), switchState(state) {};
    };

    const int32_t ENCRYPT_TAG_LEN = 32;
    const int32_t MSG_MAX_SIZE = 45 * 1024;

    const uint32_t SESSION_NAME_SIZE_MAX = 256;
    const uint32_t DEVICE_ID_SIZE_MAX = 65;
    const uint32_t INTERCEPT_STRING_LENGTH = 20;

    const std::string DINPUT_PKG_NAME = "ohos.dhardware.dinput";
    const std::string SESSION_NAME = "ohos.dhardware.dinput.session";
    const std::string GROUP_ID = "input_softbus_group_id";

    #define DINPUT_SOFTBUS_KEY_CMD_TYPE "dinput_softbus_key_cmd_type"
    #define DINPUT_SOFTBUS_KEY_DEVICE_ID "dinput_softbus_key_device_id"
    #define DINPUT_SOFTBUS_KEY_SESSION_ID "dinput_softbus_key_session_id"
    #define DINPUT_SOFTBUS_KEY_INPUT_TYPE "dinput_softbus_key_input_type"
    #define DINPUT_SOFTBUS_KEY_VECTOR_DHID "dinput_softbus_key_vector_dhid"
    #define DINPUT_SOFTBUS_KEY_RESP_VALUE "dinput_softbus_key_resp_value"
    #define DINPUT_SOFTBUS_KEY_WHITE_LIST "dinput_softbus_key_list_list"
    #define DINPUT_SOFTBUS_KEY_INPUT_DATA "dinput_softbus_key_input_data"
    #define DINPUT_SOFTBUS_KEY_KEYSTATE_DHID "dinput_softbus_key_keystate_dhid"
    #define DINPUT_SOFTBUS_KEY_KEYSTATE_TYPE "dinput_softbus_key_keystate_type"
    #define DINPUT_SOFTBUS_KEY_KEYSTATE_CODE "dinput_softbus_key_keystate_code"
    #define DINPUT_SOFTBUS_KEY_KEYSTATE_VALUE "dinput_softbus_key_keystate_value"
    #define DINPUT_SOFTBUS_KEY_SRC_DEV_ID "dinput_softbus_key_src_dev_id"
    #define DINPUT_SOFTBUS_KEY_SINK_DEV_ID "dinput_softbus_key_sink_dev_id"

    // src will receive
    const uint32_t TRANS_SINK_MSG_ONPREPARE    = 1;
    const uint32_t TRANS_SINK_MSG_ONUNPREPARE  = 2;
    const uint32_t TRANS_SINK_MSG_ONSTART      = 3;
    const uint32_t TRANS_SINK_MSG_ONSTOP       = 4;
    const uint32_t TRANS_SINK_MSG_BODY_DATA    = 5;
    const uint32_t TRANS_SINK_MSG_LATENCY      = 6;
    const uint32_t TRANS_SINK_MSG_DHID_ONSTART = 7;
    const uint32_t TRANS_SINK_MSG_DHID_ONSTOP  = 8;
    const uint32_t TRANS_SINK_MSG_KEY_STATE    = 9;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_PREPARE    = 10;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE  = 11;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_PREPARE_RESULT    = 12;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_UNPREPARE_RESULT  = 13;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_START_DHID    = 14;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID    = 15;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_START_DHID_RESULT  = 16;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_STOP_DHID_RESULT   = 17;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE    = 18;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE    = 19;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_START_TYPE_RESULT  = 20;
    const uint32_t TRANS_SOURCE_TO_SOURCE_MSG_STOP_TYPE_RESULT   = 21;
    const uint32_t TRANS_SINK_MSG_ON_RELAY_PREPARE    = 22;
    const uint32_t TRANS_SINK_MSG_ON_RELAY_UNPREPARE  = 23;
    const uint32_t TRANS_SINK_MSG_ON_RELAY_STARTDHID  = 24;
    const uint32_t TRANS_SINK_MSG_ON_RELAY_STOPDHID   = 25;
    const uint32_t TRANS_SINK_MSG_ON_RELAY_STARTTYPE  = 26;
    const uint32_t TRANS_SINK_MSG_ON_RELAY_STOPTYPE   = 27;
    const uint32_t TRANS_SINK_MSG_KEY_STATE_BATCH     = 28;

    // src or sink
    const uint32_t TRANS_MSG_SRC_SINK_SPLIT    = 30;

    // sink will receive
    const uint32_t TRANS_SOURCE_MSG_PREPARE    = 31;
    const uint32_t TRANS_SOURCE_MSG_UNPREPARE  = 32;
    const uint32_t TRANS_SOURCE_MSG_START_TYPE = 33;
    const uint32_t TRANS_SOURCE_MSG_STOP_TYPE  = 34;
    const uint32_t TRANS_SOURCE_MSG_START_DHID = 35;
    const uint32_t TRANS_SOURCE_MSG_STOP_DHID  = 36;
    const uint32_t TRANS_SOURCE_MSG_LATENCY    = 37;
    const uint32_t TRANS_SOURCE_MSG_START_DHID_FOR_REL   = 38;
    const uint32_t TRANS_SOURCE_MSG_STOP_DHID_FOR_REL    = 39;
    const uint32_t TRANS_SOURCE_MSG_START_TYPE_FOR_REL   = 40;
    const uint32_t TRANS_SOURCE_MSG_STOP_TYPE_FOR_REL    = 41;
    const uint32_t TRANS_SOURCE_MSG_PREPARE_FOR_REL      = 42;
    const uint32_t TRANS_SOURCE_MSG_UNPREPARE_FOR_REL    = 43;
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_INPUT_SOFTBUS_DEFINE_H
