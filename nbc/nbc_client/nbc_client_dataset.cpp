#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <random>
#include <algorithm>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_client/nbc_client_dataset.hpp>

namespace nbc_client
{

static std::vector<std::vector<std::string>>
readData(const std::string& filename)
{
    std::vector<std::vector<std::string>> data;
    std::ifstream infile(filename);
    std::string line;

    if (!infile.is_open()) {
        std::ostringstream oss;
        oss << "Failed to open file. (" << filename << ")";
        STDSC_THROW_FILE(oss.str());
    }
    
    while (std::getline(infile, line)) {
        std::vector<std::string> temp;
        std::stringstream ss(line);
        std::string value;

        while (std::getline(ss, value, ',')) {
            temp.push_back(value);
        }
        
        temp.pop_back(); //pops out the class label
        data.push_back(temp);
    }
    infile.close();
    return data;
}

static std::vector<long>
parseData(const std::vector<std::string>& sample,
          const std::vector<std::vector<std::string>>& attr_values)
{
    std::vector<long> parsed;
    for (size_t i=0; i<sample.size(); i++) {
        for (size_t j=0; j< attr_values[i].size(); j++) {
            if (sample[i] == attr_values[i][j]) {
                parsed.push_back(1);
            } else {
                parsed.push_back(0);
            }
        }
    }

    return parsed;
}

Dataset::Dataset(const nbc_share::InfoFile& info)
    : info_(info)
{}

void Dataset::read(const std::string& filename)
{
    auto orig_data = readData(filename);
    STDSC_LOG_TRACE("read test data from %s.",
                    filename.c_str());
    
    data_.clear();
    for (const auto& d : orig_data) {
        std::vector<long> tmp = {1};
        auto tmp2 = parseData(d, info_.attr_values);
        tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
        data_.push_back(tmp);
    }
}
    
const std::vector<std::vector<long>>& Dataset::data(void) const
{
    return data_;
}

} /* namespace nbc_client */

