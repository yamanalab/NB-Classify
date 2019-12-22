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

#ifndef NBC_CLIENT_TA_CLIENT_HPP
#define NBC_CLIENT_TA_CLIENT_HPP

#include <memory>
#include <vector>
#include <nbc_share/nbc_ta_client_base.hpp>

namespace nbc_client
{
    
/**
 * @brief Provides client for Server#1 on TA.
 */
class TAClient : public nbc_share::TAClientBase
{
    using super = nbc_share::TAClientBase;
    
public:
    TAClient(const char* host, const char* port);
    virtual ~TAClient(void) = default;

    std::vector<int64_t> get_results(const int32_t session_id) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};
    
} /* namespace nbc_client */

#endif /* NBC_CLIENT_TA_CLIENT_HPP */
