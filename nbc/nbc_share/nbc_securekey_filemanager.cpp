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

#include <sstream>
#include <unordered_map>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_utility.hpp>
#include <nbc_share/nbc_securekey_filemanager.hpp>
#include <nbc_share/nbc_config.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

namespace nbc_share
{

struct SecureKeyFileManager::Impl
{
    Impl(const std::string& pubkey_filename,
         const std::string& seckey_filename,
         const std::string& context_filename,
         const long fheM,
         const long fheL,
         const long fheP,
         const long fheR,
         const long fheC,
         const long fheW,
         const long fheD)
        : m_(fheM),
          l_(fheL),
          p_(fheP),
          r_(fheR),
          c_(fheC),
          w_(fheW),
          d_(fheD)
    {
        filenames_.emplace(kKindPubKey,  pubkey_filename);
        filenames_.emplace(kKindSecKey,  seckey_filename);
        filenames_.emplace(kKindContext, context_filename);
    }
    
    Impl(const std::string& pubkey_filename,
         const std::string& seckey_filename,
         const std::string& context_filename,
         const std::string& config_filename)
    {
        Config conf;
        conf.load_from_file(config_filename);
#define READ(key, val) do {                                 \
            if (conf.is_exist_key(#key))                    \
                val = config_get_value<long>(conf, #key);   \
            STDSC_LOG_INFO("read fhe parameter. (%s: %ld)", \
                           #key, val);                      \
        } while(0)

        READ(fheM, m_);
        READ(fheL, l_);
        READ(fheP, p_);
        READ(fheR, r_);
        READ(fheC, c_);
        READ(fheW, w_);
        READ(fheD, d_);

#undef READ
        filenames_.emplace(kKindPubKey,  pubkey_filename);
        filenames_.emplace(kKindSecKey,  seckey_filename);
        filenames_.emplace(kKindContext, context_filename);
    }
    
    ~Impl(void) = default;

    void initialize(void)
    {
        helib::FHEcontext context(m_, p_, r_);
        helib::buildModChain(context, l_, c_);
        helib::FHESecKey secretKey(context);
        const helib::FHEPubKey& publicKey = secretKey;
        secretKey.GenSecKey(w_);
        helib::addSome1DMatrices(secretKey);
        STDSC_LOG_INFO("Generated the secret key for FHE.");
        
        double security = context.securityLevel();
        long slots = context.zMStar.getNSlots();

        std::ostringstream oss;
        oss << "slots=" << slots << ", m=" << m_ << ", p=" << p_
            << ", r=" << r_ << ", L=" << l_ << ", security=" << security
            << "PhiM: " << context.zMStar.getPhiM();
        STDSC_LOG_INFO(oss.str().c_str());

        std::ofstream contextfile(filename(kKindContext), std::ios::binary);
        helib::writeContextBaseBinary(contextfile, context);
        helib::writeContextBinary(contextfile, context);
        contextfile.close();

        std::ofstream pubkeyfile(filename(kKindPubKey), std::ios::binary);
        helib::writePubKeyBinary(pubkeyfile, publicKey);
        pubkeyfile.close();

        std::ofstream seckeyfile(filename(kKindSecKey), std::ios::binary);
        helib::writeSecKeyBinary(seckeyfile, secretKey);
        seckeyfile.close();
    }

    size_t size(const Kind_t kind) const
    {
        return nbc_share::utility::file_size(filename(kind));
    }
    
    void data(const Kind_t kind, void* buffer)
    {
        size_t sz = size(kind);
        std::ifstream ifs(filenames_[kind], std::ios::binary);
        if (!ifs.is_open())
        {
            std::ostringstream oss;
            oss << "failed to open. (" << filenames_[kind] << ")";
            STDSC_THROW_FILE(oss.str());
        }
        else
        {
            ifs.read(reinterpret_cast<char*>(buffer), sz);
            STDSC_LOG_DEBUG("read %s", filenames_[kind].c_str());
        }
    }
    
    bool is_exist(const Kind_t kind) const
    {
        std::ifstream ifs(filename(kind));
        return ifs.is_open();
    }

    std::string filename(const Kind_t kind) const
    {
        return filenames_.at(kind);
    }

    long plain_mod(void) const
    {
        return std::pow(p_, r_);
    }

private:
    std::unordered_map<Kind_t, std::string> filenames_;
    long m_;
    long l_;
    long p_;
    long r_;
    long c_;
    long w_;
    long d_;
};

SecureKeyFileManager::SecureKeyFileManager(const std::string& pubkey_filename,
                                           const std::string& seckey_filename,
                                           const std::string& context_filename,
                                           const long fheM,
                                           const long fheL,
                                           const long fheP,
                                           const long fheR,
                                           const long fheC,
                                           const long fheW,
                                           const long fheD)
  : pimpl_(new Impl(pubkey_filename,
                    seckey_filename,
                    context_filename,
                    fheM,
                    fheL,
                    fheP,
                    fheR,
                    fheC,
                    fheW,
                    fheD))
{
}

SecureKeyFileManager::SecureKeyFileManager(const std::string& pubkey_filename,
                                           const std::string& seckey_filename,
                                           const std::string& context_filename,
                                           const std::string& config_filename)
  : pimpl_(new Impl(pubkey_filename,
                    seckey_filename,
                    context_filename,
                    config_filename))
{
}

void SecureKeyFileManager::initialize(void)
{
    pimpl_->initialize();
}

size_t SecureKeyFileManager::size(const Kind_t kind) const
{
    return pimpl_->size(kind);
}

void SecureKeyFileManager::data(const Kind_t kind, void* buffer)
{
    pimpl_->data(kind, buffer);
}

bool SecureKeyFileManager::is_exist(const Kind_t kind) const
{
    return pimpl_->is_exist(kind);
}

std::string SecureKeyFileManager::filename(const Kind_t kind) const
{
    return pimpl_->filename(kind);
}

long SecureKeyFileManager::plain_mod(void) const
{
    return pimpl_->plain_mod();
}

} /* namespace nbc_share */
