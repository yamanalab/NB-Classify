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
#include <vector>
#include <cstring>
#include <stdsc/stdsc_buffer.hpp>
#include <stdsc/stdsc_socket.hpp>
#include <stdsc/stdsc_packet.hpp>
#include <stdsc/stdsc_state.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_share/nbc_encdata.hpp>
#include <nbc_share/nbc_plaindata.hpp>
#include <nbc_share/nbc_computeparam.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_seckey.hpp>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>
#include <nbc_ta/nbc_ta_srv2_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv2_state.hpp>
#include <nbc_ta/nbc_ta_session.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_ta
{
namespace srv2
{

std::vector<long> compute(const nbc_share::EncData& enc_diff,
                          const nbc_share::ComputeParam& cparam,
                          const nbc_share::Context& context,
                          const nbc_share::SecKey& seckey,
                          const long plain_mod,
                          const std::vector<long>& last_indexes,
                          const size_t compute_unit,
                          std::vector<long>& vec_b)
{
    const size_t num_probs = cparam.num_features + 1;
    
    std::vector<long> vdiff;
    std::vector<long> new_indexes(last_indexes);

    enc_diff.decrypt(context, seckey, vdiff);

    STDSC_LOG_DEBUG("vdiff: sz:%ld, [0]:%ld, plain_mod/2:%ld, compute_unit:%lu, num_probs:%lu\n",
                    vdiff.size(), vdiff[0], plain_mod/2, compute_unit, num_probs);

#if defined (USE_MULTI)
    for (size_t k=1; k<=compute_unit; ++k) {
        long b;
        STDSC_LOG_DEBUG("k:%lu, vdiff:%ld", k, vdiff[(num_probs * k) - 1]);
        if (vdiff[(num_probs * k) - 1] < plain_mod / 2) {
            b = 1;
            new_indexes[k - 1] = cparam.compute_index;
        } else {
            b = 0;
        }
        vec_b[(num_probs * k) - 1] = b;
    }
#else

  #if defined(USE_SINGLE_OPT)
    size_t index = num_probs - 1;
  #else
    size_t index = 0;
  #endif
    
    long b = 0;
    if (vdiff[index] < plain_mod / 2) {
        b = 1;
        std::fill(new_indexes.begin(), new_indexes.end(), cparam.compute_index);
    } else {
        b = 0;
    }
    std::fill(vec_b.begin(), vec_b.end(), b);
#endif
    
    return new_indexes;
}
    
// CallbackFunctionSessionCreate
DEFUN_DOWNLOAD(CallbackFunctionSessionCreate)
{
    STDSC_LOG_INFO("Received session create. (current state : %s)",
                   state.current_state_str().c_str());

    const int32_t session_id = nbc_share::utility::gen_uuid();
    const size_t size = sizeof(session_id);

    STDSC_LOG_TRACE("generate session#%d", session_id);
    
    stdsc::Buffer buffer(size);
    std::memcpy(buffer.data(), &session_id, size);
    
    sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataSessionID,
                                             sizeof(session_id)));
    sock.send_buffer(buffer);
    state.set(kEventSessionCreate);
}

// CallbackFunctionBeginComputation
DEFUN_DATA(CallbackFunctionBeginComputation)
{
    STDSC_LOG_INFO("Received begin computation. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateSessionCreated == state.current_state() ||
         kStateComputing == state.current_state()),
        "Warn: must be session created or computing state to receive begin computation.");

    DEF_CDATA_ON_EACH(nbc_ta::CallbackParam);
    
    const auto* p = reinterpret_cast<const uint8_t*>(buffer.data());
    auto session_id   = *reinterpret_cast<const int32_t*>(p + 0);
    auto compute_unit = *reinterpret_cast<const size_t*>(p + sizeof(int32_t));
    STDSC_LOG_DEBUG("session_id:%d, compute_unit:%lu", session_id, compute_unit);
    cdata_e->sc_ptr->initialize(session_id, compute_unit);

    cdata_e->compute_unit = compute_unit;
    
    state.set(kEventBeginRequest);
}

// CallbackFunctionComputeData
DEFUN_UPDOWNLOAD(CallbackFunctionCompute)
{
    STDSC_LOG_INFO("Received compute request. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        kStateComputable <= state.current_state(),
        "Warn: must be computable state to receive compute request.");

    DEF_CDATA_ON_EACH(nbc_ta::CallbackParam);

    const auto& context = *cdata_e->context_ptr;
    const auto& pubkey  = *cdata_e->pubkey_ptr;
    const auto& seckey  = *cdata_e->seckey_ptr;
    
    stdsc::BufferStream upbuffstream(buffer);
    std::iostream upstream(&upbuffstream);

    nbc_share::EncData ust_encdata(pubkey);
    nbc_share::PlainData<nbc_share::ComputeParam> ust_pladata;
    
    ust_encdata.load_from_stream(upstream);
    ust_pladata.load_from_stream(upstream);

    const auto& ct_diff = ust_encdata.data();
    const auto& cparam  = ust_pladata.data();

    STDSC_LOG_INFO("compute %lu (class_num:%lu, num_features:%lu, compute_unit:%lu, session_id:%d)",
                   cparam.compute_index,
                   cparam.class_num,
                   cparam.num_features,
                   cparam.compute_unit,
                   cparam.session_id);

    long num_slots = cdata_e->context_ptr->get().zMStar.getNSlots();
    std::vector<long> vec_b(num_slots);

    long   plain_mod    = cdata_e->skm_ptr->plain_mod();
    auto&  indexes      = cdata_e->sc_ptr->get(cparam.session_id).get_results();
    size_t compute_unit = cdata_e->compute_unit;

    std::vector<long> last_indexes(indexes);

    nbc_share::EncData enc_diff(pubkey);
    enc_diff.push(ct_diff);
    auto new_indexes = compute(enc_diff, cparam, context, seckey,
                               plain_mod, last_indexes, compute_unit, vec_b);
    cdata_e->sc_ptr->set_results(cparam.session_id, new_indexes);

    nbc_share::EncData dst_encdata(pubkey);
    dst_encdata.encrypt(vec_b, context);

    auto dst_encdata_size = dst_encdata.stream_size();
    stdsc::BufferStream dlbuffstream(dst_encdata_size);
    std::iostream dlstream(&dlbuffstream);

    dst_encdata.save_to_stream(dlstream);
    stdsc::Buffer* dlbuffer = &dlbuffstream;
    sock.send_packet(
      stdsc::make_data_packet(nbc_share::kControlCodeDataComputeAck,
                              dst_encdata_size));
    sock.send_buffer(*dlbuffer);
}

// CallbackFunctionEndComputation
DEFUN_DATA(CallbackFunctionEndComputation)
{
    STDSC_LOG_INFO("Received end computation. (current state : %s)",
                   state.current_state_str().c_str());
    STDSC_THROW_CALLBACK_IF_CHECK(
        (kStateComputable == state.current_state() ||
         kStateComputing  == state.current_state()),
        "Warn: must be computable or computing state to receive begin computation.");

    DEF_CDATA_ON_EACH(nbc_ta::CallbackParam);
    
    auto session_id = *reinterpret_cast<const int32_t*>(buffer.data());
    cdata_e->sc_ptr->set_computed(session_id);

    STDSC_LOG_DEBUG("end computation: session_id:%d", session_id);

    state.set(kEventEndRequest);
}

} /* namespace srv2 */
} /* namespace nbc_ta */
