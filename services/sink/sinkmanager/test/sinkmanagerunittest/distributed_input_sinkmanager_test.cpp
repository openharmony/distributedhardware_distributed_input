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

#include "distributed_input_sinkmanager_test.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "softbus_common.h"

#include "dinput_errcode.h"
#include "mock_process.h"

using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace std;
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
void DistributedInputSinkManagerTest::SetUp()
{
    sinkManager_ = new DistributedInputSinkManager(DISTRIBUTED_HARDWARE_INPUT_SINK_SA_ID, true);
    sinkManager_->Init();
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = OHOS_PERMISSION_DISTRIBUTED_SOFTBUS_CENTER;
    perms[1] = OHOS_PERMISSION_DISTRIBUTED_DATASYNC;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "dsoftbus_service",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DistributedInputSinkManagerTest::TearDown()
{
}

void DistributedInputSinkManagerTest::SetUpTestCase()
{
}

void DistributedInputSinkManagerTest::TearDownTestCase()
{
}

void DistributedInputSinkManagerTest::TestGetSinkScreenInfosCb::OnResult(const std::string &strJson)
{
    (void)strJson;
}

int32_t DistributedInputSinkManagerTest::TestSharingDhIdListenerStub::OnSharing(const std::string &dhId)
{
    (void)dhId;
    return DH_SUCCESS;
}

int32_t DistributedInputSinkManagerTest::TestSharingDhIdListenerStub::OnNoSharing(const std::string &dhId)
{
    (void)dhId;
    return DH_SUCCESS;
}

HWTEST_F(DistributedInputSinkManagerTest, InitAuto, testing::ext::TestSize.Level0)
{
    bool ret = sinkManager_->InitAuto();
    EXPECT_EQ(true, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, Init, testing::ext::TestSize.Level0)
{
    int32_t ret = sinkManager_->Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, Release, testing::ext::TestSize.Level0)
{
    MockProcess::MockDinputProcess("dinput");
    int32_t ret = sinkManager_->Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, GetStartTransFlag, testing::ext::TestSize.Level0)
{
    DInputServerType flag = DInputServerType::SINK_SERVER_TYPE;
    sinkManager_->SetStartTransFlag(flag);
    DInputServerType retFlag = sinkManager_->GetStartTransFlag();
    EXPECT_EQ(flag, retFlag);
}

HWTEST_F(DistributedInputSinkManagerTest, GetInputTypes, testing::ext::TestSize.Level0)
{
    uint32_t inputTypes = static_cast<uint32_t>(DInputDeviceType::MOUSE);
    sinkManager_->SetInputTypes(inputTypes);
    uint32_t retType = sinkManager_->GetInputTypes();
    EXPECT_EQ(inputTypes, retType);
}

HWTEST_F(DistributedInputSinkManagerTest, DeleteStopDhids01, testing::ext::TestSize.Level0)
{
    int32_t sessionId = 1;
    std::vector<std::string> stopDhIds;
    std::vector<std::string> stopIndeedDhIds;
    stopDhIds.push_back("Input_123123123123");
    for (auto iter : stopDhIds) {
        sinkManager_->sharingDhIds_.insert(iter);
    }
    sinkManager_->sharingDhIdsMap_[sessionId] = sinkManager_->sharingDhIds_;
    sinkManager_->DeleteStopDhids(100, stopDhIds, stopIndeedDhIds);
    sinkManager_->DeleteStopDhids(sessionId, stopDhIds, stopIndeedDhIds);
    EXPECT_EQ(0, sinkManager_->sharingDhIdsMap_.size());
}

HWTEST_F(DistributedInputSinkManagerTest, GetSinkScreenInfosCbackSize01, testing::ext::TestSize.Level0)
{
    uint32_t ret = sinkManager_->GetSinkScreenInfosCbackSize();
    EXPECT_EQ(0, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, RegisterGetSinkScreenInfosCallback_01, testing::ext::TestSize.Level1)
{
    sptr<TestGetSinkScreenInfosCb> callback(new TestGetSinkScreenInfosCb());
    int32_t ret = sinkManager_->RegisterGetSinkScreenInfosCallback(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, RegisterGetSinkScreenInfosCallback_02, testing::ext::TestSize.Level1)
{
    sptr<TestGetSinkScreenInfosCb> callback = nullptr;
    int32_t ret = sinkManager_->RegisterGetSinkScreenInfosCallback(callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, OnMessage_01, testing::ext::TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sinkManager_->Dump(fd, args);
    sinkManager_->OnStart();
    sinkManager_->OnStop();
    std::string message = "";
    sinkManager_->projectWindowListener_->OnMessage(DHTopic::TOPIC_START_DSCREEN, message);
    sinkManager_->projectWindowListener_->OnMessage(DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO, message);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, ParseMessage_01, testing::ext::TestSize.Level1)
{
    std::string message = "";
    std::string srcDeviceId = "";
    uint64_t srcWinId = 0;
    SinkScreenInfo sinkScreenInfo;
    int32_t ret = sinkManager_->projectWindowListener_->ParseMessage(message, srcDeviceId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, ParseMessage_02, testing::ext::TestSize.Level1)
{
    std::string srcDevId = "umkyu1b165e1be98151891erbe8r91ev";
    uint64_t srcWinId = 1;
    uint64_t sinkShowWinId = 1;
    uint32_t sinkShowWidth = 1860;
    uint32_t sinkShowHeigth = 980;
    uint32_t sinkShowX = 100;
    uint32_t sinkShowY = 100;
    nlohmann::json jsonObj;
    jsonObj[SOURCE_DEVICE_ID] = srcDevId;
    jsonObj[SOURCE_WINDOW_ID] = srcWinId;
    jsonObj[SINK_SHOW_WINDOW_ID] = sinkShowWinId;
    jsonObj[SINK_PROJECT_SHOW_WIDTH] = sinkShowWidth;
    jsonObj[SINK_PROJECT_SHOW_HEIGHT] = sinkShowHeigth;
    jsonObj[SINK_WINDOW_SHOW_X] = sinkShowX;
    jsonObj[SINK_WINDOW_SHOW_Y] = sinkShowY;
    SinkScreenInfo sinkScreenInfo;
    int32_t ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, ParseMessage_04, testing::ext::TestSize.Level1)
{
    std::string srcDevId = "umkyu1b165e1be98151891erbe8r91ev";
    SinkScreenInfo sinkScreenInfo;
    uint64_t srcWinId = 1;
    uint64_t sinkShowWinId = 1;
    uint32_t sinkShowWidth = 1860;
    uint32_t sinkShowHeigth = 980;
    uint32_t sinkShowX = 100;
    nlohmann::json jsonObj;
    jsonObj[SOURCE_DEVICE_ID] = srcWinId;
    int32_t ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SOURCE_DEVICE_ID] = srcDevId;
    jsonObj[SOURCE_WINDOW_ID] = srcDevId;
    ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SOURCE_WINDOW_ID] = srcWinId;
    jsonObj[SINK_SHOW_WINDOW_ID] = srcDevId;
    ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SINK_SHOW_WINDOW_ID] = sinkShowWinId;
    jsonObj[SINK_PROJECT_SHOW_WIDTH] = srcDevId;
    ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SINK_PROJECT_SHOW_WIDTH] = sinkShowWidth;
    jsonObj[SINK_PROJECT_SHOW_HEIGHT] = srcDevId;
    ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SINK_PROJECT_SHOW_HEIGHT] = sinkShowHeigth;
    jsonObj[SINK_WINDOW_SHOW_X] = srcDevId;
    ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);

    jsonObj[SINK_WINDOW_SHOW_X] = sinkShowX;
    jsonObj[SINK_WINDOW_SHOW_Y] = srcDevId;
    ret = sinkManager_->projectWindowListener_->ParseMessage(jsonObj.dump(),
        srcDevId, srcWinId, sinkScreenInfo);
    EXPECT_EQ(ERR_DH_INPUT_JSON_PARSE_FAIL, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, UpdateSinkScreenInfoCache_01, testing::ext::TestSize.Level1)
{
    std::string srcDevId = "umkyu1b165e1be98151891erbe8r91ev";
    uint64_t srcWinId = 1;
    SinkScreenInfo sinkScreenInfoTmp {2, 1860, 980, 200, 200};
    auto ret = sinkManager_->projectWindowListener_->UpdateSinkScreenInfoCache(srcDevId, srcWinId, sinkScreenInfoTmp);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, NotifyStopDScreen_01, testing::ext::TestSize.Level1)
{
    std::string srcScreenInfoKey  = "";
    int32_t ret = sinkManager_->NotifyStopDScreen(srcScreenInfoKey);
    EXPECT_EQ(ERR_DH_INPUT_SERVER_SINK_SCREEN_INFO_IS_EMPTY, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, NotifyStopDScreen_02, testing::ext::TestSize.Level1)
{
    std::string srcScreenInfoKey  = "srcScreenInfoKey_test";
    int32_t ret = sinkManager_->NotifyStopDScreen(srcScreenInfoKey);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, RegisterSharingDhIdListener_01, testing::ext::TestSize.Level1)
{
    sptr<TestSharingDhIdListenerStub> sharingDhIdListener = new TestSharingDhIdListenerStub();
    int32_t ret = sinkManager_->RegisterSharingDhIdListener(sharingDhIdListener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

HWTEST_F(DistributedInputSinkManagerTest, Dump_01, testing::ext::TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    int32_t ret = sinkManager_->Dump(fd, args);
    EXPECT_EQ(ERR_DH_INPUT_HIDUMP_DUMP_PROCESS_FAIL, ret);
}
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS