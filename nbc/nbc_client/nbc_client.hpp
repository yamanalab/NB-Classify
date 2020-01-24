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

#ifndef NBC_CLIENT_HPP
#define NBC_CLIENT_HPP

#include <memory>
#include <vector>
#include <nbc_share/nbc_define.hpp>
#include <nbc_share/nbc_permvec.hpp>
#include <nbc_client/nbc_client_result_cbfunc.hpp>

namespace nbc_client
{
    
class Dataset;
    
/**
 * @brief Provides client.
 */
class Client
{
    static constexpr int32_t ALL_SESSION = -1;
    
public:
    
    Client(const char* ta_host, const char* ta_port,
           const char* cs_host, const char* cs_port,
           const bool dl_pubkey = true,
           const uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC,
           const uint32_t timeout_sec = NBC_TIMEOUT_SEC);
    virtual ~Client(void) = default;

    size_t calc_computation_unit_size(const size_t num_features) const;
    
    int32_t create_session(nbc_client::cbfunc_t resultcb,
                           void* result_cbargs);

    void compute(const int32_t session_id,
                 const std::vector<long>& data,
                 const nbc_share::PermVec& permvec,
                 const size_t class_num,
                 const size_t num_features);

    void wait(const int32_t session_id = ALL_SESSION);
        
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_client */

#endif /* NBC_CLIENT_HPP */
