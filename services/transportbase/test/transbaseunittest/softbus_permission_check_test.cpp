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

#include "gtest/gtest.h"
#include <memory>
#include <unistd.h>

#include "device_manager_impl_mock.h"
#include "dinput_errcode.h"
#include "dinput_log.h"
#include "dinput_utils_tool.h"
#include "mock_other_method.h"
#include "socket_mock.h"
#include "softbus_permission_check.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
using namespace testing;
using namespace testing::ext;
using namespace std;

class SoftbusPermissionCheckTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() {};
    void TearDown() {};
public:
    static inline shared_ptr<DeviceOtherMethodMock> otherMethodMock_ = nullptr;
    static inline shared_ptr<SocketMock> socketMock_ = nullptr;
    static inline shared_ptr<DeviceManagerMock> deviceManagerMock_ = nullptr;
};

void SoftbusPermissionCheckTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase";
    otherMethodMock_ = make_shared<DeviceOtherMethodMock>();
    DeviceOtherMethodMock::otherMethod = otherMethodMock_;
    deviceManagerMock_ = make_shared<DeviceManagerMock>();
    DeviceManagerMock::deviceManager = deviceManagerMock_;
    socketMock_ = make_shared<SocketMock>();
    SocketMock::dSocket = socketMock_;
}

void SoftbusPermissionCheckTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase";
    SocketMock::dSocket = nullptr;
    socketMock_ = nullptr;
    DeviceManagerMock::deviceManager = nullptr;
    deviceManagerMock_ = nullptr;
    DeviceOtherMethodMock::otherMethod = nullptr;
    otherMethodMock_ = nullptr;
}

