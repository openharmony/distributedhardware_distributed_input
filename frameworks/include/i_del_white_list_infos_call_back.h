/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef I_DEL_WHITE_LIST_INFOS_CALL_BACK_H
#define I_DEL_WHITE_LIST_INFOS_CALL_BACK_H

#include <string>

#include <iremote_broker.h>

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class IDelWhiteListInfosCallback : public IRemoteBroker {
public:
    virtual void OnResult(const std::string& deviceId) = 0;

    enum class Message {
        RESULT,
    };

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.DistributedHardware.DistributedInput.IDelWhiteListInfosCallback");
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // I_ADD_WHITE_LIST_INFOS_CALL_BACK_H
