/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "device_manager_impl.h"
#include "device_manager_impl_mock.h"
#include "dm_constants.h"

#include "gtest/gtest.h"

namespace OHOS {
namespace DistributedHardware {
#ifndef NORMAL_MOCK
const std::string NETWORKID_ONE = "45656596896323231";
const std::string NETWORKID_TWO = "45656596896323232";
const std::string NETWORKID_THREE = "45656596896323233";
constexpr int32_t NETWORKTYPE_WITH_WIFI = 2;
constexpr int32_t NETWORKTYPE_NONE_WIFI = 4;
#endif

int32_t DeviceManagerImpl::InitDeviceManager(const std::string &pkgName, std::shared_ptr<DmInitCallback> dmInitCallback)
{
    return DeviceManagerBase::deviceManager->InitDeviceManager(pkgName, dmInitCallback);
}

int32_t DeviceManagerImpl::UnInitDeviceManager(const std::string &pkgName)
{
    return DeviceManagerBase::deviceManager->UnInitDeviceManager(pkgName);
}

int32_t DeviceManagerImpl::GetTrustedDeviceList(const std::string &pkgName,
                                                const std::string &extra,
                                                std::vector<DmDeviceInfo> &deviceList)
{
    return DeviceManagerBase::deviceManager->GetTrustedDeviceList(pkgName, extra, deviceList);
}

int32_t DeviceManagerImpl::GetLocalDeviceInfo(const std::string &pkgName, DmDeviceInfo &info)
{
    return DeviceManagerBase::deviceManager->GetLocalDeviceInfo(pkgName, info);
}

int32_t DeviceManagerImpl::RegisterDevStateCallback(const std::string &pkgName,
                                                    const std::string &extra,
                                                    std::shared_ptr<DeviceStateCallback> callback)
{
    return DeviceManagerBase::deviceManager->RegisterDevStateCallback(pkgName, extra, callback);
}

int32_t DeviceManagerImpl::UnRegisterDevStateCallback(const std::string &pkgName)
{
    return DeviceManagerBase::deviceManager->UnRegisterDevStateCallback(pkgName);
}

#ifdef NORMAL_MOCK
int32_t DeviceManagerImpl::GetNetworkTypeByNetworkId(const std::string &pkgName,
                                                     const std::string &netWorkId,
                                                     int32_t &netWorkType)
{
    return DeviceManagerBase::deviceManager->GetNetworkTypeByNetworkId(pkgName, netWorkId, netWorkType);
}
#else
int32_t DeviceManagerImpl::GetNetworkTypeByNetworkId(const std::string &pkgName,
                                                     const std::string &netWorkId,
                                                     int32_t &netWorkType)
{
    GTEST_LOG_(INFO) << "GetTrustedDeviceList start";
    if (netWorkId == NETWORKID_ONE) {
        return ERR_DM_INPUT_PARA_INVALID;
    }

    if (netWorkId == NETWORKID_TWO) {
        netWorkType = NETWORKTYPE_WITH_WIFI;
        return DM_OK;
    }

    netWorkType = NETWORKTYPE_NONE_WIFI;
    return DM_OK;
}
#endif

bool DeviceManagerImpl::CheckSrcAccessControl(const DmAccessCaller &caller, const DmAccessCallee &callee)
{
    return DeviceManagerBase::deviceManager->CheckSrcAccessControl(caller, callee);
}

bool DeviceManagerImpl::CheckSinkAccessControl(const DmAccessCaller &caller, const DmAccessCallee &callee)
{
    return DeviceManagerBase::deviceManager->CheckSinkAccessControl(caller, callee);
}

bool DeviceManagerImpl::CheckSrcIsSameAccount(const DmAccessCaller &caller, const DmAccessCallee &callee)
{
    return DeviceManagerBase::deviceManager->CheckSrcIsSameAccount(caller, callee);
}

bool DeviceManagerImpl::CheckSinkIsSameAccount(const DmAccessCaller &caller, const DmAccessCallee &callee)
{
    return DeviceManagerBase::deviceManager->CheckSinkIsSameAccount(caller, callee);
}
} // namespace DistributedHardware
} // namespace OHOS
