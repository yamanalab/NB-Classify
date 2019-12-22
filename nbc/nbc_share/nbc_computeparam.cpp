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

#include <nbc_share/nbc_computeparam.hpp>

namespace nbc_share
{

std::ostream& operator<<(std::ostream& os, const ComputeParam& cparam)
{
    os << cparam.compute_index << std::endl;
    os << cparam.class_num     << std::endl;
    os << cparam.num_features  << std::endl;
    os << cparam.compute_unit  << std::endl;
    os << cparam.session_id    << std::endl;
    return os;
}

std::istream& operator>>(std::istream& is, ComputeParam& cparam)
{
    is >> cparam.compute_index;
    is >> cparam.class_num;
    is >> cparam.num_features;
    is >> cparam.compute_unit;
    is >> cparam.session_id;
    return is;
}
    
} /* namespace nbc_share */
