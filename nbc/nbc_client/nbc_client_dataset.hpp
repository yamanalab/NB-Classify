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

#ifndef NBC_CLIENT_DATASET_HPP
#define NBC_CLIENT_DATASET_HPP

#include <string>
#include <vector>

namespace nbc_share
{
class InfoFile;
}

namespace nbc_client
{

class Dataset
{
public:
    Dataset(const nbc_share::InfoFile& info);
    virtual ~Dataset(void) = default;

    void read(const std::string& filename);

    const std::vector<std::vector<long>>& data(void) const;

private:
    nbc_share::InfoFile info_;
    std::vector<std::vector<long>> data_;
};

} /* namespace nbc_client */

#endif /* NBC_CLIENT_DATASET_HPP */
