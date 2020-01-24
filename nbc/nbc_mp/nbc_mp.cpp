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
#include <fstream>
#include <vector>
#include <cstring>
#include <stdsc/stdsc_client.hpp>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_define.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_mp/nbc_mp_ta_client.hpp>
#include <nbc_mp/nbc_mp_cs_client.hpp>
#include <nbc_mp/nbc_mp_model.hpp>
#include <nbc_mp/nbc_mp.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_mp
{
    
struct ModelProvider::Impl
{
    Impl(const char* ta_host, const char* ta_port,
         const char* cs_host, const char* cs_port,
         const bool dl_pubkey,
         const uint32_t retry_interval_usec,
         const uint32_t timeout_sec)
        : ta_client_(new TAClient(ta_host, ta_port)),
          cs_client_(new CSClient(cs_host, cs_port)),
          retry_interval_usec_(retry_interval_usec),
          timeout_sec_(timeout_sec)
    {
        STDSC_IF_CHECK(dl_pubkey, "False of dl_pubkey is not supported yet.");

        ta_client_->connect(retry_interval_usec_, timeout_sec_);
        cs_client_->connect(retry_interval_usec_, timeout_sec_);

        context_ = std::make_shared<nbc_share::Context>();
        ta_client_->get_context(*context_);
        
        pubkey_ = std::make_shared<nbc_share::PubKey>(context_->get());
        ta_client_->get_pubkey(*pubkey_);
    }

    ~Impl(void)
    {
        ta_client_->disconnect();
        cs_client_->disconnect();
    }

    void send_encmodel(const std::vector<std::vector<long>>& probs,
                       const size_t num_features,
                       const size_t class_num)
    {
        auto& context = *context_;
        auto& pubkey  = *pubkey_;

        const auto num_slots = context.get().zMStar.getNSlots();

        size_t num_probs = num_features + 1;
        size_t num_data  = num_slots / num_probs;

        nbc_share::EncData encdata(pubkey);
        
        for (size_t i=0; i<class_num; ++i) {
            
            std::vector<long> tmp = probs[i];

            for (size_t j=0; j<num_data - 1; j++) {
                tmp.insert(tmp.end(), probs[i].begin(), probs[i].end());
            }
            tmp.resize(num_slots);
            
            encdata.encrypt(tmp, context);
        }

        encdata.save_to_file("encmodel.txt");
        cs_client_->send_encdata(encdata);
    }

private:
    std::shared_ptr<TAClient> ta_client_;
    std::shared_ptr<CSClient> cs_client_;
    const uint32_t retry_interval_usec_;
    const uint32_t timeout_sec_;
    std::shared_ptr<nbc_share::Context> context_;
    std::shared_ptr<nbc_share::PubKey> pubkey_;
};

ModelProvider::ModelProvider(const char* ta_host, const char* ta_port,
                             const char* cs_host, const char* cs_port,
                             const bool dl_pubkey,
                             const uint32_t retry_interval_usec,
                             const uint32_t timeout_sec)
    : pimpl_(new Impl(ta_host, ta_port, cs_host, cs_port,
                      dl_pubkey, retry_interval_usec, timeout_sec))
{
}

void ModelProvider::send_encmodel(const std::vector<std::vector<long>>& probs,
                                  const size_t num_features,
                                  const size_t class_num)
{
    pimpl_->send_encmodel(probs, num_features, class_num);
}
    
} /* namespace nbc_mp */
