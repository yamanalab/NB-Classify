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

#ifndef NBC_MP_CS_CLIENT_HPP
#define NBC_MP_CS_CLIENT_HPP

#include <memory>
#include <string>
#include <nbc_share/nbc_define.hpp>

namespace nbc_share
{
    class EncData;
}

namespace nbc_mp
{

/**
 * @brief Provides client for Server#1 on CS.
 */
class CSClient
{
public:
    CSClient(const char* host, const char* port);
    virtual ~CSClient(void) = default;

    void connect(const uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC,
                 const uint32_t timeout_sec = NBC_TIMEOUT_SEC);
    void disconnect();
    
    void send_encdata(const nbc_share::EncData& encdata);

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_mp */

#endif /* NBC_MP_CS_CLIENT_HPP */
