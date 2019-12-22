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

#include <unistd.h>
#include <iostream>
#include <memory>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_callback_function_container.hpp>
#include <stdsc/stdsc_server.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_packet.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_share/nbc_context.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_seckey.hpp>
#include <nbc_ta/nbc_ta_srv1_state.hpp>
#include <nbc_ta/nbc_ta_srv1_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv2_state.hpp>
#include <nbc_ta/nbc_ta_srv2_callback_function.hpp>
#include <nbc_ta/nbc_ta_srv.hpp>
#include <nbc_ta/nbc_ta_share_callback_param.hpp>
#include <share/define.hpp>

#if !defined(ENABLE_TEST_MODE)
static constexpr const char* PUBKEY_FILENAME  = "pubkey.txt";
static constexpr const char* SECKEY_FILENAME  = "seckey.txt";
static constexpr const char* CONTEXT_FILENAME = "context.txt";
#else
static constexpr const char* PUBKEY_FILENAME  = "../../../testdata/pk_m11119_p2_L180.bin";
static constexpr const char* SECKEY_FILENAME  = "../../../testdata/sk_m11119_p2_L180.bin";
static constexpr const char* CONTEXT_FILENAME = "../../../testdata/context_m11119_p2_L180.bin";
#endif

struct Option
{
    std::string pubkey_filename  = PUBKEY_FILENAME;
    std::string seckey_filename  = SECKEY_FILENAME;
    std::string context_filename = CONTEXT_FILENAME;
    std::string config_filename; // set empty if file is specified
    bool is_generate_securekey   = false;
};

void init(Option& param, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "p:s:c:t:gh")) != -1)
    {
        switch (opt)
        {
            case 'p':
                param.pubkey_filename = optarg;
                break;
            case 's':
                param.seckey_filename = optarg;
                break;
            case 'c':
                param.context_filename = optarg;
                break;
            case 't':
                param.config_filename = optarg;
                break;
            case 'g':
                param.is_generate_securekey = true;
                break;
            case 'h':
            default:
                printf(
                  "Usage: %s [-p pubkey_filename] [-s seckey_filename] [-c context_filename] [-g]\n",
                  argv[0]);
                exit(1);
        }
    }

#if defined(ENABLE_TEST_MODE)
    param.is_generate_securekey = false;
#endif
}

static std::shared_ptr<nbc_ta::TAServer>
start_srv1_async(nbc_ta::CallbackParam& cb_param)
{
    stdsc::StateContext state(std::make_shared<nbc_ta::srv1::StateReady>());

    stdsc::CallbackFunctionContainer callback;
    {
        std::shared_ptr<stdsc::CallbackFunction> cb_pubkey(
            new nbc_ta::srv1::CallbackFunctionPubkeyRequest(cb_param));
        callback.set(nbc_share::kControlCodeDownloadPubkey, cb_pubkey);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_context(
            new nbc_ta::srv1::CallbackFunctionContextRequest(cb_param));
        callback.set(nbc_share::kControlCodeDownloadContext, cb_context);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_result(
            new nbc_ta::srv1::CallbackFunctionResultRequest(cb_param));
        callback.set(nbc_share::kControlCodeUpDownloadResult, cb_result);
    }

    std::shared_ptr<nbc_ta::TAServer> server = std::make_shared<nbc_ta::TAServer>(
        PORT_TA_SRV1, callback, state, *cb_param.skm_ptr);
    server->start();

    return server;
}    

static std::shared_ptr<nbc_ta::TAServer>
start_srv2_async(nbc_ta::CallbackParam& cb_param)
{
    stdsc::StateContext state(std::make_shared<nbc_ta::srv2::StateReady>());

    stdsc::CallbackFunctionContainer callback;
    {
        std::shared_ptr<stdsc::CallbackFunction> cb_session(
            new nbc_ta::srv2::CallbackFunctionSessionCreate(cb_param));
        callback.set(nbc_share::kControlCodeDownloadSessionID, cb_session);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_begin(
            new nbc_ta::srv2::CallbackFunctionBeginComputation(cb_param));
        callback.set(nbc_share::kControlCodeDataBeginComputation, cb_begin);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_compute(
            new nbc_ta::srv2::CallbackFunctionCompute(cb_param));
        callback.set(nbc_share::kControlCodeUpDownloadComputeData, cb_compute);

        std::shared_ptr<stdsc::CallbackFunction> cb_end(
            new nbc_ta::srv2::CallbackFunctionEndComputation(cb_param));
        callback.set(nbc_share::kControlCodeDataEndComputation, cb_end);
    }

    std::shared_ptr<nbc_ta::TAServer> server = std::make_shared<nbc_ta::TAServer>(
        PORT_TA_SRV2, callback, state, *cb_param.skm_ptr);
    server->start();

    return server;
}    

static void exec(const Option& opt)
{
    std::shared_ptr<nbc_share::SecureKeyFileManager> skm_ptr;
    if (opt.config_filename.empty()) {
        skm_ptr = std::make_shared<nbc_share::SecureKeyFileManager>(
            opt.pubkey_filename,
            opt.seckey_filename,
            opt.context_filename);
    } else {
        skm_ptr = std::make_shared<nbc_share::SecureKeyFileManager>(
            opt.pubkey_filename,
            opt.seckey_filename,
            opt.context_filename,
            opt.config_filename);
    }
    
    if (opt.is_generate_securekey) {
        skm_ptr->initialize();
    }

    nbc_ta::CallbackParam cb_param;
    cb_param.skm_ptr = skm_ptr;
    cb_param.context_ptr = std::make_shared<nbc_share::Context>();
    cb_param.context_ptr->load_from_file(opt.context_filename);
    cb_param.pubkey_ptr = std::make_shared<nbc_share::PubKey>(cb_param.context_ptr->get());
    cb_param.pubkey_ptr->load_from_file(opt.pubkey_filename);
    cb_param.seckey_ptr = std::make_shared<nbc_share::SecKey>(cb_param.context_ptr->get());
    cb_param.seckey_ptr->load_from_file(opt.seckey_filename);
    
    auto server1 = start_srv1_async(cb_param);
    auto server2 = start_srv2_async(cb_param);

    server1->wait();
    server2->wait();
}    

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        STDSC_LOG_INFO("Start TA");
        Option opt;
        init(opt, argc, argv);
        exec(opt);
    }
    catch (stdsc::AbstractException &e)
    {
        std::cerr << "catch exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "catch unknown exception" << std::endl;
    }
    return 0;
}
