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

#ifndef NBC_MP_MODEL_HPP
#define NBC_MP_MODEL_HPP

#include <string>
#include <vector>

namespace nbc_share
{
class InfoFile;
}

namespace nbc_mp
{

class DataInfo;
    
struct Model
{
    Model(const nbc_share::InfoFile& info);
    virtual ~Model(void) = default;

    void read(const std::string& filename);

    void print(void) const;
    
    const int class_num(void) const
    {
        return class_num_;
    }

    const std::vector<std::vector<long>>& probs(void) const
    {
        return probs_;
    }
    
private:
    nbc_share::InfoFile info_;
    int class_num_;
    int feature_num_;
    std::vector<int> feature_value_num_;
    std::vector<std::vector<long>> probs_;
};

} /* namespace nbc_mp */

#endif /* NBC_MP_MODEL_HPP */
