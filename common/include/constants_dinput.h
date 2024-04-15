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

#ifndef OHOS_DISTRIBUTED_INPUT_CONSTANTS_H
#define OHOS_DISTRIBUTED_INPUT_CONSTANTS_H

#include <map>
#include <string>
#include <vector>

#include <linux/uinput.h>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
#define INPUT_KEY_WHEN  "when"
#define INPUT_KEY_TYPE  "type"
#define INPUT_KEY_CODE  "code"
#define INPUT_KEY_VALUE "value"
#define INPUT_KEY_DESCRIPTOR  "descriptor"
#define INPUT_KEY_PATH "path"

#define VIRTUAL_DEVICE_NAME "DistributedInput "
#define LONG_BITS (sizeof(long) * 8)
#define NLONGS(x) (((x) + LONG_BITS - 1) / LONG_BITS)

    const char INPUT_STRING_SPLIT_POINT = '.';
    const uint32_t KEY_DOWN_STATE = 1;
    const uint32_t KEY_UP_STATE = 0;
    const uint32_t KEY_REPEAT = 2;
    const uint32_t READ_SLEEP_TIME_MS = 50;
    const uint32_t READ_RETRY_MAX = 5;
    const uint32_t DH_ID_LENGTH_MAX = 256;
    const uint32_t DEV_ID_LENGTH_MAX = 256;
    const uint32_t STRING_MAX_SIZE = 40 * 1024 * 1024;
    const uint32_t SCREEN_MSG_MAX = 40 * 1024 * 1024;
    const uint32_t AUTH_SESSION_SIDE_SERVER = 0;
    const uint32_t IPC_VECTOR_MAX_SIZE = 32;

    /*
     * Device Type definitions
     */
    enum class DInputDeviceType : uint32_t {
        NONE = 0x0000,
        MOUSE = 0x0001,
        KEYBOARD = 0x0002,
        TOUCHSCREEN = 0x0004,
        TOUCHPAD = MOUSE,
        ALL = MOUSE | KEYBOARD | TOUCHSCREEN,
    };

    const char * const DEVICE_PATH = "/dev/input";

    /*
     * Maximum number of signalled FDs to handle at a time.
     */
    constexpr uint32_t EPOLL_MAX_EVENTS = 16;

    /*
     * Maximum number of event buffer size.
     */
    constexpr uint32_t INPUT_EVENT_BUFFER_SIZE = 256;

    constexpr int32_t INPUT_LOAD_SA_TIMEOUT_MS = 10000;

    constexpr int32_t INPUT_LATENCY_DELAYTIME_US = 50 * 1000;

    constexpr uint32_t INPUT_LATENCY_DELAY_TIMES = 60;

    constexpr int32_t SESSION_WAIT_TIMEOUT_SECOND = 5;

    constexpr int32_t EPOLL_WAITTIME = 100;

    constexpr uint64_t WATCHDOG_INTERVAL_TIME_MS = 20 * 1000;

    /* The input device is a keyboard or has buttons. */
    constexpr uint32_t INPUT_DEVICE_CLASS_KEYBOARD      = 0x00000001;

    /* The input device is an alpha-numeric keyboard (not just a dial pad). */
    constexpr uint32_t INPUT_DEVICE_CLASS_ALPHAKEY      = 0x00000002;

    /* The input device is a touchscreen or a touchpad (either single-touch or multi-touch). */
    constexpr uint32_t INPUT_DEVICE_CLASS_TOUCH         = 0x00000004;

    /* The input device is a cursor device such as a trackball or mouse. */
    constexpr uint32_t INPUT_DEVICE_CLASS_CURSOR        = 0x00000008;

    /* The input device is a multi-touch touchscreen. */
    constexpr uint32_t INPUT_DEVICE_CLASS_TOUCH_MT      = 0x00000010;

    /* The input device is a directional pad (implies keyboard, has DPAD keys). */
    constexpr uint32_t INPUT_DEVICE_CLASS_DPAD          = 0x00000020;

    /* The input device is a gamepad (implies keyboard, has BUTTON keys). */
    constexpr uint32_t INPUT_DEVICE_CLASS_GAMEPAD       = 0x00000040;

    /* The input device has switches. */
    constexpr uint32_t INPUT_DEVICE_CLASS_SWITCH        = 0x00000080;

    /* The input device is a joystick (implies gamepad, has joystick absolute axes). */
    constexpr uint32_t INPUT_DEVICE_CLASS_JOYSTICK      = 0x00000100;

    /* The input device has a vibrator (supports FF_RUMBLE). */
    constexpr uint32_t INPUT_DEVICE_CLASS_VIBRATOR      = 0x00000200;

    /* The input device has a microphone. */
    constexpr uint32_t INPUT_DEVICE_CLASS_MIC           = 0x00000400;

    /* The input device is an external stylus (has data we want to fuse with touch data). */
    constexpr uint32_t INPUT_DEVICE_CLASS_EXTERNAL_STYLUS = 0x00000800;

    /* The input device has a rotary encoder. */
    constexpr uint32_t INPUT_DEVICE_CLASS_ROTARY_ENCODER = 0x00001000;

    /* The input device is virtual (not a real device, not part of UI configuration). */
    constexpr uint32_t INPUT_DEVICE_CLASS_VIRTUAL       = 0x40000000;

    /* The input device is external (not built-in). */
    constexpr uint32_t INPUT_DEVICE_CLASS_EXTERNAL      = 0x80000000;

    constexpr uint32_t MAX_SIZE_OF_DEVICE_NAME = UINPUT_MAX_NAME_SIZE - 1;

    const std::string DH_ID_PREFIX = "Input_";

    const std::string DINPUT_SPLIT_COMMA = ", ";

    const char VIR_NODE_SPLIT_CHAR = '|';
    const std::string VIR_NODE_SPLIT = "|";
    const std::string VIR_NODE_PID_SPLIT = "/";
    const uint32_t VIR_NODE_PHY_LEN = 3;
    const uint32_t VIR_NODE_PHY_DEVID_IDX = 1;
    const uint32_t VIR_NODE_PHY_DHID_IDX = 2;

    const std::string SOURCE_DEVICE_ID = "sourceDevId";

    const std::string SINK_DEVICE_ID = "sinkDevId";

    const std::string SOURCE_WINDOW_ID = "sourceWinId";

    const std::string SINK_SHOW_WINDOW_ID = "sinkShowWinId";

    const std::string SOURCE_WINDOW_WIDTH = "sourceWinWidth";

    const std::string SOURCE_WINDOW_HEIGHT = "sourceWinHeight";

    const std::string SINK_PROJECT_SHOW_WIDTH = "sinkProjShowWidth";

    const std::string SINK_PROJECT_SHOW_HEIGHT = "sinkProjShowHeight";

    const std::string SINK_WINDOW_SHOW_X = "sinkWinShowX";

    const std::string SINK_WINDOW_SHOW_Y = "sinkWinShowY";

    const std::string DEVICE_NAME = "name";

    const std::string PHYSICAL_PATH = "physicalPath";

    const std::string UNIQUE_ID = "uniqueId";

    const std::string BUS = "bus";

    const std::string VENDOR = "vendor";

    const std::string PRODUCT = "product";

    const std::string VERSION = "version";

    const std::string DESCRIPTOR = "descriptor";

    const std::string CLASSES = "classes";

    const std::string EVENT_TYPES = "eventTypes";

    const std::string EVENT_KEYS = "eventKeys";

    const std::string ABS_TYPES = "absTypes";

    const std::string ABS_INFOS = "absInfos";

    const std::string REL_TYPES = "relTypes";

    const std::string PROPERTIES = "properties";

    const std::string MISCELLANEOUS = "miscellaneous";

    const std::string LEDS = "leds";

    const std::string REPEATS = "repeats";

    const std::string SWITCHS = "switchs";

    const std::string DH_TOUCH_PAD = "touchpad";

    const std::string DINPUT_LOG_TITLE_TAG = "DINPUT";

    constexpr const char* LATENCY_COUNT_THREAD_NAME = "latencyCount";

    constexpr const char* EVENT_INJECT_THREAD_NAME = "eventInject";

    constexpr const char* COLLECT_EVENT_THREAD_NAME = "collectEvents";

    constexpr const char* CHECK_KEY_STATUS_THREAD_NAME = "checkKeyStatus";

    constexpr int32_t LOG_MAX_LEN = 4096;

    constexpr uint32_t SCREEN_ID_DEFAULT = 0;

    constexpr uint32_t DEFAULT_VALUE = 0;

    constexpr int32_t UN_INIT_FD_VALUE = -1;

    constexpr int32_t SINK_SCREEN_INFO_SIZE = 4;

    constexpr int32_t MAX_LOG_TIMES = 20;

    enum class EHandlerMsgType {
        DINPUT_SINK_EVENT_HANDLER_MSG = 1,
        DINPUT_SOURCE_EVENT_HANDLER_MSG = 2
    };

    struct BusinessEvent {
        std::vector<int32_t> pressedKeys;
        int32_t keyCode;
        int32_t keyAction;
    };

    struct TouchScreenEvent {
        uint32_t absX;
        uint32_t absY;
    };

    /*
     * A raw event as retrieved from the input_event.
     */
    struct RawEvent {
        int64_t when;
        uint32_t type;
        uint32_t code;
        int32_t value;
        std::string descriptor;
        std::string path;

        bool operator == (const RawEvent &e)
        {
            return this->type == e.type && this->code == e.code &&
                this->descriptor == e.descriptor && this->path == e.path;
        }
    };

    /*
     * Input device Info retrieved from the kernel.
     */
    struct InputDevice {
        inline InputDevice() : name(""), physicalPath(""), uniqueId(""), bus(0), vendor(0), product(0),
            version(0), descriptor(""), classes(0) {}
        std::string name;
        std::string physicalPath;
        std::string uniqueId;
        uint16_t bus;
        uint16_t vendor;
        uint16_t product;
        uint16_t version;
        std::string descriptor;
        uint32_t classes;
        std::vector<uint32_t> eventTypes;
        std::vector<uint32_t> eventKeys;
        std::vector<uint32_t> absTypes;
        std::map<uint32_t, std::vector<int32_t>> absInfos;
        std::vector<uint32_t> relTypes;
        std::vector<uint32_t> properties;

        std::vector<uint32_t> miscellaneous;
        std::vector<uint32_t> leds;
        std::vector<uint32_t> switchs;
        std::vector<uint32_t> repeats;
    };

    /*
     * Distributed Hardware Handle
     */
    struct HardwareHandle {
        // equipment ID
        std::string eqId;

        // Hardware ID
        std::string hhId;

        // Hardware detailed information
        std::string hdInfo;
    };

    // Synthetic raw event type codes produced when devices are added or removed.
    enum class DeviceType {
        // Sent when a device is added.
        DEVICE_ADDED = 0x10000000,

        // Sent when a device is removed.
        DEVICE_REMOVED = 0x20000000,

        // Sent when all added/removed devices from the most recent scan have been reported.
        // This event is always sent at least once.
        FINISHED_DEVICE_SCAN = 0x30000000,

        FIRST_SYNTHETIC_EVENT = DEVICE_ADDED,
    };

    /*
     * Input device connection status
     */
    struct InputDeviceEvent {
        DeviceType type;
        InputDevice deviceInfo;
    };

    /*
     * Device Type definitions
     */
    enum class DInputServerType {
        /*
         * null server
         */
        NULL_SERVER_TYPE = 0,

        /*
         * source server
         */
        SOURCE_SERVER_TYPE = 1,

        /*
         * sink server.
         */
        SINK_SERVER_TYPE = 2,
    };

    // Current Input Session Status
    enum class SessionStatus : uint32_t {
        CLOSED = 0x00,
        OPENING = 0x01,
        OPENED = 0x02,
        CLOSING = 0x03,
    };
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_INPUT_CONSTANTS_H
