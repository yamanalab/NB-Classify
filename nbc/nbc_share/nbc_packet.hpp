/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NBC_PACKET_HPP
#define NBC_PACKET_HPP

#include <cstdint>

namespace nbc_share
{

/**
 * @brief Enumeration for control code of packet.
 */
enum ControlCode_t : uint64_t
{
    /* Code for Request packet: 0x0201-0x02FF */

    /* Code for Data packet: 0x0401-0x04FF */
    kControlCodeDataPubkey        = 0x0401,
    kControlCodeDataModel         = 0x0402,
    kControlCodeDataInput         = 0x0403,
    kControlCodeDataContext       = 0x0404,
    kControlCodeDataSessionID     = 0x0405,
    kControlCodeDataPermVec       = 0x0406,
    kControlCodeDataCompute       = 0x0407,
    kControlCodeDataComputeAck    = 0x0408,
    kControlCodeDataBeginComputation = 0x0409,
    kControlCodeDataEndComputation = 0x0410,
    kControlCodeDataResultIndex = 0x0411,

    /* Code for Download packet: 0x0801-0x08FF */
    kControlCodeDownloadPubkey    = 0x0801,
    kControlCodeDownloadSessionID = 0x0802,
    kControlCodeDownloadContext   = 0x0803,
    
    /* Code for UpDownload packet: 0x1000-0x10FF */
    kControlCodeGroupUpDownload   = 0x1000,
    kControlCodeUpDownloadComputeData = 0x1001,
    kControlCodeUpDownloadResult = 0x1002,
};

} /* namespace nbc_share */

#endif /* NBC_PACKET_HPP */
