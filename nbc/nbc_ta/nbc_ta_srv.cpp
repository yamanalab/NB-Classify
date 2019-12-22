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

#include <sstream>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_ta/nbc_ta_srv1_state.hpp>
#include <nbc_ta/nbc_ta_srv.hpp>

namespace nbc_ta
{

struct TAServer::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state,
         nbc_share::SecureKeyFileManager& skm)
        : server_(new stdsc::Server<>(port, state, callback)),
          state_(state),
          skm_(skm)
    {
        STDSC_LOG_INFO("Lanched TA server (%s)", port);
    }

    ~Impl(void) = default;

    void start(void)
    {
        auto kind_pubkey = static_cast<int32_t>(nbc_share::SecureKeyFileManager::Kind_t::kKindPubKey);
        auto kind_seckey = static_cast<int32_t>(nbc_share::SecureKeyFileManager::Kind_t::kKindSecKey);

        for (int32_t k=kind_pubkey; k<=kind_seckey; ++k) {
            auto kind = static_cast<nbc_share::SecureKeyFileManager::Kind_t>(k);
            if (!skm_.is_exist(kind))
            {
                std::ostringstream oss;
                oss << "Err: key file not found. (" << skm_.filename(kind) << ")" << std::endl;
                STDSC_THROW_FILE(oss.str());
            }
        }

        bool enable_async_mode = true;
        server_->start(enable_async_mode);
    }

    void stop(void)
    {
        server_->stop();
    }

    void wait(void)
    {
        server_->wait();
    }

private:
    std::shared_ptr<stdsc::Server<>> server_;
    stdsc::StateContext& state_;
    nbc_share::SecureKeyFileManager& skm_;
};

TAServer::TAServer(const char* port,
                   stdsc::CallbackFunctionContainer& callback,
                   stdsc::StateContext& state,
                   nbc_share::SecureKeyFileManager& skm)
    : pimpl_(new Impl(port, callback, state, skm))
{
}

void TAServer::start(void)
{
    pimpl_->start();
}

void TAServer::stop(void)
{
    pimpl_->stop();
}

void TAServer::wait(void)
{
    pimpl_->wait();
}

} /* namespace nbc_ta */
