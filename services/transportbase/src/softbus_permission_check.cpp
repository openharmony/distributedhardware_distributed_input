/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "softbus_permission_check.h"

#include "device_manager.h"
#include "dm_device_info.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "ipc_skeleton.h"
#include "ohos_account_kits.h"
#include "os_account_manager.h"
#include "transport/socket.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
constexpr const char *ACCOUNT_ID = "accountId";
static inline const std::string SERVICE_NAME { "ohos.dhardware.dinput" };
using namespace DistributedHardware;
bool SoftBusPermissionCheck::CheckSrcPermission(const std::string &sinkNetworkId)
{
    DHLOGI("Begin");
    AccountInfo localAccountInfo;
    if (!GetLocalAccountInfo(localAccountInfo)) {
        DHLOGE("Get os account data failed");
        return false;
    }
#ifdef SUPPORT_SAME_ACCOUNT
    if (!CheckSrcIsSameAccount(sinkNetworkId, localAccountInfo)) {
        DHLOGE("Check src same account failed");
        return false;
    }
#endif
    return true;
}

bool SoftBusPermissionCheck::CheckSinkPermission(const AccountInfo &callerAccountInfo)
{
    DHLOGI("Begin");
    AccountInfo localAccountInfo;
    if (!GetLocalAccountInfo(localAccountInfo)) {
        DHLOGE("Get local account info failed");
        return false;
    }
#ifdef SUPPORT_SAME_ACCOUNT
    if (!CheckSinkIsSameAccount(callerAccountInfo, localAccountInfo)) {
        DHLOGE("Check sink same account failed");
        return false;
    }
#endif
    return true;
}

bool SoftBusPermissionCheck::GetLocalAccountInfo(AccountInfo &localAccountInfo)
{
    int32_t userId = GetCurrentUserId();
    if (userId == INVALID_USER_ID) {
        DHLOGE("Get current userid failed");
        return false;
    }
    localAccountInfo.userId_ = userId;
#ifdef SUPPORT_SAME_ACCOUNT
    AccountSA::OhosAccountInfo osAccountInfo;
    int ret = AccountSA::OhosAccountKits::GetInstance().GetOhosAccountInfo(osAccountInfo);
    if (ret != 0 || osAccountInfo.uid_ == "") {
        DHLOGE("Get accountId from Ohos account info failed, ret: %{public}d.", ret);
        return false;
    }
    localAccountInfo.accountId_ = osAccountInfo.uid_;
#endif
    localAccountInfo.tokenId_ = IPCSkeleton::GetSelfTokenID();

    if (!GetLocalNetworkId(localAccountInfo.networkId_)) {
        DHLOGE("Get local networkid failed");
        return false;
    }
    DHLOGI("Get local accountinfo success, accountId %{public}s, userId %{public}s, networkId %{public}s.",
        GetAnonyString(localAccountInfo.accountId_).c_str(), std::to_string(localAccountInfo.userId_).c_str(),
        GetAnonyString(localAccountInfo.networkId_).c_str());
    return true;
}

bool SoftBusPermissionCheck::CheckSrcIsSameAccount(const std::string &sinkNetworkId,
    const AccountInfo &localAccountInfo)
{
#ifdef SUPPORT_SAME_ACCOUNT
    DmAccessCaller caller = {
        .accountId = localAccountInfo.accountId_,
        .networkId = localAccountInfo.networkId_,
        .userId = localAccountInfo.userId_,
    };
    DmAccessCallee callee = {
        .networkId = sinkNetworkId
    };
    if (!DeviceManager::GetInstance().CheckSrcIsSameAccount(caller, callee)) {
        DHLOGE("Check src same account failed");
        return false;
    }
#endif
    return true;
}

bool SoftBusPermissionCheck::CheckSinkIsSameAccount(const AccountInfo &callerAccountInfo,
    const AccountInfo &calleeAccountInfo)
{
#ifdef SUPPORT_SAME_ACCOUNT
    DmAccessCaller caller = {
        .accountId = callerAccountInfo.accountId_,
        .networkId = callerAccountInfo.networkId_,
        .userId = callerAccountInfo.userId_,
    };
    DmAccessCallee callee = {
        .accountId = calleeAccountInfo.accountId_,
        .networkId = calleeAccountInfo.networkId_,
        .userId = calleeAccountInfo.userId_,
    };
    if (!DeviceManager::GetInstance().CheckSinkIsSameAccount(caller, callee)) {
        DHLOGE("Check sink same account failed");
        return false;
    }
#endif
    return true;
}

bool SoftBusPermissionCheck::CheckSrcAccessControl(const std::string &sinkNetworkId,
    const AccountInfo &localAccountInfo)
{
    DmAccessCaller caller {
        .accountId = localAccountInfo.accountId_,
        .pkgName = SERVICE_NAME,
        .networkId = localAccountInfo.networkId_,
        .userId = localAccountInfo.userId_,
        .tokenId = localAccountInfo.tokenId_,
    };
    DmAccessCallee callee = {
        .networkId = sinkNetworkId,
    };
    if (!DeviceManager::GetInstance().CheckSrcAccessControl(caller, callee)) {
        DHLOGE("Check src acl failed");
        return false;
    }
    return true;
}

