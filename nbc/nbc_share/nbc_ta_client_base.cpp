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
#include <nbc_share/nbc_ta_client_base.hpp>

namespace nbc_share
{
    
struct TAClientBase::Impl
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
        STDSC_LOG_INFO("Connecting to Server#1 on TA.");
        client_.connect(host_, port_, retry_interval_usec, timeout_sec);
        STDSC_LOG_INFO("Connected to Server#1 on TA.");
    }

    void disconnect(void)
    {
        client_.close();
    }
    
    void get_pubkey(nbc_share::PubKey& pubkey, const std::string filename)
    {
        stdsc::Buffer buffer;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadPubkey, buffer);

        stdsc::BufferStream buffstream(buffer);
        std::iostream stream(&buffstream);
        pubkey.load_from_stream(stream);
    }

    void get_context(nbc_share::Context& context, const std::string filename)
    {
        stdsc::Buffer buffer;
        client_.recv_data_blocking(nbc_share::kControlCodeDownloadContext, buffer);

        stdsc::BufferStream buffstream(buffer);
        std::iostream stream(&buffstream);
        context.load_from_stream(stream);
    }

    stdsc::Client& client(void)
    {
        return client_;
    }

private:
    const char* host_;
    const char* port_;    
    stdsc::Client client_;
};

TAClientBase::TAClientBase(const char* host, const char* port)
  : pimpl_(new Impl(host, port))
{
}

void TAClientBase::connect(const uint32_t retry_interval_usec,
                       const uint32_t timeout_sec)
{
    pimpl_->connect(retry_interval_usec, timeout_sec);
}

void TAClientBase::disconnect(void)
{
    pimpl_->disconnect();
}
    
void TAClientBase::get_pubkey(nbc_share::PubKey& pubkey, const char* filename)
{
    pimpl_->get_pubkey(pubkey, filename);
}

void TAClientBase::get_context(nbc_share::Context& context, const char* filename)
{
    pimpl_->get_context(context, filename);
}

stdsc::Client& TAClientBase::client(void)
{
    return pimpl_->client();
}
    
} /* namespace nbc_share */
