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
#include <cassert>
#include <sstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_client/nbc_client.hpp>
#include <nbc_client/nbc_client_dataset.hpp>
#include <share/define.hpp>

#define USE_TEST_PERMVEC

static constexpr const char* INFO_FILENAME   = "../../../datasets/sample_info.csv";
static constexpr const char* TEST_FILENAME   = "../../../datasets/sample_test.csv";
static constexpr const char* PVEC_FILENAME   = "../../../testdata/permvec.txt"; // for test mode
static constexpr const char* EXPC_FILENAME   = "../../../testdata/expect_result.txt"; // for verify
static constexpr const char* PUBKEY_FILENAME = "pubkey.txt";

void verify_result(const std::vector<std::string>& result_classes,
                   const std::string& expect_filename)
{
    if (!nbc_share::utility::file_exist(expect_filename)) {
        printf("%s is not found.\n", expect_filename.c_str());
        return;
    }
    
    std::ifstream ifs(expect_filename);
    std::string line;

    std::vector<std::string> expect_classes;
    while (std::getline(ifs, line)) {
        std::istringstream ss(line);
        std::string cls;

        while (std::getline(ss, cls, ',')) {
            expect_classes.push_back(cls);
        }
    }

    bool res = true;
    std::cout << "Verifying results ..." << std::endl;
    for (size_t i=0; i<result_classes.size(); ++i) {
        auto& result = result_classes[i];
        auto& expect = expect_classes[i];
        printf("  expect: %s  - result: %s", expect.c_str(), result.c_str());
        std::string verif;
        if (result == expect) {
            verif = "PASS";
        } else {
            verif = "NG";
            res   = false;
        }
        printf(" : %s\n", verif.c_str());
        fflush(stdout);
    }
    std::cout << "Verified: " << (res ? "PASS" : "NG") << std::endl;
}

struct Option
{
    std::string info_filename = INFO_FILENAME;
    std::string test_filename = TEST_FILENAME;
    std::string expc_filename = EXPC_FILENAME;
    bool verify = false;
};

void init(Option& option, int argc, char* argv[])
{
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "i:t:v:h")) != -1)
    {
        switch (opt)
        {
            case 'i':
                option.info_filename = optarg;
                break;
            case 't':
                option.test_filename = optarg;
                break;
            case 'v':
                option.verify = true;
                option.expc_filename = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s -i [-i info_filename] [-t test_filename] [-v expect_filename]\n",
                       argv[0]);
                exit(1);
        }
    }
}

struct ResultParam
{
    std::vector<int64_t> indexes;
};

void result_cb(const std::vector<int64_t>& indexes, void* args)
{
    std::cout << "called result callback. Index after permutation: [";
    auto* param = static_cast<ResultParam*>(args);
    for (const auto& index : indexes) {
        param->indexes.push_back(index);
        std::cout << index << " ";
    }
    std::cout << "]" << std::endl;
}

void exec(Option& option)
{
    const char* host   = "localhost";
    int32_t session_id = -1;
    ResultParam args;

    nbc_client::Client client(host, PORT_TA_SRV1,
                              host, PORT_CS_SRV1);

    nbc_share::InfoFile info;
    info.read(option.info_filename);
    
    nbc_client::Dataset dataset(info);
    dataset.read(option.test_filename);
    
    std::vector<nbc_share::PermVec> permvecs;

    auto unit = client.calc_computation_unit_size(info.num_features);    

    for (size_t i=0; i<dataset.data().size() / unit; ++i) {
        size_t index = i * unit;
        std::cout << "Classifying data " << index << std::endl;
        
        session_id = client.create_session(result_cb, &args);
        std::cout << "session_id: " << session_id << std::endl;
        
        nbc_share::PermVec permvec;

#if defined(ENABLE_TEST_MODE)
        permvec.load_from_csvfile(PVEC_FILENAME);
#else
        permvec.gen_permvec(info.class_num);
#endif
        for (size_t j=0; j<unit; ++j) {
            permvecs.push_back(permvec);
        }
        
#if defined(USE_MULTI)
        size_t num_probs = info.num_features + 1;
        size_t num_slots = unit * num_probs;
        std::vector<long> data(num_slots, 0);
        for (size_t j=0; j<unit; ++j) {
            for (size_t k=0; k<num_probs; ++k) {
                data[(j * num_probs) + k] = (dataset.data())[index + j][k];
            }
        }
#else        
        const auto& data = (dataset.data())[index];
#endif
        
        client.compute(session_id, data, permvec,
                       info.class_num, info.num_features);
    }

    client.wait();

    std::vector<std::string> result_classes;
    for (size_t i=0; i<args.indexes.size(); ++i) {
        auto& permvec = permvecs[i];
        auto& index   = args.indexes[i];
        auto depermed = (permvec.vdata())[index];
        auto cls      = info.class_names[depermed];
        
        printf("Classification result of %ld: index=%ld, class=%s\n",
               i+1, depermed, cls.c_str());

        result_classes.push_back(cls);
    }

    if (option.verify) {
        verify_result(result_classes, option.expc_filename);
    }
}

int main(int argc, char* argv[])
{
    STDSC_INIT_LOG();
    try
    {
        Option option;
        init(option, argc, argv);
        STDSC_LOG_INFO("Launched Client demo app");
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
