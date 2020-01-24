#include <numeric>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <nbc_share/nbc_permvec.hpp>

namespace nbc_share
{

void PermVec::gen_permvec(const size_t class_num)
{
    vec_.clear();
    vec_.resize(class_num);
    std::iota(vec_.begin(), vec_.end(), 0);

    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::shuffle(vec_.begin(), vec_.end(), engine);
}

void PermVec::load_from_csvfile(const std::string& filename)
{
    std::ifstream ifs(filename);
    std::string line;

    vec_.clear();
    
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        int num;

        while (ss >> num){
            vec_.push_back(num);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
    }
}

} /* namespace nbc_share */
