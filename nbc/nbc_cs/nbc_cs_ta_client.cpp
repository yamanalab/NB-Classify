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

#include <memory>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_define.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_cs/nbc_cs_ta_client.hpp>

namespace nbc_cs
{
    
struct TAClient::Impl
{
    Impl(stdsc::Client& client)
        : client_(client)
    {}

    ~Impl(void)
    {}

    void get_session_id(int32_t& session_id)
    {
        stdsc::Buffer buffer;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadSessionID, buffer);
        session_id = *reinterpret_cast<int32_t*>(buffer.data());
    }

    void send_begin_computation(const int32_t session_id, const size_t compute_unit)
    {
        auto sz = sizeof(session_id) + sizeof(compute_unit);
        stdsc::Buffer buffer(sz);
        auto* p = reinterpret_cast<uint8_t*>(buffer.data());
        std::memcpy(reinterpret_cast<void*>(p + 0),
                    static_cast<const void*>(&session_id), sizeof(session_id));
        std::memcpy(reinterpret_cast<void*>(p + sizeof(session_id)),
                    static_cast<const void*>(&compute_unit), sizeof(compute_unit));
        
        client_.send_data_blocking(nbc_share::kControlCodeDataBeginComputation, buffer);
    }
    
    void compute(const stdsc::Buffer& sbuffer,
                 stdsc::Buffer& rbuffer)
    {
        client_.send_recv_data_blocking(
            nbc_share::ControlCode_t::kControlCodeUpDownloadComputeData,
            sbuffer, rbuffer);
    }

    void send_end_computation(const int32_t session_id)
    {
        auto sz = sizeof(session_id);
        stdsc::Buffer buffer(sz);
        std::memcpy(buffer.data(), static_cast<const void*>(&session_id), sz);
        client_.send_data_blocking(nbc_share::kControlCodeDataEndComputation, buffer);
    }
    
private:
    stdsc::Client& client_;
};

TAClient::TAClient(const char* host, const char* port)
    : super(host, port)
{
    auto& client = super::client();
    pimpl_ = std::make_shared<Impl>(client);
}

void TAClient::get_session_id(int32_t& session_id)
{
    pimpl_->get_session_id(session_id);
}

void TAClient::send_begin_computation(const int32_t session_id,
                                      const size_t compute_unit)
{
    pimpl_->send_begin_computation(session_id, compute_unit);
}

void TAClient::compute(const stdsc::Buffer& sbuffer,
                       stdsc::Buffer& rbuffer)
{
    pimpl_->compute(sbuffer, rbuffer);
}

void TAClient::send_end_computation(const int32_t session_id)
{
    pimpl_->send_end_computation(session_id);
}

} /* namespace nbc_cs */
