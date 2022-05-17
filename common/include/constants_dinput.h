/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <string>
#include <vector>
#include <map>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
#define INPUT_KEY_WHEN  "when"
#define INPUT_KEY_TYPE  "type"
#define INPUT_KEY_CODE  "code"
#define INPUT_KEY_VALUE "value"
#define INPUT_KEY_DESCRIPTOR  "descriptor"

#define VIRTUAL_DEVICE_NAME "Hos Distributed Virtual Device "

    /**
     * Status code.
     */
    const int32_t NO_ERROR = 0;
    const int32_t ERROR = -1;

    /**
     * Status code, indicates general success.
     */
    const int32_t SUCCESS = 0;

    /**
     * Status code, indicates general failure.
     */
    const int32_t FAILURE = -60000;

    /**
     * Status code, DISERVER general failure.
     */
    const int32_t FAILURE_DIS = -60001;

    /**
     * Status code, hardware is resigtring.
     */
    const int32_t FAILURE_REGISTING = -60002;

    /**
     * Status code, hardware is unresigtring.
     */
    const int32_t FAILURE_UNREGISTING = -60003;

    /**
     * Input Type NULL.
     */
    const int32_t INPUT_TYPE_NULL = 0;

    /**
     * Input Type MOUSE.
     */
    const int32_t INPUT_TYPE_MOUSE = 1;

    /**
     * Input Type KEYBOARD.
     */
    const int32_t INPUT_TYPE_KEYBOARD = 2;

    /**
     * Input Type TOUCH.
     */
    const int32_t INPUT_TYPE_TOUCH = 4;

    /**
     * Input Type ALL.
     */
    const int32_t INPUT_TYPE_ALL = INPUT_TYPE_MOUSE | INPUT_TYPE_KEYBOARD | INPUT_TYPE_TOUCH;

    constexpr int32_t INPUT_LOADSA_TIMEOUT_MS = 10000;

    enum class EHandlerMsgType {
        DINPUT_SINK_EVENT_HANDLER_MSG = 1,
        DINPUT_SOURCE_EVENT_HANDLER_MSG = 2
    };

    struct BusinessEvent {
        std::vector<int32_t> pressedKeys;
        int32_t keyCode;
        int32_t keyAction;
    };

    /*
     * A raw event as retrieved from the input_event.
     */
    struct RawEvent {
        int64_t when;
        int32_t type;
        int32_t code;
        int32_t value;
        std::string descriptor;
    };

    /*
     * Input device Info retrieved from the kernel.
     */
    struct InputDevice {
        inline InputDevice() : bus(0), vendor(0), product(0), version(0) {}
        std::string name;
        std::string location;
        std::string uniqueId;
        uint16_t bus;
        uint16_t vendor;
        uint16_t product;
        uint16_t version;
        std::string descriptor;
        uint16_t nonce;
        uint32_t classes;
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
    * Input device classes.
    */
    enum class DeviceClasses {
        /* The input device is a keyboard or has buttons. */
        INPUT_DEVICE_CLASS_KEYBOARD      = 0x00000001,

        /* The input device is an alpha-numeric keyboard (not just a dial pad). */
        INPUT_DEVICE_CLASS_ALPHAKEY      = 0x00000002,

        /* The input device is a touchscreen or a touchpad (either single-touch or multi-touch). */
        INPUT_DEVICE_CLASS_TOUCH         = 0x00000004,

        /* The input device is a cursor device such as a trackball or mouse. */
        INPUT_DEVICE_CLASS_CURSOR        = 0x00000008,

        /* The input device is a multi-touch touchscreen. */
        INPUT_DEVICE_CLASS_TOUCH_MT      = 0x00000010,

        /* The input device is a directional pad (implies keyboard, has DPAD keys). */
        INPUT_DEVICE_CLASS_DPAD          = 0x00000020,

        /* The input device is a gamepad (implies keyboard, has BUTTON keys). */
        INPUT_DEVICE_CLASS_GAMEPAD       = 0x00000040,

        /* The input device has switches. */
        INPUT_DEVICE_CLASS_SWITCH        = 0x00000080,

        /* The input device is a joystick (implies gamepad, has joystick absolute axes). */
        INPUT_DEVICE_CLASS_JOYSTICK      = 0x00000100,

        /* The input device has a vibrator (supports FF_RUMBLE). */
        INPUT_DEVICE_CLASS_VIBRATOR      = 0x00000200,

        /* The input device has a microphone. */
        INPUT_DEVICE_CLASS_MIC           = 0x00000400,

        /* The input device is an external stylus (has data we want to fuse with touch data). */
        INPUT_DEVICE_CLASS_EXTERNAL_STYLUS = 0x00000800,

        /* The input device has a rotary encoder. */
        INPUT_DEVICE_CLASS_ROTARY_ENCODER = 0x00001000,

        /* The input device is virtual (not a real device, not part of UI configuration). */
        INPUT_DEVICE_CLASS_VIRTUAL       = 0x40000000,

        /* The input device is external (not built-in). */
        INPUT_DEVICE_CLASS_EXTERNAL      = 0x80000000,
    };

    /**
     * Device Type definitions
     */
    enum class DInputServerType {
        /**
         * null server
         */
        NULL_SERVER_TYPE = 0,

        /**
         * source server
         */
        SOURCE_SERVER_TYPE = 1,

        /**
         * sink server.
         */
        SINK_SERVER_TYPE = 2,
    };
}
}
}
#endif