/**
 * @tc.name: SoftbusPermissionCheckTest_GetLocalNetworkId_001
 * @tc.desc: Verify the GetLocalNetworkId function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_GetLocalNetworkId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_GetLocalNetworkId_001 start";
    std::string networkId;
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_TRUE(SoftBusPermissionCheck::GetLocalNetworkId(networkId));

    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::GetLocalNetworkId(networkId));
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_GetLocalNetworkId_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_GetCurrentUserId_001
 * @tc.desc: Verify the GetCurrentUserId function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_GetCurrentUserId_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_GetCurrentUserId_001 start";
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(INVALID_USER_ID));
    auto userId = SoftBusPermissionCheck::GetCurrentUserId();
    EXPECT_EQ(userId, INVALID_USER_ID);

    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(DH_SUCCESS));
    userId = SoftBusPermissionCheck::GetCurrentUserId();
    EXPECT_EQ(userId, INVALID_USER_ID);

    std::vector<int32_t> userIds{100, 101};
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    userId = SoftBusPermissionCheck::GetCurrentUserId();
    EXPECT_EQ(userId, userIds[0]);
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_GetCurrentUserId_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_FillLocalInfo_001
 * @tc.desc: Verify the FillLocalInfo function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_FillLocalInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_FillLocalInfo_001 start";
    SocketAccessInfo *localInfoPtr = nullptr;
    EXPECT_FALSE(SoftBusPermissionCheck::FillLocalInfo(localInfoPtr));

    SocketAccessInfo localInfo;
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::FillLocalInfo(&localInfo));

    std::vector<int32_t> userIds{100, 101};
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_TRUE(SoftBusPermissionCheck::FillLocalInfo(&localInfo));
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_FillLocalInfo_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_TransCallerInfo_001
 * @tc.desc: Verify the TransCallerInfo function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_TransCallerInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_TransCallerInfo_001 start";
    SocketAccessInfo *localInfoPtr = nullptr;
    AccountInfo callerAccountInfo;
    std::string networkId;
    EXPECT_TRUE(SoftBusPermissionCheck::TransCallerInfo(localInfoPtr, callerAccountInfo, networkId));

    SocketAccessInfo localInfo;
    localInfo.extraAccessInfo = nullptr;
    EXPECT_TRUE(SoftBusPermissionCheck::TransCallerInfo(&localInfo, callerAccountInfo, networkId));

    localInfo.extraAccessInfo = const_cast<char*>("test");
    EXPECT_FALSE(SoftBusPermissionCheck::TransCallerInfo(&localInfo, callerAccountInfo, networkId));

    localInfo.extraAccessInfo = const_cast<char*>("{\"accountId\":1}");
    EXPECT_FALSE(SoftBusPermissionCheck::TransCallerInfo(&localInfo, callerAccountInfo, networkId));

    localInfo.extraAccessInfo = const_cast<char*>("{\"accountId\":\"test\"}");
    EXPECT_TRUE(SoftBusPermissionCheck::TransCallerInfo(&localInfo, callerAccountInfo, networkId));
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_TransCallerInfo_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_GetLocalAccountInfo_001
 * @tc.desc: Verify the GetLocalAccountInfo function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_GetLocalAccountInfo_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_GetLocalAccountInfo_001 start";
    AccountInfo localAccountInfo;
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::GetLocalAccountInfo(localAccountInfo));

    std::vector<int32_t> userIds{100, 101};
#ifdef SUPPORT_SAME_ACCOUNT
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    AccountSA::OhosAccountInfo osAccountInfo;
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(INVALID_USER_ID)));
    EXPECT_FALSE(SoftBusPermissionCheck::GetLocalAccountInfo(localAccountInfo));

    osAccountInfo.uid_ = ""; // accountinfo uid = ""
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_FALSE(SoftBusPermissionCheck::GetLocalAccountInfo(localAccountInfo));
#endif

    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
#ifdef SUPPORT_SAME_ACCOUNT
    osAccountInfo.uid_ = "test";
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
#endif
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::GetLocalAccountInfo(localAccountInfo));

    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
#ifdef SUPPORT_SAME_ACCOUNT
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
#endif
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_TRUE(SoftBusPermissionCheck::GetLocalAccountInfo(localAccountInfo));
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_GetLocalAccountInfo_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_SetAccessInfoToSocket_001
 * @tc.desc: Verify the SetAccessInfoToSocket function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_SetAccessInfoToSocket_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_SetAccessInfoToSocket_001 start";
    int32_t socketId = 0;
#ifdef SUPPORT_SAME_ACCOUNT
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::SetAccessInfoToSocket(socketId));

    std::vector<int32_t> userIds{100, 101};
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    AccountSA::OhosAccountInfo osAccountInfo;
    osAccountInfo.uid_ = "test";
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*socketMock_, SetAccessInfo(_, _)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::SetAccessInfoToSocket(socketId));

    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*socketMock_, SetAccessInfo(_, _)).WillOnce(Return(DH_SUCCESS));
    EXPECT_TRUE(SoftBusPermissionCheck::SetAccessInfoToSocket(socketId));
#else
    EXPECT_TRUE(SoftBusPermissionCheck::SetAccessInfoToSocket(socketId));
#endif
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_SetAccessInfoToSocket_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_CheckSrcIsSameAccount_001
 * @tc.desc: Verify the CheckSrcIsSameAccount function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_CheckSrcIsSameAccount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSrcIsSameAccount_001 start";
    std::string networkId;
    AccountInfo callerAccountInfo;
#ifdef SUPPORT_SAME_ACCOUNT
    EXPECT_CALL(*deviceManagerMock_, CheckSrcIsSameAccount(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSrcIsSameAccount(networkId, callerAccountInfo));

    EXPECT_CALL(*deviceManagerMock_, CheckSrcIsSameAccount(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSrcIsSameAccount(networkId, callerAccountInfo));
#else
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSrcIsSameAccount(networkId, callerAccountInfo));

#endif
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSrcIsSameAccount_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_CheckSinkIsSameAccount_001
 * @tc.desc: Verify the CheckSinkIsSameAccount function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_CheckSinkIsSameAccount_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSinkIsSameAccount_001 start";
    const AccountInfo callerAccountInfo;
    const AccountInfo calleeAccountInfo;
#ifdef SUPPORT_SAME_ACCOUNT
    EXPECT_CALL(*deviceManagerMock_, CheckSinkIsSameAccount(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSinkIsSameAccount(callerAccountInfo, calleeAccountInfo));

    EXPECT_CALL(*deviceManagerMock_, CheckSinkIsSameAccount(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSinkIsSameAccount(callerAccountInfo, calleeAccountInfo));
#else
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSinkIsSameAccount(callerAccountInfo, calleeAccountInfo));
#endif
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSinkIsSameAccount_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_CheckSrcAccessControl_001
 * @tc.desc: Verify the CheckSrcAccessControl function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_CheckSrcAccessControl_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSrcAccessControl_001 start";
    std::string networkId;
    AccountInfo localAccountInfo;
    EXPECT_CALL(*deviceManagerMock_, CheckSrcAccessControl(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSrcAccessControl(networkId, localAccountInfo));

    EXPECT_CALL(*deviceManagerMock_, CheckSrcAccessControl(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSrcAccessControl(networkId, localAccountInfo));
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSrcAccessControl_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_CheckSinkAccessControl_001
 * @tc.desc: Verify the CheckSinkAccessControl function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_CheckSinkAccessControl_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSinkAccessControl_001 start";
    const AccountInfo callerAccountInfo;
    const AccountInfo calleeAccountInfo;
    EXPECT_CALL(*deviceManagerMock_, CheckSinkAccessControl(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSinkAccessControl(callerAccountInfo, calleeAccountInfo));

    EXPECT_CALL(*deviceManagerMock_, CheckSinkAccessControl(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSinkAccessControl(callerAccountInfo, calleeAccountInfo));
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSinkAccessControl_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_CheckSrcPermission_001
 * @tc.desc: Verify the CheckSrcPermission function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_CheckSrcPermission_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSrcPermission_001 start";
    std::string networkId; // GetLocalAccount failed
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSrcPermission(networkId));

    std::vector<int32_t> userIds{100, 101};
#ifdef SUPPORT_SAME_ACCOUNT
    AccountSA::OhosAccountInfo osAccountInfo;
    osAccountInfo.uid_ = "test";
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*deviceManagerMock_, CheckSrcIsSameAccount(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSrcPermission(networkId));

    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*deviceManagerMock_, CheckSrcIsSameAccount(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSrcPermission(networkId));
#else
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSrcPermission(networkId));
#endif
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSrcPermission_001 end";
}

/**
 * @tc.name: SoftbusPermissionCheckTest_CheckSinkPermission_001
 * @tc.desc: Verify the CheckSinkPermission function.
 * @tc.type: FUNC
 * @tc.require: ICCZFE
 */
