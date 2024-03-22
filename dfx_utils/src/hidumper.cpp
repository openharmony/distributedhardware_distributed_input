/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "hidumper.h"

#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_softbus_define.h"
#include "dinput_utils_tool.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
IMPLEMENT_SINGLE_INSTANCE(HiDumper);
namespace {
    const std::string ARGS_HELP = "-h";
    const std::string ARGS_NODE_INFO = "-nodeinfo";
    const std::string ARGS_SESSION_INFO = "-sessioninfo";

    const std::map<std::string, HiDumperFlag> ARGS_MAP = {
        {ARGS_HELP, HiDumperFlag::GET_HELP},
        {ARGS_NODE_INFO, HiDumperFlag::GET_NODE_INFO},
        {ARGS_SESSION_INFO, HiDumperFlag::GET_SESSION_INFO},
    };

    const std::map<SessionStatus, std::string> SESSION_STATUS = {
        {SessionStatus::CLOSED, "closed"},
        {SessionStatus::OPENING, "opening"},
        {SessionStatus::OPENED, "opened"},
        {SessionStatus::CLOSING, "closing"},
    };
}

bool HiDumper::HiDump(const std::vector<std::string> &args, std::string &result)
{
    if (args.empty()) {
        DHLOGE("args is empty");
        return false;
    }

    result.clear();
    int32_t argsSize = static_cast<int32_t>(args.size());
    for (int32_t i = 0; i < argsSize; i++) {
        DHLOGI("HiDumper Dump args[%{public}d]: %{public}s.", i, args.at(i).c_str());
    }
    if (ProcessDump(args[0], result) != DH_SUCCESS) {
        return false;
    }
    return true;
}

int32_t HiDumper::ProcessDump(const std::string &args, std::string &result)
{
    DHLOGI("ProcessDump Dump.");
    int32_t ret = ERR_DH_INPUT_HIDUMP_INVALID_ARGS;
    result.clear();

    std::map<std::string, HiDumperFlag>::const_iterator operatorIter;
    {
        std::lock_guard<std::mutex> lock(operationMutex_);
        operatorIter = ARGS_MAP.find(args);
        if (operatorIter == ARGS_MAP.end()) {
            result.append("unknown command");
            DHLOGI("ProcessDump");
            return ret;
        }
    }

    HiDumperFlag hidumperFlag = operatorIter->second;
    switch (hidumperFlag) {
        case HiDumperFlag::GET_HELP: {
            ret = ShowHelp(result);
            break;
        }
        case HiDumperFlag::GET_NODE_INFO: {
            ret = GetAllNodeInfos(result);
            break;
        }
        case HiDumperFlag::GET_SESSION_INFO: {
            ret = GetSessionInfo(result);
            break;
        }
        default:
            break;
    }
    return ret;
}

int32_t HiDumper::GetAllNodeInfos(std::string &result)
{
    DHLOGI("GetAllNodeInfos Dump.");
    std::lock_guard<std::mutex> node_lock(nodeMutex_);
    for (auto iter = nodeInfos_.begin(); iter != nodeInfos_.end(); iter++) {
        result.append("\n{");
        result.append("\n   deviceid :   ");
        result.append(GetAnonyString((*iter).devId));
        result.append("\n   nodename :   ");
        result.append((*iter).virNodeName);
        result.append("\n   dhId :   ");
        result.append(GetAnonyString((*iter).inputDhId));
        result.append("\n},");
    }
    return DH_SUCCESS;
}

void HiDumper::DeleteNodeInfo(const std::string &deviceId, const std::string &dhId)
{
    DHLOGI("DeleteNodeInfo Dump.");
    std::lock_guard<std::mutex> node_lock(nodeMutex_);
    for (auto iter = nodeInfos_.begin(); iter != nodeInfos_.end();) {
        if ((*iter).devId.compare(deviceId) == 0 && (*iter).inputDhId.compare(dhId) == 0) {
            iter = nodeInfos_.erase(iter);
        } else {
            iter++;
        }
    }
}

int32_t HiDumper::GetSessionInfo(std::string &result)
{
    DHLOGI("GetSessionInfo Dump.");
    std::lock_guard<std::mutex> lock(sessionMutex_);
    for (auto iter = sessionInfos_.begin(); iter != sessionInfos_.end(); iter++) {
        result.append("\n{");
        result.append("\n   remotedevid :   ");
        result.append(GetAnonyString(iter->first));
        result.append("\n   sessionid :   ");
        result.append(std::to_string(iter->second.sesId));
        result.append("\n   mysessionname :   ");
        result.append(iter->second.mySesName);
        result.append("\n   peersessionname :   ");
        result.append(iter->second.peerSesName);

        std::string sessionStatus("");
        auto item = SESSION_STATUS.find(iter->second.sessionState);
        if (item == SESSION_STATUS.end()) {
            sessionStatus = "unknown state";
        } else {
            sessionStatus = SESSION_STATUS.find(iter->second.sessionState)->second;
        }
        result.append("\n   sessionstate :   ");
        result.append(sessionStatus);
        result.append("\n},");
    }
    return DH_SUCCESS;
}

void HiDumper::DeleteSessionInfo(const std::string &remoteDevId)
{
    DHLOGI("DeleteSessionInfo Dump.");
    std::lock_guard<std::mutex> session_lock(sessionMutex_);
    auto iter = sessionInfos_.find(remoteDevId);
    if (iter == sessionInfos_.end()) {
        DHLOGI("remote deviceid does not exist");
        return;
    } else {
        sessionInfos_.erase(iter);
    }
}

int32_t HiDumper::ShowHelp(std::string &result)
{
    DHLOGI("ShowHelp Dump.");
    result.append("Usage:dump  <command> [options]\n")
        .append("Description:\n")
        .append("-nodeinfo        ")
        .append("dump all input node information in the system\n")
        .append("-sessioninfo     ")
        .append("dump all input session information in the system\n");
    return DH_SUCCESS;
}

void HiDumper::SaveNodeInfo(const std::string &deviceId, const std::string &nodeName, const std::string &dhId)
{
    std::lock_guard<std::mutex> node_lock(nodeMutex_);
    NodeInfo nodeInfo = {
        .devId = deviceId,
        .virNodeName = nodeName,
        .inputDhId = dhId,
    };
    nodeInfos_.push_back(nodeInfo);
}

void HiDumper::CreateSessionInfo(const std::string &remoteDevId, const int32_t &sessionId,
    const std::string &mySessionName, const std::string &peerSessionName, const SessionStatus &sessionStatus)
{
    std::lock_guard<std::mutex> session_lock(sessionMutex_);
    auto iter = sessionInfos_.find(remoteDevId);
    if (iter == sessionInfos_.end()) {
        SessionInfo sessionInfo = {
            .sesId = sessionId,
            .mySesName = mySessionName,
            .peerSesName = peerSessionName,
            .sessionState = sessionStatus,
        };
        sessionInfos_[remoteDevId] = sessionInfo;
    }
}

void HiDumper::SetSessionStatus(const std::string &remoteDevId, const SessionStatus &sessionStatus)
{
    std::lock_guard<std::mutex> session_lock(sessionMutex_);
    auto iter = sessionInfos_.find(remoteDevId);
    if (iter == sessionInfos_.end()) {
        DHLOGI("remote deviceid does not exist");
        return;
    }
    sessionInfos_[remoteDevId].sessionState = sessionStatus;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

