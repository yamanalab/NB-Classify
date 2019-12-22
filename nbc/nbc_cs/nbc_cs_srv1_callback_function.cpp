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

#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_permvec.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_plaindata.hpp>
#include <nbc_share/nbc_computeparam.hpp>
#include <nbc_cs/nbc_cs_share_callback_param.hpp>
#include <nbc_cs/nbc_cs_srv1_callback_function.hpp>
#include <nbc_cs/nbc_cs_client.hpp>
#include <nbc_cs/nbc_cs_srv1_state.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

#if defined(ENABLE_TEST_MODE)
#include <nbc_share/nbc_seckey.hpp>
#endif


namespace nbc_cs
{
namespace srv1
{

#if defined(USE_SINGLE_OPT) || defined(USE_MULTI)
static void
modifiedTotalSums(const helib::EncryptedArray& ea, helib::Ctxt& ctxt, long n)
{
    if (n == 1) return;

    helib::Ctxt orig = ctxt;
    long k = NTL::NumBits(n), e = 1;

    for (long i = k-2; i >= 0; i--) {
        helib::Ctxt tmp1 = ctxt;
        ea.rotate(tmp1, e);
        ctxt += tmp1; // ctxt = ctxt + (ctxt >>> e)
        e = 2*e;

        if (NTL::bit(n, i)) {
            helib::Ctxt tmp2 = orig;
            ea.rotate(tmp2, e);
            ctxt += tmp2;
            e += 1;
        }
    }
}
#endif /*#if defined(USE_SINGLE_OPT) */

long calc_coeff(void)
{
#if defined(ENABLE_TEST_MODE)
    long coeff = 10;
#else
    long coeff = (std::rand() % 100) + 1;
#endif
    return coeff;
}
    
// CallbackFunctionSessionCreate
DEFUN_DOWNLOAD(CallbackFunctionSessionCreate)
{
    STDSC_LOG_INFO("Received session create. (current state : %s)",
                   state.current_state_str().c_str());
    
    auto& client = param_.get_client();
    auto session_id = client.create_session();
    const size_t size = sizeof(session_id);
    
    stdsc::Buffer buffer(size);
    std::memcpy(buffer.data(), &session_id, size);
    STDSC_LOG_INFO("Sending session id. (id: %d)", session_id);
    sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataSessionID, size));
    sock.send_buffer(buffer);
    state.set(kEventSessionCreate);
}

// CallbackFunctionEncModel
DEFUN_DATA(CallbackFunctionEncModel)
{
    STDSC_LOG_INFO("Received encrypted model. (current state : %s)",
                   state.current_state_str().c_str());
    
    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);
    
    auto& client = param_.get_client();

    std::shared_ptr<nbc_share::EncData> encmodel_ptr(new nbc_share::EncData(client.pubkey()));
    encmodel_ptr->load_from_stream(stream);
    param_.encmodel_ptr = encmodel_ptr;

    state.set(kEventEncModel);
}

// CallbackFunctionEncInput
DEFUN_DATA(CallbackFunctionEncInput)
{
    STDSC_LOG_INFO("Received input data. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateSessionCreated == state.current_state() ||
         kStateComputable     == state.current_state()),
        "Warn: must be SessionCreated or Computable state to receive encrypting input.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);

    auto& client = param_.get_client();

    std::shared_ptr<nbc_share::EncData> encdata_ptr(new nbc_share::EncData(client.pubkey()));
    encdata_ptr->load_from_stream(stream);
    param_.encdata_ptr = encdata_ptr;

    std::shared_ptr<nbc_share::PermVec> permvec_ptr(new nbc_share::PermVec());
    permvec_ptr->load_from_stream(stream);
    param_.permvec_ptr = permvec_ptr;

    encdata_ptr->save_to_file(param_.encdata_filename);

    {
        const auto& vec = permvec_ptr->vdata();
        param_.permvec.resize(vec.size(), -1);
        std::memcpy(param_.permvec.data(), vec.data(), sizeof(long) * vec.size());
    }
    
    {        
        std::ostringstream oss;
        oss << "permvec: sz=" << param_.permvec.size();
        oss << ", data=";
        for (auto& v : param_.permvec) oss << " " << v;
        STDSC_LOG_DEBUG(oss.str().c_str());
    }
    
    state.set(kEventEncInput);
    state.set(kEventPermVec);
}

