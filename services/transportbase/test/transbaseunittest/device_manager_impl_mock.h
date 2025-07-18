/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef DISTRIBUTED_INPUT_DEVICE_MANAGER_MOCK_H
#define DISTRIBUTED_INPUT_DEVICE_MANAGER_MOCK_H

#include <memory>
#include <string>
#include <gmock/gmock.h>

#include "device_manager.h"
#include "softbus_bus_center.h"

namespace OHOS::DistributedHardware {
class DeviceManagerBase {
public:
    virtual ~DeviceManagerBase() = default;
public:
    virtual int32_t InitDeviceManager(const std::string &pkgName,
        std::shared_ptr<DmInitCallback> dmInitCallback) = 0;
    virtual int32_t UnInitDeviceManager(const std::string &pkgName) = 0;
    virtual int32_t GetTrustedDeviceList(const std::string &pkgName,
        const std::string &extra, std::vector<DmDeviceInfo> &deviceList) = 0;
    virtual int32_t RegisterDevStateCallback(const std::string &pkgName,
        const std::string &extra, std::shared_ptr<DeviceStateCallback> callback) = 0;
    virtual int32_t UnRegisterDevStateCallback(const std::string &pkgName) = 0;
    virtual int32_t GetLocalDeviceInfo(const std::string &pkgName, DmDeviceInfo &info) = 0;
    virtual bool CheckSrcAccessControl(const DmAccessCaller &caller, const DmAccessCallee &callee) = 0;
    virtual bool CheckSinkAccessControl(const DmAccessCaller &caller, const DmAccessCallee &callee) = 0;
    virtual bool CheckSrcIsSameAccount(const DmAccessCaller &caller, const DmAccessCallee &callee) = 0;
    virtual bool CheckSinkIsSameAccount(const DmAccessCaller &caller, const DmAccessCallee &callee) = 0;
    #ifdef NORMAL_MOCK
    virtual int32_t GetNetworkTypeByNetworkId(const std::string &pkgName, const std::string &netWorkId,
        int32_t &netWorkType) = 0;
    #endif
public:
    virtual int32_t GetLocalNodeDeviceInfo(const char *pkgName, NodeBasicInfo *info) = 0;
public:
    static inline std::shared_ptr<DeviceManagerBase> deviceManager = nullptr;
};

class DeviceManagerMock : public DeviceManagerBase {
public:
    MOCK_METHOD2(InitDeviceManager, int32_t(const std::string &pkgName,
        std::shared_ptr<DmInitCallback> dmInitCallback));
    MOCK_METHOD1(UnInitDeviceManager, int32_t(const std::string &pkgName));
    MOCK_METHOD3(GetTrustedDeviceList, int32_t(const std::string &pkgName,
        const std::string &extra, std::vector<DmDeviceInfo> &deviceList));
    MOCK_METHOD3(RegisterDevStateCallback, int32_t(const std::string &pkgName,
        const std::string &extra, std::shared_ptr<DeviceStateCallback> callback));
    MOCK_METHOD1(UnRegisterDevStateCallback, int32_t(const std::string &pkgName));
    MOCK_METHOD2(GetLocalNodeDeviceInfo, int32_t(const char *pkgName, NodeBasicInfo *info));
    MOCK_METHOD2(GetLocalDeviceInfo, int32_t(const std::string &pkgName, DmDeviceInfo &info));
    MOCK_METHOD2(CheckSrcAccessControl, bool(const DmAccessCaller &caller, const DmAccessCallee &callee));
    MOCK_METHOD2(CheckSinkAccessControl, bool(const DmAccessCaller &caller, const DmAccessCallee &callee));
    MOCK_METHOD2(CheckSrcIsSameAccount, bool(const DmAccessCaller &caller, const DmAccessCallee &callee));
    MOCK_METHOD2(CheckSinkIsSameAccount, bool(const DmAccessCaller &caller, const DmAccessCallee &callee));
    #ifdef NORMAL_MOCK
    MOCK_METHOD3(GetNetworkTypeByNetworkId, int32_t(const std::string &pkgName, const std::string &netWorkId,
        int32_t &netWorkType));
    #endif
};
}
#endif