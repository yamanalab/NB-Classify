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
#include <nbc_cs/nbc_cs_srv1_state.hpp>
#include <nbc_cs/nbc_cs_srv1.hpp>

namespace nbc_cs
{
namespace srv1
{

struct CSServer::Impl
{
    Impl(const char* port, stdsc::CallbackFunctionContainer& callback,
         stdsc::StateContext& state)
        : server_(new stdsc::Server<>(port, state, callback)), state_(state)
    {
        STDSC_LOG_INFO("Lanched CS server#1 (%s)", port);
    }

    ~Impl(void) = default;

    void start(void)
    {
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
};

CSServer::CSServer(const char* port,
                   stdsc::CallbackFunctionContainer& callback,
                   stdsc::StateContext& state)
  : pimpl_(new Impl(port, callback, state))
{
}

void CSServer::start(void)
{
    pimpl_->start();
}

void CSServer::stop(void)
{
    pimpl_->stop();
}

void CSServer::wait(void)
{
    pimpl_->wait();
}
    
} /* namespace srv1 */
} /* namespace nbc_cs */