// CallbackFunctionComputeRequest
DEFUN_DATA(CallbackFunctionComputeRequest)
{
    STDSC_LOG_INFO("Received compute request. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        kStateComputable == state.current_state(),
        "Warn: must be Computable state to receive compute request.");

    stdsc::BufferStream buffstream(buffer);
    std::iostream stream(&buffstream);
    
    nbc_share::PlainData<nbc_share::ComputeParam> plaindata;
    plaindata.load_from_stream(stream);
    
    auto& cparam = plaindata.data();
    auto  class_num    = cparam.class_num;
    auto  num_features = cparam.num_features;
    auto  compute_unit = cparam.compute_unit;
    auto  session_id   = cparam.session_id;

    STDSC_LOG_INFO("start computing of session#%d. (class_num:%lu, num_features:%lu)",
                   session_id, class_num, num_features);
    
    auto& client  = param_.get_client();
    auto& context = client.context();

    auto& ct_data     = param_.encdata_ptr->data();
    auto& model_ctxts = param_.encmodel_ptr->vdata();
    auto& permvec     = param_.permvec;

    auto& context_data = context.get();
    NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
    helib::EncryptedArray ea(context_data, G);

    long num_probs = static_cast<long>(num_features + 1);
    long num_slots = context_data.zMStar.getNSlots();
    long num_data  = num_slots / num_probs;
    STDSC_LOG_DEBUG("num_probs:%ld, num_slots:%ld, num_data:%ld",
                    num_probs, num_slots, num_data);

    std::vector<helib::Ctxt> res_ctxts;
    for (size_t j=0; j<class_num; ++j) {
        auto res = model_ctxts[j];
        res.multiplyBy(ct_data);
#if defined(USE_SINGLE_OPT) || defined(USE_MULTI)
        modifiedTotalSums(ea, res, num_probs);
#else
        totalSums(ea, res);
#endif
        res_ctxts.push_back(res);
        
#if defined(ENABLE_TEST_MODE)
        {
            nbc_share::SecKey seckey(context_data);
            seckey.load_from_file("../../../testdata/sk_m11119_p2_L180.bin");
            auto& seckey_data = seckey.get();

            NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
            helib::EncryptedArray ea(context_data, G);
            
            std::vector <long> m;
            ea.decrypt(res, seckey_data, m);
            printf("res_ctxts[%ld]: sz:%ld, [0]:%ld\n",
                   j, m.size(), m[0]);
        }
#endif /* #if defined(ENABLE_TEST_MODE) */
    }
    STDSC_LOG_TRACE("finished calculating probability of each class");

    std::vector<helib::Ctxt> permed;
    for (size_t j=0; j<permvec.size(); ++j) {
        permed.push_back(res_ctxts[permvec[j]]);
    }
    STDSC_LOG_TRACE("Permuted the probability ciphertexts");

    client.begin_computation(session_id, compute_unit);

    helib::Ctxt max = permed[0];

    std::srand(std::time(nullptr));
    
    for (size_t j=1; j<class_num; ++j) {
        STDSC_LOG_INFO("computation loop (%lu / %lu)",
                       j, class_num);

        auto ct_diff = permed[j];
        auto tmp = max;
        ct_diff -= tmp;
#if defined(USE_SINGLE_OPT) || defined(USE_MULTI)
        std::vector<long> mask(num_slots, 0);
    #if defined(USE_MULTI)
        for (size_t k=1; k<=compute_unit; ++k) {
            mask[(num_probs * k) - 1] = calc_coeff();
        }
    #else
        mask[num_probs - 1] = calc_coeff();
    #endif      
        NTL::ZZX mask_poly;
        ea.encode(mask_poly, mask);
        ct_diff.multByConstant(mask_poly);
#else
        ct_diff.multByConstant(NTL::to_ZZ(calc_coeff()));
#endif
        STDSC_LOG_TRACE("computed ct_diff");

#if defined(ENABLE_TEST_MODE)
        {
            nbc_share::SecKey seckey(context_data);
            seckey.load_from_file("../../../testdata/sk_m11119_p2_L180.bin");
            auto& seckey_data = seckey.get();

            NTL::ZZX G = context_data.alMod.getFactorsOverZZ()[0];
            helib::EncryptedArray ea(context_data, G);
            
            std::vector <long> m;
            ea.decrypt(max, seckey_data, m);
            std::vector<long> p;
            ea.decrypt(permed[j], seckey_data, p);
            printf("j:%ld, max: sz:%ld, [0]:%ld, permed[j]: sz:%ld, [0]:%ld\n",
                   j, m.size(), m[0], p.size(), p[0]);
        }
#endif /* #if defined(ENABLE_TEST_MODE) */

        cparam.compute_index = j;
        auto ct_b = client.compute_on_TA(ct_diff, cparam);
        
        helib::Ctxt cur_max = max;
        max = permed[j];
        max.multiplyBy(ct_b);
        helib::Ctxt ct_temp = ct_b;
        ct_temp.addConstant(NTL::to_ZZ(-1));
        ct_temp.multiplyBy(cur_max);
        max -= ct_temp;
    }

    client.end_computation(session_id);
}

} /* namespace srv1 */
} /* namespace nbc_cs */
