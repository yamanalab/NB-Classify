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

#ifndef NBC_SECUREKEY_FILEMANAGER
#define NBC_SECUREKEY_FILEMANAGER

#include <memory>

namespace nbc_share
{

/**
 * @brief Manages secure key files. (public / secret key files)
 */
class SecureKeyFileManager
{
    static constexpr long DefFheM = 11119;
    static constexpr long DefFheL = 180;

    static constexpr long DefFheP = 2;
    static constexpr long DefFheR = 18;
    static constexpr long DefFheC = 3;
    static constexpr long DefFheW = 64;
    static constexpr long DefFheD = 0;

public:
    enum Kind_t : int32_t
    {
        kKindUnkonw  = -1,
        kKindPubKey  = 0,
        kKindSecKey  = 1,
        kKindContext = 2,
    };
    
public:
    SecureKeyFileManager(const std::string& pubkey_filename,
                         const std::string& seckey_filename,
                         const std::string& context_filename,
                         const long fheM = DefFheM,
                         const long fheL = DefFheL,
                         const long fheP = DefFheP,
                         const long fheR = DefFheR,
                         const long fheC = DefFheC,
                         const long fheW = DefFheW,
                         const long fheD = DefFheD);

    SecureKeyFileManager(const std::string& pubkey_filename,
                         const std::string& seckey_filename,
                         const std::string& context_filename,
                         const std::string& config_filename);
    
    ~SecureKeyFileManager(void) = default;

    void initialize(void);

    size_t size(const Kind_t kind) const;

    void data(const Kind_t kind, void* buffer);

    bool is_exist(const Kind_t kind) const;

    std::string filename(const Kind_t kind) const;

    long plain_mod(void) const;

private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};

} /* namespace nbc_share */

#endif /* NBC_SECUREKEY_FILEMANAGER */