HWTEST_F(SoftbusPermissionCheckTest, SoftbusPermissionCheckTest_CheckSinkPermission_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSinkPermission_001 start";
    AccountInfo callerAccountInfo;
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_)).WillOnce(Return(INVALID_USER_ID));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSinkPermission(callerAccountInfo));

    std::vector<int32_t> userIds{100, 101};
#ifdef SUPPORT_SAME_ACCOUNT
    AccountSA::OhosAccountInfo osAccountInfo;
    osAccountInfo.uid_ = "test";
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*deviceManagerMock_, CheckSinkIsSameAccount(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSinkPermission(callerAccountInfo));

    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*otherMethodMock_, GetOhosAccountInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(osAccountInfo), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_CALL(*deviceManagerMock_, CheckSinkIsSameAccount(_, _)).WillOnce(Return(false));
    EXPECT_FALSE(SoftBusPermissionCheck::CheckSinkPermission(callerAccountInfo));
#else
    EXPECT_CALL(*otherMethodMock_, QueryActiveOsAccountIds(_))
        .WillOnce(DoAll(SetArgReferee<0>(userIds), Return(DH_SUCCESS)));
    EXPECT_CALL(*deviceManagerMock_, GetLocalDeviceInfo(_, _)).WillOnce(Return(0));
    EXPECT_TRUE(SoftBusPermissionCheck::CheckSinkPermission(callerAccountInfo));
#endif
    GTEST_LOG_(INFO) << "SoftbusPermissionCheckTest_CheckSinkPermission_001 end";
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS
