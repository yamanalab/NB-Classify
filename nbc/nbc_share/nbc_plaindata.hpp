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

#ifndef NBC_PLAINDATA_HPP
#define NBC_PLAINDATA_HPP

#include <memory>
#include <nbc_share/nbc_basicdata.hpp>

namespace nbc_share
{

/**
 * @brief This clas is used to hold the plain data.
 */
template <class T>
struct PlainData : public nbc_share::BasicData<T>
{
    using super = nbc_share::BasicData<T>;
    
    PlainData() = default;
    virtual ~PlainData(void) = default;

    virtual void save_to_stream(std::ostream& os) const override
    {
        if (super::vec_.size() == 0) {
            return;
        }
        
        os << super::vec_.size() << std::endl;
        for (const auto& v : super::vec_) {
            os << v << std::endl;
        }
    }        
    virtual void load_from_stream(std::istream& is) override
    {
        size_t sz;
        is >> sz;
    
        super::clear();
        
        for (size_t i=0; i<sz; ++i) {
            T v;
            is >> v;
            super::vec_.push_back(v);
        }
    }
};

} /* namespace nbc_share */

#endif /* NBC_PLAINDATA_HPP */
