#include <iostream>
#include <fstream>
#include <sstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_infofile.hpp>

namespace nbc_share
{

void InfoFile::read(const std::string& filename)
{
    std::ifstream infile(filename);
    std::string line;
    int count = 0;
    num_features = 0;

    if (!infile.is_open()) {
        std::ostringstream oss;
        oss << "Failed to open file. (" << filename << ")";
        STDSC_THROW_FILE(oss.str());
    }

    while (std::getline(infile, line)) {
        std::stringstream ss(line);

        if (count == 0) {
            count++;
            while (ss.good()) {
                std::string substr;
                std::getline(ss, substr, ',');
                
                class_names.push_back(substr);
            }
        } else {
            std::vector<std::string> temp;
            while (ss.good()) {
                std::string substr;
                std::getline(ss, substr, ',');
                temp.push_back(substr);
            }
            
            num_features += temp.size();
            attr_values.push_back(temp);
            
        }
    }
    
    class_num = class_names.size();
}

} /* namespace nbc_share */

