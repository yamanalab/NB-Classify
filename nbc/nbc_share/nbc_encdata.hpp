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

#ifndef NBC_ENCDATA_HPP
#define NBC_ENCDATA_HPP

#include <memory>
#include <nbc_share/nbc_basicdata.hpp>

namespace helib
{
class Ctxt;
}

namespace nbc_share
{

class PubKey;
class SecKey;
class Context;
    
/**
 * @brief This clas is used to hold the encrypted data.
 */
struct EncData : public nbc_share::BasicData<helib::Ctxt>
{
    EncData(const PubKey& pubkey);
    virtual ~EncData(void) = default;

    void encrypt(const std::vector<long>& inputdata,
                 const Context& context);

    void decrypt(const nbc_share::Context& context,
                 const nbc_share::SecKey& seckey,
                 std::vector<long>& outputdata) const;

    virtual void save_to_stream(std::ostream& os) const override;
    virtual void load_from_stream(std::istream& is) override;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_ENCDATA_HPP */
