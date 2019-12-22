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

#ifndef NBC_CS_CLIENT_HPP
#define NBC_CS_CLIENT_HPP

#include <memory>
#include <nbc_share/nbc_define.hpp>

namespace helib
{
class Ctxt;
}

namespace nbc_share
{
class PubKey;
class Context;
class ComputeParam;
}

namespace nbc_cs
{
    
class Dataset;
    
/**
 * @brief Provides client.
 */
class Client
{
public:
    Client(const char* ta_host,
           const char* ta_srv1_port,
           const char* ta_srv2_port,
           const bool dl_pubkey = true,
           const uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC,
           const uint32_t timeout_sec = NBC_TIMEOUT_SEC);
    virtual ~Client(void) = default;

    const nbc_share::PubKey&  pubkey(void)  const;
    const nbc_share::Context& context(void) const;

    int32_t create_session(void);
    
    void begin_computation(const int32_t session_id,
                           const size_t compute_unit);
    
    helib::Ctxt compute_on_TA(const helib::Ctxt& ct_diff,
                              const nbc_share::ComputeParam& cparam);
    
    void end_computation(const int32_t session_id);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_cs */

#endif /* NBC_CS_CLIENT_HPP */
