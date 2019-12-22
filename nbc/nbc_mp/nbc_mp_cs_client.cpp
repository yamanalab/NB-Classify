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
#include <nbc_mp/nbc_mp_cs_client.hpp>

namespace nbc_mp
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
    
    void send_encdata(const nbc_share::EncData& encdata)
    {
        stdsc::BufferStream buffstream(encdata.stream_size());
        std::iostream stream(&buffstream);
        encdata.save_to_stream(stream);

        STDSC_LOG_INFO("Sending encrypted model.");
        stdsc::Buffer* buffer = &buffstream;
        client_.send_data_blocking(nbc_share::kControlCodeDataModel, *buffer);
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
    
void CSClient::send_encdata(const nbc_share::EncData& encdata)
{
    pimpl_->send_encdata(encdata);
}

} /* namespace nbc_mp */
