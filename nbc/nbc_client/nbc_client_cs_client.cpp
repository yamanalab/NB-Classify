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
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_permvec.hpp>
#include <nbc_share/nbc_computeparam.hpp>
#include <nbc_client/nbc_client_cs_client.hpp>

namespace nbc_client
{
    
struct CSClient::Impl
{
    Impl(const char* host, const char* port)
        : host_(host),
          port_(port)
    {}

    ~Impl(void)
    {
        disconnect();
    }

    void connect(const uint32_t retry_interval_usec,
                 const uint32_t timeout_sec)
    {
        STDSC_LOG_INFO("Connecting to Server#1 on CS.");
        client_.connect(host_, port_, retry_interval_usec, timeout_sec);
        STDSC_LOG_INFO("Connected to Server#1 on CS.");
    }

    void disconnect(void)
    {
        client_.close();
    }
    
    int32_t send_session_create()
    {
        STDSC_LOG_INFO("Requesting session create.");

        stdsc::Buffer buffer;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadSessionID, buffer);
        const int32_t session_id = *reinterpret_cast<int32_t*>(buffer.data());
        STDSC_LOG_INFO("Sending session id. (id: %d)", session_id);
        return session_id;
    }
    
    void send_encdata(const int32_t session_id, const nbc_share::EncData& encdata)
    {
        stdsc::BufferStream buffstream(encdata.stream_size());
        std::iostream stream(&buffstream);
        encdata.save_to_stream(stream);

        STDSC_LOG_INFO("Sending encrypted input.");
        stdsc::Buffer* buffer = &buffstream;
        client_.send_data_blocking(nbc_share::kControlCodeDataInput, *buffer);
    }

    void send_permvec(const int32_t session_id, const std::vector<long>& permvec)
    {
        auto hdr_sz  = sizeof(size_t);
        auto data_sz = permvec.size() * sizeof(long);
        
        stdsc::Buffer buffer(hdr_sz + data_sz);
        auto* p = static_cast<uint8_t*>(buffer.data());
        auto* hdr_p  = reinterpret_cast<size_t*>(p + 0);
        auto* data_p = static_cast<void*>(p + hdr_sz);
        
        *hdr_p = permvec.size();
        std::memcpy(data_p, permvec.data(), data_sz);

        client_.send_data_blocking(nbc_share::kControlCodeDataPermVec, buffer);
    }

    void send_input(const int32_t session_id,
                    const nbc_share::EncData& encdata,
                    const nbc_share::PermVec& permvec)
    {
        auto size = encdata.stream_size() + permvec.stream_size();
        stdsc::BufferStream buffstream(size);
        std::iostream stream(&buffstream);

        encdata.save_to_stream(stream);
        permvec.save_to_stream(stream);
        
        STDSC_LOG_INFO("Sending input data.");
        stdsc::Buffer* buffer = &buffstream;
        client_.send_data_blocking(nbc_share::kControlCodeDataInput, *buffer);
    }
    
    
    void send_compute_request(const int32_t session_id,
                              const size_t class_num,
                              const size_t num_features,
                              const size_t compute_unit)
    {
        STDSC_LOG_INFO("Requesting compute.");

        nbc_share::ComputeParam cparam;
        cparam.class_num    = class_num;
        cparam.num_features = num_features;
        cparam.compute_unit = compute_unit;
        cparam.session_id   = session_id;
        
        nbc_share::PlainData<nbc_share::ComputeParam> plaindata;
        plaindata.push(cparam);
        
        auto sz = plaindata.stream_size();
        stdsc::BufferStream bufferstream(sz);
        std::iostream stream(&bufferstream);

        plaindata.save_to_stream(stream);

        stdsc::Buffer* buffer = &bufferstream;
        client_.send_data_blocking(nbc_share::kControlCodeDataCompute, *buffer);
    }

private:
    const char* host_;
    const char* port_;    
    stdsc::Client client_;
};

CSClient::CSClient(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

void CSClient::connect(const uint32_t retry_interval_usec,
                       const uint32_t timeout_sec)
{
    pimpl_->connect(retry_interval_usec, timeout_sec);
}

void CSClient::disconnect(void)
{
    pimpl_->disconnect();
}
    
int32_t CSClient::send_session_create(void)
{
    return pimpl_->send_session_create();
}

void CSClient::send_encdata(const int32_t session_id,
                            const nbc_share::EncData& encdata)
{
    pimpl_->send_encdata(session_id, encdata);
}

void CSClient::send_permvec(const int32_t session_id,
                            const std::vector<long>& permvec)
{
    pimpl_->send_permvec(session_id, permvec);
}

void CSClient::send_input(const int32_t session_id,
                          const nbc_share::EncData& encdata,
                          const nbc_share::PermVec& permvec)
{
    pimpl_->send_input(session_id, encdata, permvec);
}

void CSClient::send_compute_request(const int32_t session_id,
                                    const size_t class_num,
                                    const size_t num_features,
                                    const size_t compute_unit)
{
    pimpl_->send_compute_request(session_id, class_num, num_features, compute_unit);
}

} /* namespace nbc_client */
