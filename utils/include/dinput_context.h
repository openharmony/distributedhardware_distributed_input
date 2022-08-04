/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_INPUT_CONTEXT_H
#define OHOS_DISTRIBUTED_INPUT_CONTEXT_H

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>

#include "single_instance.h"

#include "distributed_hardware_log.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
struct LocalAbsInfo {
    int32_t abs_x_min = 0;
    int32_t abs_x_max = 0;
    int32_t abs_y_min = 0;
    int32_t abs_y_max = 0;
    int32_t abs_pressure_min = 0;
    int32_t abs_pressure_max = 0;
    int32_t abs_mt_touch_major_min = 0;
    int32_t abs_mt_touch_major_max = 0;
    int32_t abs_mt_touch_minor_min = 0;
    int32_t abs_mt_touch_minor_max = 0;
    int32_t abs_mt_orientation_min = 0;
    int32_t abs_mt_orientation_max = 0;
    int32_t abs_mt_position_x_min = 0;
    int32_t abs_mt_position_x_max = 0;
    int32_t abs_mt_position_y_min = 0;
    int32_t abs_mt_position_y_max = 0;
    int32_t abs_mt_blob_id_min = 0;
    int32_t abs_mt_blob_id_max = 0;
    int32_t abs_mt_tracking_id_min = 0;
    int32_t abs_mt_tracking_id_max = 0;
    int32_t abs_mt_pressure_min = 0;
    int32_t abs_mt_pressure_max = 0;
};

struct LocalTouchScreenInfo {
    uint32_t sinkShowWidth = 0;
    uint32_t sinkShowHeight = 0;
    uint32_t sinkPhyWidth = 0;
    uint32_t sinkPhyHeight = 0;
    LocalAbsInfo localAbsInfo;
};

struct SrcScreenInfo {
    std::string devId = "";             // source device id
    std::string sourceWinId = "";       // source projection window id
    uint32_t sourceWinWidth = 0;        // source projection window width
    uint32_t sourceWinHeight = 0;       // source projection window height
    std::string sourcePhyId = "";       // source virtual screen driver id
    uint32_t sourcePhyFd = 0;           // source virtual screen driver fd
    uint32_t sourcePhyWidth = 0;        // source virtual screen driver width
    uint32_t sourcePhyHeight = 0;       // source virtual screen driver height
};

struct TransformInfo {
    uint32_t sinkWinPhyX = 0;           // projection area X coordinate in touch coordinate
    uint32_t sinkWinPhyY = 0;           // projection area Y coordinate in touch coordinate
    uint32_t sinkProjPhyWidth = 0;      // projection area width in touch coordinate
    uint32_t sinkProjPhyHeight = 0;     // projection area height in touch coordinate
    double coeffWidth = 0.0;            // sink width transform source coefficient
    double coeffHeight = 0.0;           // sink height transform source coefficient
};

struct SinkScreenInfo {
    uint32_t sinkShowWidth = 0;         // sink show width
    uint32_t sinkShowHeight = 0;        // sink show height
    uint32_t sinkPhyWidth = 0;          // sink touch screen width
    uint32_t sinkPhyHeight = 0;         // sink touch screen height
    std::string sinkShowWinId = "";     // sink show window id
    uint32_t sinkWinShowX = 0;          // sink show window x coordinate
    uint32_t sinkWinShowY = 0;          // sink show window y coordinate
    uint32_t sinkProjShowWidth = 0;     // sink show window width
    uint32_t sinkProjShowHeight = 0;    // sink show window height
    SrcScreenInfo srcScreenInfo;
    TransformInfo transformInfo;
};

class DInputContext {
DECLARE_SINGLE_INSTANCE_BASE(DInputContext);
public:
    std::string GetScreenInfoKey(const std::string &devId, const std::string &sourceWinId);
    int32_t RemoveSinkScreenInfo(const std::string &sourceWinId);
    int32_t UpdateSinkScreenInfo(const std::string &sourceWinId, const SinkScreenInfo &sinkScreenInfo);
    SinkScreenInfo GetSinkScreenInfo(const std::string &sourceWinId);

    int32_t RemoveSrcScreenInfo(const std::string &sourceWinId);
    int32_t UpdateSrcScreenInfo(const std::string &sourceWinId, const SrcScreenInfo &srcScreenInfo);
    SrcScreenInfo GetSrcScreenInfo(const std::string &sourceWinId);

    void SetLocalTouchScreenInfo(const LocalTouchScreenInfo &localTouchScreenInfo);
    LocalTouchScreenInfo GetLocalTouchScreenInfo();
private:
    int32_t CalculateTransformInfo(SinkScreenInfo &sinkScreenInfo);

private:
    DInputContext() = default;
    ~DInputContext();

    std::unordered_map<std::string, SinkScreenInfo> sinkScreenInfoMap_;
    std::unordered_map<std::string, SrcScreenInfo> srcScreenInfoMap_;
    LocalTouchScreenInfo localTouchScreenInfo_;
    std::mutex srcMapMutex_;
    std::mutex sinkMapMutex_;
    std::mutex localTouchScreenInfoMutex_;
};
} // namespace DistributedInput
} // namespace DistributedHardware
} // namespace OHOS

#endif // OHOS_DISTRIBUTED_INPUT_CONTEXT_H