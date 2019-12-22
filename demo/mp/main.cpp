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
#include <string>
#include <iostream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_mp/nbc_mp.hpp>
#include <nbc_mp/nbc_mp_model.hpp>
#include <nbc_mp/nbc_mp.hpp>
#include <share/define.hpp>

static constexpr const char* INFO_FILENAME   = "../../../datasets/sample_info.csv";
static constexpr const char* MODEL_FILENAME  = "../../../datasets/sample_model.csv";
static constexpr const char* PUBKEY_FILENAME = "pubkey.txt";

struct Option
{
    std::string info_filename;
    std::string model_filename;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:m:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                option.info_filename = optarg;
                break;
            case 'm':
                option.model_filename = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s [-i info_filename] [-m model_filename]\n", argv[0]);
                exit(1);
        }
    }
}

void exec(Option& option)
{
    const char* host   = "localhost";

    nbc_mp::ModelProvider mp(host, PORT_TA_SRV1, host, PORT_CS_SRV1);

    nbc_share::InfoFile info;
    info.read(INFO_FILENAME);
    
    nbc_mp::Model model(info);
    model.read(MODEL_FILENAME);

    mp.send_encmodel(model.probs(), info.num_features, model.class_num());
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched ModelProvider demo app");
        exec(option);
    }
    catch (stdsc::AbstractException& e)
    {
        STDSC_LOG_ERR("Err: %s", e.what());
    }
    catch (...)
    {
        STDSC_LOG_ERR("Catch unknown exception");
    }

    return 0;
}
