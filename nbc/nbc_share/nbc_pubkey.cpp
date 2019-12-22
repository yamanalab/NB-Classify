#include <fstream>
#include <stdsc/stdsc_exception.hpp>
#include <stdsc/stdsc_log.hpp>
#include <nbc_share/nbc_pubkey.hpp>
#include <nbc_share/nbc_utility.hpp>

#include <helib/FHE.h>
#include <helib/EncryptedArray.h>

namespace nbc_share
{

struct PubKey::Impl
{
    Impl(const helib::FHEcontext& context)
        : context_(context)
    {}

    void save_to_stream(std::ostream& os) const
    {
        helib::writePubKeyBinary(os, *data_);
    }
    
    void load_from_stream(std::istream& is)
    {
        data_ = std::make_shared<helib::FHEPubKey>(context_);
        STDSC_LOG_INFO("reading public key.");
        readPubKeyBinary(is, *data_);
    }
    
    void save_to_file(const std::string& filepath) const
    {
        std::ofstream ofs(filepath);
        save_to_stream(ofs);
        ofs.close();
    }
    
    void load_from_file(const std::string& filepath)
    {
        if (!nbc_share::utility::file_exist(filepath)) {
            std::ostringstream oss;
            oss << "File not found. (" << filepath << ")";
            STDSC_THROW_FILE(oss.str());
        }
        std::ifstream ifs(filepath, std::ios::binary);
        load_from_stream(ifs);
        ifs.close();
    }

    const helib::FHEPubKey& get(void) const
    {
        return *data_;
    }
    
private:
    const helib::FHEcontext& context_;
    std::shared_ptr<helib::FHEPubKey>  data_;
};

PubKey::PubKey(const helib::FHEcontext& context)
    : pimpl_(new Impl(context))
{}

void PubKey::save_to_stream(std::ostream& os) const
{
    pimpl_->save_to_stream(os);
}

void PubKey::load_from_stream(std::istream& is)
{
    pimpl_->load_from_stream(is);
}

void PubKey::save_to_file(const std::string& filepath) const
{
    pimpl_->save_to_file(filepath);
}
    
void PubKey::load_from_file(const std::string& filepath)
{
    pimpl_->load_from_file(filepath);
}

const helib::FHEPubKey& PubKey::get(void) const
{
    return pimpl_->get();
}
    
} /* namespace nbc_share */
