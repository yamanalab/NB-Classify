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

#ifndef NBC_TA_SHARE_CALLBACK_PARAM_HPP
#define NBC_TA_SHARE_CALLBACK_PARAM_HPP

#include <memory>

namespace nbc_share
{
class SecureKeyFileManager;
class Context;
class PubKey;
class SecKey;
}

namespace nbc_ta
{

class SessionContainer;
    
/**
 * @brief This class is used to hold the callback parameters for Server#1 on TA.
 */
struct CallbackParam
{
    CallbackParam(void);
    ~CallbackParam(void) = default;

    std::shared_ptr<nbc_share::SecureKeyFileManager> skm_ptr;
    std::shared_ptr<nbc_share::Context> context_ptr;
    std::shared_ptr<nbc_share::PubKey>  pubkey_ptr;
    std::shared_ptr<nbc_share::SecKey>  seckey_ptr;

    size_t compute_unit;

    std::shared_ptr<SessionContainer> sc_ptr;
};

} /* namespace nbc_ta */

#endif /* NBC_TA_SHARE_CALLBACK_PARAM_HPP */
