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

#ifndef NBC_COMPUTEPARAM_HPP
#define NBC_COMPUTEPARAM_HPP

#include <iostream>
#include <nbc_share/nbc_plaindata.hpp>

namespace nbc_share
{

/**
 * @brief This clas is used to hold the parameters to compute between CS and TA.
 */
struct ComputeParam
{
    size_t  compute_index = -1;
    size_t  class_num     = -1;
    size_t  num_features  = -1;
    size_t  compute_unit  = -1;
    int32_t session_id    = -1;;
};

std::ostream& operator<<(std::ostream& os, const ComputeParam& cparam);
std::istream& operator>>(std::istream& is, ComputeParam& cparam);

} /* namespace nbc_share */

#endif /* NBC_COMPUTEPARAM_HPP */
