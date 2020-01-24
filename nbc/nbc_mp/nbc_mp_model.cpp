#include <iostream>
#include <fstream>
#include <sstream>
#include <stdsc/stdsc_log.hpp>
#include <stdsc/stdsc_exception.hpp>
#include <nbc_share/nbc_infofile.hpp>
#include <nbc_mp/nbc_mp_model.hpp>

namespace nbc_mp
{

Model::Model(const nbc_share::InfoFile& info)
    : info_(info)
{}

void Model::read(const std::string& filename)
{
    class_num_ = info_.class_names.size();
    feature_num_ = info_.attr_values.size();
    for (int i = 0; i<feature_num_; i++) {
        feature_value_num_.push_back(info_.attr_values[i].size());
    }
    
    std::ifstream infile(filename);
    std::string line;

    int num;
    int count = 0;
    int class_count = -1;
    
    while (getline(infile, line)) {
        std::stringstream ss(line);
        
        //Reads class probabilities into class_prob
        if (count == 0) {
            while (ss >> num) {
                std::vector<long> temp;
                temp.push_back(num);
                probs_.push_back(temp);
                if (ss.peek()== ',') {
                    ss.ignore();
                }
            }
        } else if (((feature_num_*class_count)+2+class_count <= count) &&
                   (count <= ((feature_num_*class_count)+1+class_count+feature_num_))) {
            while (ss >> num) {
                probs_[class_count].push_back(num);
                if (ss.peek()== ',') {
                    ss.ignore();
                }
            }
        } else {
            class_count += 1;
        }
        
        count += 1;
    }
}

void Model::print(void) const
{
    int class_num = probs_.size();
    
    std::cout << "Class probability" << std::endl;
    for (int i=0; i<class_num; i++) {
        std::cout << "Class " + std::to_string(i) + ": " << probs_[i][0] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Conditional probability" << std::endl;
    for (int i = 0; i<class_num; i++) {
        std::cout << std::endl;
        std::cout << "Class " + std::to_string(i) << std::endl;
        for (size_t j = 0; j<probs_[i].size(); j++) {
            std::cout << probs_[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
    
} /* namespace nbc_mp */

