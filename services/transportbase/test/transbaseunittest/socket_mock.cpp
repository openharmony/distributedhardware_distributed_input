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

#include "socket_mock.h"

#include "inner_socket.h"

int32_t Socket(SocketInfo info)
{
    return OHOS::DistributedHardware::DSocket::dSocket->Socket(info);
}

int32_t Listen(int32_t socket, const QosTV qos[], uint32_t qosCount, const ISocketListener *listener)
{
    return OHOS::DistributedHardware::DSocket::dSocket->Listen(socket, qos, qosCount, listener);
}

int32_t Bind(int32_t socket, const QosTV qos[], uint32_t qosCount, const ISocketListener *listener)
{
    return OHOS::DistributedHardware::DSocket::dSocket->Bind(socket, qos, qosCount, listener);
}

void Shutdown(int32_t socket)
{
    (void)socket;
}

int SendFile(int32_t socket, const char *sFileList[], const char *dFileList[], uint32_t fileCnt)
{
    return OHOS::DistributedHardware::DSocket::dSocket->SendFile(socket, sFileList, dFileList, fileCnt);
}

int32_t SetAccessInfo(int32_t socket, SocketAccessInfo accessInfo)
{
    return OHOS::DistributedHardware::DSocket::dSocket->SetAccessInfo(socket, accessInfo);
}