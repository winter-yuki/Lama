#include "interpreter.hpp"

#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>

#include "../byterun/lib.h"
#include "instructions.hpp"

void interpret(std::string const & filename) {
    bytefile const * bf = read_file(const_cast<char *>(filename.c_str()));
    const auto instrs = convert(bf);
    // TODO prepare memory & gc
    // TODO run loop
}
