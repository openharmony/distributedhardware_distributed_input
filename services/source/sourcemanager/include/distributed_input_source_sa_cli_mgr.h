/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_INPUT_SOURCE_SA_CLI_MGR_H
#define OHOS_DISTRIBUTED_INPUT_SOURCE_SA_CLI_MGR_H

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "event_handler.h"

#include "i_distributed_source_input.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DInputSourceSACliMgr {
DECLARE_SINGLE_INSTANCE_BASE(DInputSourceSACliMgr);
public:
    DInputSourceSACliMgr();
    virtual ~DInputSourceSACliMgr();

    sptr<IDistributedSourceInput> GetRemoteCli(const std::string &deviceId);

private:
    class RemoteCliDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };
    sptr<RemoteCliDeathRecipient> remoteCliDeathRcv;
    sptr<IDistributedSourceInput> GetRemoteCliFromCache(const std::string &devId);
    void AddRemoteCli(const std::string &devId, sptr<IRemoteObject> object);
    void DeleteRemoteCli(const std::string &devId);
    void DeleteRemoteCli(const sptr<IRemoteObject> remote);
    void ProcRemoteCliDied(const sptr<IRemoteObject> &remote);

private:
    std::mutex remoteCliLock;
    std::map<std::string, sptr<IDistributedSourceInput>> remoteCliMap;
    std::shared_ptr<AppExecFwk::EventHandler> dinputMgrHandler_;
};
}
}
}
#endif