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

#ifndef NBC_MP_HPP
#define NBC_MP_HPP

#include <memory>
#include <vector>
#include <functional>
#include <nbc_share/nbc_define.hpp>

namespace nbc_mp
{
    
/**
 * @brief Provides client.
 */
class ModelProvider
{
public:
    ModelProvider(const char* ta_host, const char* ta_port,
                  const char* cs_host, const char* cs_port,
                  const bool dl_pubkey = true,
                  const uint32_t retry_interval_usec = NBC_RETRY_INTERVAL_USEC,
                  const uint32_t timeout_sec = NBC_TIMEOUT_SEC);
    virtual ~ModelProvider(void) = default;

    void send_encmodel(const std::vector<std::vector<long>>& probs,
                       const size_t num_features,
                       const size_t class_num);
    
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_mp */

#endif /* NBC_MP_HPP */