bool SoftBusPermissionCheck::CheckSinkAccessControl(const AccountInfo &callerAccountInfo,
    const AccountInfo &calleeAccountInfo)
{
    DmAccessCaller caller {
        .accountId = callerAccountInfo.accountId_,
        .pkgName = SERVICE_NAME,
        .networkId = callerAccountInfo.networkId_,
        .userId = callerAccountInfo.userId_,
        .tokenId = callerAccountInfo.tokenId_,
    };
    DmAccessCallee callee {
        .accountId = calleeAccountInfo.accountId_,
        .networkId = calleeAccountInfo.networkId_,
        .pkgName = SERVICE_NAME,
        .userId = calleeAccountInfo.userId_,
        .tokenId = calleeAccountInfo.tokenId_,
    };
    if (!DeviceManager::GetInstance().CheckSinkAccessControl(caller, callee)) {
        DHLOGE("Check sink acl failed");
        return false;
    }
    return true;
}

int32_t SoftBusPermissionCheck::GetCurrentUserId()
{
    std::vector<int32_t> userIds{};
    auto ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(userIds);
    if (ret != NO_ERROR || userIds.empty()) {
        DHLOGE("Query active os account id failed, ret = %{public}d", ret);
        return INVALID_USER_ID;
    }
    return userIds[0];
}

bool SoftBusPermissionCheck::GetLocalNetworkId(std::string &networkId)
{
    DistributedHardware::DmDeviceInfo localDeviceInfo{};
    int errCode = DeviceManager::GetInstance().GetLocalDeviceInfo(SERVICE_NAME, localDeviceInfo);
    if (errCode != 0) {
        DHLOGE("Get localdeviceInfo failed, ret = %{public}d", errCode);
        return false;
    }
    networkId = localDeviceInfo.networkId;
    return true;
}

bool SoftBusPermissionCheck::SetAccessInfoToSocket(const int32_t sessionId)
{
#ifdef SUPPORT_SAME_ACCOUNT
    AccountInfo accountInfo;
    if (!GetLocalAccountInfo(accountInfo)) {
        DHLOGE("GetOsAccountData failed.");
        return false;
    }
    nlohmann::json jsonObj;
    jsonObj[ACCOUNT_ID] = accountInfo.accountId_;

    SocketAccessInfo accessInfo;
    accessInfo.userId = accountInfo.userId_;
    std::string jsonStr = jsonObj.dump();
    size_t len = jsonStr.size();
    std::shared_ptr<char> charArr(new char[len + 1], [](char *p) {delete[] p;});
    if (strcpy_s(charArr.get(), len + 1, jsonStr.c_str()) != 0) {
        DHLOGE("strcpy_s failed.");
        return false;
    }
    accessInfo.extraAccessInfo = charArr.get();
    accessInfo.localTokenId = accountInfo.tokenId_;
    if (SetAccessInfo(sessionId, accessInfo) != 0) {
        DHLOGE("set access info failed");
        return false;
    }
#endif
    return true;
}

bool SoftBusPermissionCheck::TransCallerInfo(SocketAccessInfo *callerInfo,
    AccountInfo &callerAccountInfo, const std::string &networkId)
{
    if (callerInfo == nullptr || callerInfo->extraAccessInfo == nullptr) {
        DHLOGE("callerInfo or extraAccessInfo is nullptr.");
        return true;
    }
    nlohmann::json jsonObj = nlohmann::json::parse(std::string(callerInfo->extraAccessInfo), nullptr, false);
    if (jsonObj.is_discarded()) {
        DHLOGE("jsonObj parse failed.");
        return false;
    }
    if (!jsonObj.contains(ACCOUNT_ID) || !jsonObj[ACCOUNT_ID].is_string()) {
        DHLOGE("get ext jsonObj parse failed.");
        return false;
    }
    callerAccountInfo.accountId_ = jsonObj[ACCOUNT_ID].get<std::string>();
    callerAccountInfo.userId_ = callerInfo->userId;
    callerAccountInfo.networkId_ = networkId;
    DHLOGI("TransCallerInfo success");
    return true;
}

bool SoftBusPermissionCheck::FillLocalInfo(SocketAccessInfo *localInfo)
{
    if (localInfo == nullptr) {
        DHLOGE("localInfo is nullptr.");
        return false;
    }
    int32_t userId = GetCurrentUserId();
    if (userId == INVALID_USER_ID) {
        DHLOGE("get current user id falied");
        return false;
    }
    localInfo->userId = userId;
    localInfo->localTokenId = IPCSkeleton::GetSelfTokenID();
    return true;
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS