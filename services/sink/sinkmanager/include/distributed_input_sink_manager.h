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

#ifndef DISTRIBUTED_INPUT_SINK_MANAGER_SERVICE_H
#define DISTRIBUTED_INPUT_SINK_MANAGER_SERVICE_H

#include <cstring>
#include <set>
#include <map>
#include <mutex>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "event_handler.h"
#include "ipublisher_listener.h"
#include "publisher_listener_stub.h"
#include "nlohmann/json.hpp"
#include "screen.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_status_change_stub.h"

#include "constants_dinput.h"
#include "dinput_sink_manager_callback.h"
#include "dinput_sink_trans_callback.h"
#include "distributed_input_sink_stub.h"
#include "distributed_input_sink_event_handler.h"
#include "dinput_sink_state.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
enum class ServiceSinkRunningState { STATE_NOT_START, STATE_RUNNING };
const std::string EVENT_HANDLER_TASKNAME_START_TYPE = "start_type_handle_task";
const std::string EVENT_HANDLER_TASKNAME_START_DHID = "start_dhid_handle_task";

class DistributedInputSinkManager : public SystemAbility, public DistributedInputSinkStub {
    DECLARE_SYSTEM_ABILITY(DistributedInputSinkManager)

public:
    DistributedInputSinkManager(int32_t saId, bool runOnCreate);
    ~DistributedInputSinkManager() override;

    class DInputSinkMgrListener : public DInputSinkManagerCallback {
    public:
        explicit DInputSinkMgrListener(DistributedInputSinkManager *manager);
        virtual ~DInputSinkMgrListener();
        void ResetSinkMgrResStatus() override;
    private:
        DistributedInputSinkManager *sinkManagerObj_;
    };

    class DInputSinkListener : public DInputSinkTransCallback {
    public:
        explicit DInputSinkListener(DistributedInputSinkManager *manager);
        ~DInputSinkListener() override;
        void OnPrepareRemoteInput(const int32_t &sessionId, const std::string &deviceId) override;
        void OnUnprepareRemoteInput(const int32_t &sessionId) override;
        void OnStartRemoteInput(const int32_t &sessionId, const uint32_t &inputTypes) override;
        void OnStopRemoteInput(const int32_t &sessionId, const uint32_t &inputTypes) override;
        void OnStartRemoteInputDhid(const int32_t &sessionId, const std::string &strDhids) override;
        void OnStopRemoteInputDhid(const int32_t &sessionId, const std::string &strDhids) override;

        void OnRelayPrepareRemoteInput(const int32_t &toSrcSessionId, const int32_t &toSinkSessionId,
            const std::string &deviceId) override;
        void OnRelayUnprepareRemoteInput(const int32_t &toSrcSessionId, const int32_t &toSinkSessionId,
            const std::string &deviceId) override;
        void OnRelayStartDhidRemoteInput(const int32_t &toSrcSessionId, const int32_t &toSinkSessionId,
            const std::string &deviceId, const std::string &strDhids) override;
        void OnRelayStopDhidRemoteInput(const int32_t &toSrcSessionId, const int32_t &toSinkSessionId,
            const std::string &deviceId, const std::string &strDhids) override;
        void OnRelayStartTypeRemoteInput(const int32_t &toSrcSessionId, const int32_t &toSinkSessionId,
            const std::string &deviceId, uint32_t inputTypes) override;
        void OnRelayStopTypeRemoteInput(const int32_t &toSrcSessionId, const int32_t &toSinkSessionId,
            const std::string &deviceId, uint32_t inputTypes) override;

    private:
        DistributedInputSinkManager *sinkManagerObj_;
    };

    class ProjectWindowListener : public PublisherListenerStub {
    public:
        explicit ProjectWindowListener(DistributedInputSinkManager *manager);
        ~ProjectWindowListener() override;
        void OnMessage(const DHTopic topic, const std::string &message) override;

    private:
        int32_t ParseMessage(const std::string &message, std::string &srcDeviceId, uint64_t &srcWinId,
            SinkScreenInfo &sinkScreenInfo);
        int32_t UpdateSinkScreenInfoCache(const std::string &srcDevId, const uint64_t srcWinId,
            const SinkScreenInfo &sinkScreenInfoTmp);
        uint32_t GetScreenWidth();
        uint32_t GetScreenHeight();

    private:
        sptr<Rosen::Screen> screen_;
        std::mutex handleScreenMutex_;
        DistributedInputSinkManager *sinkManagerObj_;
    };

    class PluginStartListener : public PublisherListenerStub {
    public:
        explicit PluginStartListener() = default;
        ~PluginStartListener() override;
        void OnMessage(const DHTopic topic, const std::string &message) override;
    };

    class DScreenSinkSvrRecipient : public IRemoteObject::DeathRecipient {
    public:
        DScreenSinkSvrRecipient(const std::string &srcDevId, const uint64_t srcWinId);
        ~DScreenSinkSvrRecipient() override;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

    private:
        std::string srcDevId_;
        uint64_t srcWinId_;
    };

public:
    void OnStart() override;

    void OnStop() override;

    int32_t Init() override;

    int32_t Release() override;

    int32_t RegisterGetSinkScreenInfosCallback(sptr<IGetSinkScreenInfosCallback> callback) override;

    uint32_t GetSinkScreenInfosCbackSize();

    DInputServerType GetStartTransFlag();

    void SetStartTransFlag(const DInputServerType flag);

    uint32_t GetInputTypes();

    void SetInputTypes(const uint32_t &inputTypes);

    /*
     * GetEventHandler, get the ui_service manager service's handler.
     *
     * @return Returns EventHandler ptr.
     */
    std::shared_ptr<DistributedInputSinkEventHandler> GetEventHandler();

    int32_t NotifyStartDScreen(const SrcScreenInfo &srcScreenInfo) override;

    int32_t NotifyStopDScreen(const std::string &srcScreenInfoKey) override;

    int32_t RegisterSharingDhIdListener(sptr<ISharingDhIdListener> sharingDhIdListener) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

    void QueryLocalWhiteList(nlohmann::json &jsonStr);

    void ClearResourcesStatus();

private:
    void CleanExceptionalInfo(const SrcScreenInfo &srcScreenInfo);
    void CallBackScreenInfoChange();

private:
    ServiceSinkRunningState serviceRunningState_ = ServiceSinkRunningState::STATE_NOT_START;
    DInputServerType isStartTrans_ = DInputServerType::NULL_SERVER_TYPE;
    std::shared_ptr<DistributedInputSinkManager::DInputSinkListener> statuslistener_;
    std::shared_ptr<DistributedInputSinkManager::DInputSinkMgrListener> sinkMgrListener_;
    std::set<sptr<IGetSinkScreenInfosCallback>> getSinkScreenInfosCallbacks_;

    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<DistributedInputSinkEventHandler> handler_;
    std::mutex mutex_;
    bool InitAuto();
    DInputDeviceType inputTypes_;
    sptr<ProjectWindowListener> projectWindowListener_ = nullptr;
    sptr<PluginStartListener> pluginStartListener_ = nullptr;
    std::set<std::string> sharingDhIds_;
    std::map<int32_t, std::set<std::string>> sharingDhIdsMap_;
    void StoreStartDhids(int32_t sessionId, const std::vector<std::string> &dhIds);

    /*
     * Stop dhids on cmd,
     * stoDhIds: dhIds on cmd
     * stopIndeedDhIds: dhId that need stop capture event.
     */
    void DeleteStopDhids(int32_t sessionId, const std::vector<std::string> stopDhIds,
        std::vector<std::string> &stopIndeedDhIds);
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_SINK_MANAGER_SERVICE_H
