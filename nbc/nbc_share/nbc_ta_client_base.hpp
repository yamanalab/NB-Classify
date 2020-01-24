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

#ifndef NBC_TA_CLIENT_BASE_HPP
#define NBC_TA_CLIENT_BASE_HPP

#include <memory>
#include <string>
#include <nbc_share/nbc_define.hpp>

namespace stdsc
{
    class Client;
}

namespace nbc_share
{
    
class PubKey;
class Context;
    
/**
 * @brief Provides client for TA.
 */
class TAClientBase
{
public:
    TAClientBase(const char* host, const char* port);
    virtual ~TAClientBase(void) = default;

    void connect(const uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC,
                 const uint32_t timeout_sec = NBC_TIMEOUT_SEC);
    void disconnect();
    
    void get_pubkey(nbc_share::PubKey& pubkey,
                    const char* filename = "pubkey.txt");

    void get_context(nbc_share::Context& context,
                     const char* filename = "context.txt");

protected:
    stdsc::Client& client(void);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_TA_CLIENT_BASE_HPP */
