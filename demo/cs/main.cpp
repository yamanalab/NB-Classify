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
#include <nbc_cs/nbc_cs_srv1.hpp>
#include <nbc_cs/nbc_cs_srv1_state.hpp>
#include <nbc_cs/nbc_cs_srv1_callback_function.hpp>
#include <nbc_cs/nbc_cs_share_callback_param.hpp>
#include <nbc_cs/nbc_cs_client.hpp>
#include <share/define.hpp>

static constexpr const char* PUBKEY_FILENAME   = "pubkey.txt";
static constexpr const char* CONTEXT_FILENAME  = "context.txt";
static constexpr const char* ENCDATA_FILENAME  = "encdata.txt";
static constexpr const char* ENCMODEL_FILENAME = "encmodel.txt";

struct Option
{
    std::string pubkey_filename  = PUBKEY_FILENAME;
    std::string context_filename = CONTEXT_FILENAME;
};

void init(Option& param, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "h")) != -1)
    {
        switch (opt)
        {
            case 'h':
            default:
                printf(
                  "Usage: %s\n", argv[0]);
                exit(1);
        }
    }
}

static void exec(const Option& opt)
{
    const char* host   = "localhost";
    
    stdsc::StateContext state1(std::make_shared<nbc_cs::srv1::StateReady>());

    stdsc::CallbackFunctionContainer callback1;
    nbc_cs::CallbackParam cb_param;
    {        
        cb_param.pubkey_filename   = PUBKEY_FILENAME;
        cb_param.context_filename  = CONTEXT_FILENAME;
        cb_param.encdata_filename  = ENCDATA_FILENAME;
        cb_param.encmodel_filename = ENCMODEL_FILENAME;
        
        std::shared_ptr<nbc_cs::Client> client(
            new nbc_cs::Client(host, PORT_TA_SRV1, PORT_TA_SRV2));
        cb_param.set_client(client);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_session(
            new nbc_cs::srv1::CallbackFunctionSessionCreate(cb_param));
        callback1.set(nbc_share::kControlCodeDownloadSessionID, cb_session);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_model(
            new nbc_cs::srv1::CallbackFunctionEncModel(cb_param));
        callback1.set(nbc_share::kControlCodeDataModel, cb_model);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_input(
            new nbc_cs::srv1::CallbackFunctionEncInput(cb_param));
        callback1.set(nbc_share::kControlCodeDataInput, cb_input);

//        std::shared_ptr<stdsc::CallbackFunction> cb_permvec(
//            new nbc_cs::srv1::CallbackFunctionPermVec(cb_param));
//        callback1.set(nbc_share::kControlCodeDataPermVec, cb_permvec);
        
        std::shared_ptr<stdsc::CallbackFunction> cb_compute(
            new nbc_cs::srv1::CallbackFunctionComputeRequest(cb_param));
        callback1.set(nbc_share::kControlCodeDataCompute, cb_compute);
    }

    std::shared_ptr<nbc_cs::srv1::CSServer> server1
        = std::make_shared<nbc_cs::srv1::CSServer>(PORT_CS_SRV1, callback1, state1);
    server1->start();

    server1->wait();
}    

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        STDSC_LOG_INFO("Start CS");
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

