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

#include <memory>
#include <nbc_ta/nbc_ta_session.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>

namespace nbc_ta
{

CallbackParam::CallbackParam(void)
    : compute_unit(0),
      sc_ptr(new SessionContainer())
{
}
    
} /* namespace nbc_ta */
