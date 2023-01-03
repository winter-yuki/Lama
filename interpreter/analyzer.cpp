#include "analyzer.hpp"

#include <algorithm>
#include <iostream>
#include <map>

#include "../byterun/lib.h"
#include "instructions.hpp"

void analyze(std::string const & filename) {
    bytefile const * bf = read_file(const_cast<char *>(filename.c_str()));
    const auto instrs = ins::convert(bf);
    std::map<std::string, size_t> frequencies;
    for (const auto instr : instrs) {
        const auto code = ins::codesWithParameters.find(instr.index());
        if (code == ins::codesWithParameters.cend()) continue;
        const auto name = code->second;
        auto search = frequencies.find(name);
        if (search == frequencies.end()) {
            frequencies.insert({name, 1});
        } else {
            frequencies.insert_or_assign(name, search->second + 1);
        }
    }
    std::vector<std::pair<std::string, size_t>> fs(frequencies.begin(),
                                                   frequencies.end());
    std::sort(fs.begin(), fs.end(),
              [](auto x, auto y) { return x.second > y.second; });
    for (const auto & f : fs) {
        std::cout << f.first << "\t: " << f.second << std::endl;
    }
}
