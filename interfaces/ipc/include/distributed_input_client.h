/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_INPUT_CLIENT_H
#define DISTRIBUTED_INPUT_CLIENT_H

#include <atomic>
#include <iostream>
#include <mutex>
#include <set>
#include <string>

#include "event_handler.h"

#include "add_white_list_infos_call_back_stub.h"
#include "del_white_list_infos_call_back_stub.h"
#include "get_sink_screen_infos_call_back_stub.h"
#include "i_distributed_source_input.h"
#include "i_distributed_sink_input.h"
#include "i_sharing_dhid_listener.h"
#include "register_d_input_call_back_stub.h"
#include "unregister_d_input_call_back_stub.h"
#include "sharing_dhid_listener_stub.h"
#include "start_stop_d_inputs_call_back_stub.h"

#include "dinput_sa_manager.h"
#include "idistributed_hardware_source.h"
#include "idistributed_hardware_sink.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class DistributedInputClient {
public:
    DistributedInputClient();
    ~DistributedInputClient(){};

    static DistributedInputClient &GetInstance();

    int32_t InitSource();

    int32_t ReleaseSource();

    int32_t InitSink();

    int32_t ReleaseSink();

    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &parameters, const std::shared_ptr<RegisterCallback> &callback);

    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::shared_ptr<UnregisterCallback> &callback);

    int32_t PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> callback);

    int32_t UnprepareRemoteInput(const std::string &deviceId, sptr<IUnprepareDInputCallback> callback);

    int32_t StartRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes, sptr<IStartDInputCallback> callback);

    int32_t StopRemoteInput(
        const std::string &deviceId, const uint32_t &inputTypes, sptr<IStopDInputCallback> callback);

    int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStartDInputCallback> callback);

    int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes,
        sptr<IStopDInputCallback> callback);

    int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IPrepareDInputCallback> callback);
    int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId,
        sptr<IUnprepareDInputCallback> callback);

    int32_t StartRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback);
    int32_t StopRemoteInput(const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback);

    int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback);
    int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const std::vector<std::string> &dhIds,
        sptr<IStartStopDInputsCallback> callback);

    bool IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event);
    bool IsTouchEventNeedFilterOut(const TouchScreenEvent &event);

    bool IsStartDistributedInput(const std::string &dhId);

    int32_t NotifyStartDScreen(const std::string &networkId, const std::string &srcDevId, const uint64_t srcWinId);
    int32_t NotifyStopDScreen(const std::string &networkId, const std::string &srcScreenInfoKey);

    int32_t RegisterSimulationEventListener(sptr<ISimulationEventListener> listener);
    int32_t UnregisterSimulationEventListener(sptr<ISimulationEventListener> listener);

    int32_t RegisterSessionStateCb(sptr<ISessionStateCallback> callback);
    int32_t UnregisterSessionStateCb();

    void CheckSourceRegisterCallback();
    void CheckWhiteListCallback();
    void CheckNodeMonitorCallback();
    void CheckKeyStateCallback();

    void CheckSinkRegisterCallback();
    void CheckSharingDhIdsCallback();
    void CheckSinkScreenInfoCallback();

public:
    class RegisterDInputCb : public OHOS::DistributedHardware::DistributedInput::RegisterDInputCallbackStub {
    public:
        RegisterDInputCb() = default;
        ~RegisterDInputCb() override = default;
        void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status) override;
    };

    class UnregisterDInputCb : public OHOS::DistributedHardware::DistributedInput::UnregisterDInputCallbackStub {
    public:
        UnregisterDInputCb() = default;
        ~UnregisterDInputCb() override = default;
        void OnResult(const std::string &devId, const std::string &dhId, const int32_t &status) override;
    };

    class AddWhiteListInfosCb : public OHOS::DistributedHardware::DistributedInput::AddWhiteListInfosCallbackStub {
    public:
        AddWhiteListInfosCb() = default;
        ~AddWhiteListInfosCb() override = default;
        void OnResult(const std::string &deviceId, const std::string &strJson) override;
    };

    class DelWhiteListInfosCb : public OHOS::DistributedHardware::DistributedInput::DelWhiteListInfosCallbackStub {
    public:
        DelWhiteListInfosCb() = default;
        ~DelWhiteListInfosCb() override = default;
        void OnResult(const std::string &deviceId) override;
    };

    class SharingDhIdListenerCb : public OHOS::DistributedHardware::DistributedInput::SharingDhIdListenerStub {
    public:
        SharingDhIdListenerCb() = default;
        ~SharingDhIdListenerCb() override = default;
        int32_t OnSharing(const std::string &dhId) override;
        int32_t OnNoSharing(const std::string &dhId) override;
    };

    class GetSinkScreenInfosCb : public OHOS::DistributedHardware::DistributedInput::GetSinkScreenInfosCallbackStub {
    public:
        GetSinkScreenInfosCb() = default;
        ~GetSinkScreenInfosCb() override = default;
        void OnResult(const std::string &strJson) override;
    };

    class DInputClientEventHandler : public AppExecFwk::EventHandler {
    public:
        explicit DInputClientEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
        ~DInputClientEventHandler() override = default;

        void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    };

private:
    bool IsJsonData(std::string strData) const;
    void AddWhiteListInfos(const std::string &deviceId, const std::string &strJson) const;
    void DelWhiteListInfos(const std::string &deviceId) const;
    void UpdateSinkScreenInfos(const std::string &strJson);
    sptr<IDistributedSinkInput> GetRemoteDInput(const std::string &networkId) const;

private:
    static std::shared_ptr<DistributedInputClient> instance;

    DInputServerType serverType = DInputServerType::NULL_SERVER_TYPE;
    DInputDeviceType inputTypes_ = DInputDeviceType::NONE;

    std::set<sptr<AddWhiteListInfosCb>> addWhiteListCallbacks_;
    std::set<sptr<DelWhiteListInfosCb>> delWhiteListCallbacks_;
    sptr<InputNodeListener> regNodeListener_ = nullptr;
    sptr<InputNodeListener> unregNodeListener_ = nullptr;
    sptr<ISimulationEventListener> regSimulationEventListener_ = nullptr;
    sptr<ISimulationEventListener> unregSimulationEventListener_ = nullptr;
    std::set<sptr<ISharingDhIdListener>> sharingDhIdListeners_;
    std::set<sptr<GetSinkScreenInfosCb>> getSinkScreenInfosCallbacks_;

    std::shared_ptr<DistributedInputClient::DInputClientEventHandler> eventHandler_;

    std::atomic<bool> isAddWhiteListCbReg;
    std::atomic<bool> isDelWhiteListCbReg;
    std::atomic<bool> isNodeMonitorCbReg;
    std::atomic<bool> isSimulationEventCbReg;
    std::atomic<bool> isSharingDhIdsReg;
    std::atomic<bool> isGetSinkScreenInfosCbReg;

    struct DHardWareFwkRegistInfo {
        std::string devId;
        std::string dhId;
        std::shared_ptr<RegisterCallback> callback = nullptr;
    };

    struct DHardWareFwkUnRegistInfo {
        std::string devId;
        std::string dhId;
        std::shared_ptr<UnregisterCallback> callback = nullptr;
    };

    std::vector<DHardWareFwkRegistInfo> dHardWareFwkRstInfos;
    std::vector<DHardWareFwkUnRegistInfo> dHardWareFwkUnRstInfos;
    std::vector<TransformInfo> screenTransInfos;
    std::mutex operationMutex_;

    std::mutex sharingDhIdsMtx_;
    // sharing local dhids
    std::set<std::string> sharingDhIds_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // DISTRIBUTED_INPUT_CLIENT_H
