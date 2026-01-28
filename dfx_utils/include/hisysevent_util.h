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

#ifndef OHOS_DISTRIBUTED_INPUT_HISYSEVENT_UTILS_H
#define OHOS_DISTRIBUTED_INPUT_HISYSEVENT_UTILS_H

#include <cstring>

#include "dhfwk_single_instance.h"
#include "hisysevent.h"
#include "constants_dinput.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
const std::string DINPUT_INIT = "DINPUT_INIT";
const std::string DINPUT_REGISTER = "DINPUT_REGISTER";
const std::string DINPUT_PREPARE = "DINPUT_PREPARE";
const std::string DINPUT_START_USE = "DINPUT_START_USE";
const std::string DINPUT_STOP_USE = "DINPUT_STOP_USE";
const std::string DINPUT_UNPREPARE = "DINPUT_UNPREPARE";
const std::string DINPUT_UNREGISTER = "DINPUT_UNREGISTER";
const std::string DINPUT_EXIT = "DINPUT_EXIT";
const std::string DINPUT_INIT_FAIL = "DINPUT_INIT_FAIL";
const std::string DINPUT_REGISTER_FAIL = "DINPUT_REGISTER_FAIL";
const std::string DINPUT_OPT_FAIL = "DINPUT_OPT_FAIL";
const std::string DINPUT_UNREGISTER_FAIL = "DINPUT_UNREGISTER_FAIL";

class HisyseventUtil {
FWK_DECLARE_SINGLE_INSTANCE_BASE(HisyseventUtil);
public:
    void SysEventWriteBehavior(std::string status, std::string msg);
    void SysEventWriteBehavior(std::string status, std::string devId, std::string msg);
    void SysEventWriteBehavior(std::string status, std::string devId, std::string dhId, std::string msg);
    void SysEventWriteFault(std::string status, std::string msg);
    void SysEventWriteFault(std::string status, std::string devId, int32_t errorCode, std::string msg);
    void SysEventWriteFault(std::string status, std::string devId, std::string dhId, int32_t errorCode,
        std::string msg);

private:
    HisyseventUtil() = default;
    ~HisyseventUtil() = default;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_INPUT_HISYSEVENT_UTILS_H
