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

#ifndef HIDUMP_HELPER_H
#define HIDUMP_HELPER_H

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>

#include <sys/epoll.h>
#include <sys/inotify.h>

#include "constants_dinput.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
enum class HiDumperFlag {
    GET_HELP = 0,
    GET_NODE_INFO,
    GET_SESSION_INFO,
};

struct NodeInfo {
    std::string devId = "";
    std::string virNodeName = "";
    std::string inputDhId = "";
};

struct SessionInfo {
    int32_t sesId = 0;
    std::string mySesName = "";
    std::string peerSesName = "";
    SessionStatus sessionState = SessionStatus::CLOSED;
};

class HiDumper {
DECLARE_SINGLE_INSTANCE_BASE(HiDumper);

public:
    bool HiDump(const std::vector<std::string> &args, std::string &result);
    void SaveNodeInfo(const std::string &deviceId, const std::string &nodeName, const std::string &dhId);
    void DeleteNodeInfo(const std::string &deviceId, const std::string &dhId);
    void CreateSessionInfo(const std::string &remoteDevId, const int32_t &sessionId, const std::string &mySessionName,
        const std::string &peerSessionName, const SessionStatus &sessionStatus);
    void SetSessionStatus(const std::string &remoteDevId, const SessionStatus &sessionStatus);
    void DeleteSessionInfo(const std::string &remoteDevId);
private:
    explicit HiDumper() = default;
    ~HiDumper() = default;
    int32_t ProcessDump(const std::string &args, std::string &result);
    int32_t GetAllNodeInfos(std::string &result);
    int32_t GetSessionInfo(std::string &result);
    int32_t ShowHelp(std::string &result);
private:
    std::vector<NodeInfo> nodeInfos_;
    std::mutex nodeMutex_;

    // the unordered_map's key is remoteDevId.
    std::unordered_map<std::string, SessionInfo> sessionInfos_;
    std::mutex sessionMutex_;
    std::mutex operationMutex_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // HIDUMP_HELPER_H