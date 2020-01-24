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

#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <nbc_cs/nbc_cs_client.hpp>
#include <nbc_cs/nbc_cs_share_callback_param.hpp>

namespace nbc_cs
{

struct CallbackParam::Impl
{
    Impl(void) = default;
    ~Impl(void) = default;

    void set_client(std::shared_ptr<nbc_cs::Client>& client)
    {
        client_ = client;
    }
    
    nbc_cs::Client& get_client(void)
    {
        STDSC_THROW_FAILURE_IF_CHECK(client_,
                                     "Err: Client is not set.");
        return *client_;
    }
    
private:
    std::shared_ptr<nbc_cs::Client> client_;
};

CallbackParam::CallbackParam(void)
    : pimpl_(new Impl())
{}

void CallbackParam::set_client(std::shared_ptr<nbc_cs::Client>& client)
{
    pimpl_->set_client(client);
}
    
nbc_cs::Client& CallbackParam::get_client(void)
{
    return pimpl_->get_client();
}

} /* namespace nbc_cs */
