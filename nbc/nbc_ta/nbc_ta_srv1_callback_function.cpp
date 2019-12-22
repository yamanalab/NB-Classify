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
#include <nbc_ta/nbc_ta_share_callback_param.hpp>
#include <nbc_ta/nbc_ta_srv1_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv1_state.hpp>
#include <nbc_ta/nbc_ta_session.hpp>

namespace nbc_ta
{
namespace srv1
{

// CallbackFunctionPubkeyRequest
DEFUN_DOWNLOAD(CallbackFunctionPubkeyRequest)
{
    STDSC_LOG_INFO("Received public key request. (current state : %s)",
                   state.current_state_str().c_str());

    auto  kind = nbc_share::SecureKeyFileManager::Kind_t::kKindPubKey;
    auto& skm  = *param_.skm_ptr;
    stdsc::Buffer pubkey(skm.size(kind));
    skm.data(kind, pubkey.data());
    STDSC_LOG_INFO("Sending public key.");
    sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataPubkey,
                                             skm.size(kind)));
    sock.send_buffer(pubkey);
    state.set(kEventPubkeyRequest);
}

// CallbackFunctionContextRequest
DEFUN_DOWNLOAD(CallbackFunctionContextRequest)
{
    STDSC_LOG_INFO("Received context request. (current state : %s)",
                   state.current_state_str().c_str());

    auto  kind = nbc_share::SecureKeyFileManager::Kind_t::kKindContext;
    auto& skm  = *param_.skm_ptr;
    stdsc::Buffer context(skm.size(kind));
    skm.data(kind, context.data());
    STDSC_LOG_INFO("Sending context.");
    sock.send_packet(stdsc::make_data_packet(nbc_share::kControlCodeDataContext,
                                             skm.size(kind)));
    sock.send_buffer(context);
    state.set(kEventContextRequest);
}

// CallbackFunctiondResultRequest
DEFUN_UPDOWNLOAD(CallbackFunctionResultRequest)
{
    STDSC_LOG_INFO("Received result request. (current state : %s)",
                   state.current_state_str().c_str());

    auto session_id = *reinterpret_cast<const int32_t*>(buffer.data());
    auto& session_container = *param_.sc_ptr;

    if (!session_container.has_context(session_id)) {
        std::ostringstream oss;
        oss << "no context for session#" << session_id;
        STDSC_THROW_CALLBACK(oss.str().c_str());
    }
    
    auto& session = session_container.get(session_id);

    if (!session.computed()) {
        std::ostringstream oss;
        oss << "no result for session#" <<  session_id;
        STDSC_THROW_CALLBACK(oss.str().c_str());
    }

    auto& result_indexes = session.get_results();

    size_t num = result_indexes.size();
    size_t sz  = sizeof(size_t) + num * sizeof(int64_t);
    stdsc::Buffer resbuffer(sz);

    auto* p = static_cast<uint8_t*>(resbuffer.data());
    std::memcpy(reinterpret_cast<void*>(p + 0),
                reinterpret_cast<const void*>(&num), sizeof(size_t));
    std::memcpy(reinterpret_cast<void*>(p + sizeof(size_t)),
                result_indexes.data(),
                num * sizeof(int64_t));

    sock.send_packet(
      stdsc::make_data_packet(nbc_share::kControlCodeDataResultIndex, sz));
    sock.send_buffer(resbuffer);
}

} /* namespace srv1 */
} /* namespace nbc_ta */
