#include "interpreter.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

#include "../byterun/lib.h"
#include "instructions.hpp"

namespace {

// TODO dynamic memory
// TODO gc
class Memory final {
   public:
    void pushOp(ins::Literal value) {
        // TODO
    }

    void pushOp(ins::Literal * ref) {
        // TODO
    }
};

}  // namespace

void interpret(std::string const & filename) {
    bytefile const * bf = read_file(const_cast<char *>(filename.c_str()));
    const auto instrs = ins::convert(bf);
    Memory memory;
    for (auto const & instr : instrs) {
        switch (instr.index()) {
            case ins::id<ins::Drop>(): {
                const auto op = std::get<ins::Drop>(instr);
                break;
            }

            case ins::id<ins::Dup>(): {
                const auto op = std::get<ins::Dup>(instr);
                break;
            }

            case ins::id<ins::Swap>(): {
                const auto op = std::get<ins::Swap>(instr);
                break;
            }

            case ins::id<ins::Binop>(): {
                const auto op = std::get<ins::Binop>(instr);
                break;
            }

            case ins::id<ins::Const>(): {
                const auto op = std::get<ins::Const>(instr);
                break;
            }

            case ins::id<ins::String>(): {
                const auto op = std::get<ins::String>(instr);
                break;
            }

            case ins::id<ins::Ld>(): {
                const auto op = std::get<ins::Ld>(instr);
                break;
            }

            case ins::id<ins::Lda>(): {
                const auto op = std::get<ins::Lda>(instr);
                break;
            }

            case ins::id<ins::St>(): {
                const auto op = std::get<ins::St>(instr);
                break;
            }

            case ins::id<ins::Sti>(): {
                const auto op = std::get<ins::Sti>(instr);
                break;
            }

            case ins::id<ins::CJmp>(): {
                const auto op = std::get<ins::CJmp>(instr);
                break;
            }

            case ins::id<ins::Jmp>(): {
                const auto op = std::get<ins::Jmp>(instr);
                break;
            }

            case ins::id<ins::Begin>(): {
                const auto op = std::get<ins::Begin>(instr);
                break;
            }

            case ins::id<ins::End>(): {
                const auto op = std::get<ins::End>(instr);
                break;
            }

            case ins::id<ins::Call>(): {
                const auto op = std::get<ins::Call>(instr);
                break;
            }

            case ins::id<ins::Tag>(): {
                const auto op = std::get<ins::Tag>(instr);
                break;
            }

            case ins::id<ins::Array>(): {
                const auto op = std::get<ins::Array>(instr);
                break;
            }

            case ins::id<ins::Sta>(): {
                const auto op = std::get<ins::Sta>(instr);
                break;
            }

            case ins::id<ins::Elem>(): {
                const auto op = std::get<ins::Elem>(instr);
                break;
            }

            case ins::id<ins::Sexp>(): {
                const auto op = std::get<ins::Sexp>(instr);
                break;
            }

            case ins::id<ins::Line>():
                // Do nothing
                break;

            case ins::id<ins::Patt>(): {
                const auto op = std::get<ins::Patt>(instr);
                break;
            }

            case ins::id<ins::RuntimeCall>(): {
                const auto op = std::get<ins::RuntimeCall>(instr);
                break;
            }

            default:
                break;
        }
    }
}
