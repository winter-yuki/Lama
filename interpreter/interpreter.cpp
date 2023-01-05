#include "interpreter.hpp"

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "../byterun/lib.h"
#include "../runtime/runtime.h"
#include "defs.hpp"
#include "instructions.hpp"

// TODO dynamic memory
// TODO gc
namespace {

class OperandsStack final {
   public:
    void push(Literal value) { stack.at(sp++) = Word(value); }

    void push(Ref ref) { stack.at(sp++) = Word(ref); }

    void push(Word value) { stack.at(sp++) = Word(value); }

    Literal topLiteral() const { return Literal(stack.at(sp - 1)); }

    Ref topRef() const { return Ref(stack.at(sp - 1)); }

    Word top() const { return Word(stack.at(sp - 1)); }

    Literal popLiteral() { return Literal(stack.at(--sp)); }

    Ref popRef() { return Ref(stack.at(--sp)); }

    Word pop() { return Word(stack.at(--sp)); }

    size_t size() const { return sp; }

   private:
    size_t sp = 0;  // stack pointer
    std::vector<Word> stack = std::vector<Word>(10000);
};

class Frame final {
   public:
    explicit Frame(size_t nArgs, size_t nLocals) noexcept
        : args(nArgs), locals(nLocals) {}

    Word & arg(size_t index) { return args.at(index); }
    Word & local(size_t index) { return locals.at(index); }

   private:
    std::vector<Word> args;
    std::vector<Word> locals;
};

}  // namespace

void interpret(std::string const & filename) {
    bytefile const * bf = read_file(const_cast<char *>(filename.c_str()));
    const auto instrs = ins::convert(bf);
    OperandsStack stack;
    std::vector<Ip> retIps;
    std::vector<Frame> frames;
    frames.reserve(10000);
    std::vector<Word> globals(bf->global_area_size);

    auto memRef = [&](ins::Location loc) -> Word & {
        switch (loc.index()) {
            case ins::loc::id<ins::loc::Global>(): {
                const auto global = std::get<ins::loc::Global>(loc);
                return globals.at(global.index);
            }

            case ins::loc::id<ins::loc::Local>(): {
                const auto local = std::get<ins::loc::Local>(loc);
                return frames.back().local(local.index);
            }

            case ins::loc::id<ins::loc::Arg>(): {
                const auto arg = std::get<ins::loc::Arg>(loc);
                return frames.back().arg(arg.index);
            }

            case ins::loc::id<ins::loc::Const>(): {
                assert(false && "Reference to const memory is not available");
            }

            default:
                assert(false && "Unknown location");
        }
    };

    auto mem = [&](ins::Location loc) -> Word {
        switch (loc.index()) {
            case ins::loc::id<ins::loc::Const>(): {
                const auto c = std::get<ins::loc::Const>(loc);
                return c.value;
            }

            default:
                return memRef(loc);
        }
    };

    Ip ip = 0;
    while (ip < instrs.size()) {
        const auto & instr = instrs.at(ip++);
#ifdef INTDBG
        std::cout << "Evaluating: "
                  << ins::instrsInfo.find(instr.index())->second.name
                  << "\ton op stack size = " << stack.size() << std::endl;
#endif  // INTDBG
        switch (instr.index()) {
            case ins::id<ins::Drop>(): {
                const auto op = std::get<ins::Drop>(instr);
                stack.pop();
                break;
            }

            case ins::id<ins::Dup>(): {
                const auto op = std::get<ins::Dup>(instr);
                const auto top = stack.top();
                stack.push(top);
                break;
            }

            case ins::id<ins::Swap>(): {
                const auto op = std::get<ins::Swap>(instr);
                const auto fst = stack.pop();
                const auto snd = stack.pop();
                stack.push(fst);
                stack.push(snd);
                break;
            }

            case ins::id<ins::Binop>(): {
                const auto op = std::get<ins::Binop>(instr);
                const auto y = UNBOX(stack.popLiteral());
                const auto x = UNBOX(stack.popLiteral());
#ifdef INTDBG
                std::cout << "x = " << x << "; y = " << y << std::endl;
#endif  // INTDBG
                switch (op.op) {
                    case ins::Operator::PLUS:
                        stack.push(BOX(x + y));
                        break;

                    case ins::Operator::MINUS:
                        stack.push(BOX(x - y));
                        break;

                    case ins::Operator::MULT:
                        stack.push(BOX(x * y));
                        break;

                    case ins::Operator::DIV:
                        stack.push(BOX(x / y));
                        break;

                    case ins::Operator::MOD:
                        stack.push(BOX(x % y));
                        break;

                    case ins::Operator::LT:
                        stack.push(BOX(x < y));
                        break;

                    case ins::Operator::LE:
                        stack.push(BOX(x <= y));
                        break;

                    case ins::Operator::GT:
                        stack.push(BOX(x > y));
                        break;

                    case ins::Operator::GE:
                        stack.push(BOX(x >= y));
                        break;

                    case ins::Operator::EQ:
                        stack.push(BOX(x == y));
                        break;

                    case ins::Operator::NEQ:
                        stack.push(BOX(x != y));
                        break;

                    case ins::Operator::OR:
                        stack.push(BOX(x || y));
                        break;

                    case ins::Operator::AND:
                        stack.push(BOX(x && y));
                        break;

                    default:
                        break;
                }
                break;
            }

            case ins::id<ins::Const>(): {
                const auto op = std::get<ins::Const>(instr);
                stack.push(op.x);
                break;
            }

            case ins::id<ins::String>(): {
                const auto op = std::get<ins::String>(instr);
                // TODO
                break;
            }

            case ins::id<ins::Ld>(): {
                const auto op = std::get<ins::Ld>(instr);
                stack.push(mem(op.loc));
                break;
            }

            case ins::id<ins::Lda>(): {
                const auto op = std::get<ins::Lda>(instr);
                stack.push(&memRef(op.loc));
                break;
            }

            case ins::id<ins::St>(): {
                const auto op = std::get<ins::St>(instr);
                memRef(op.loc) = stack.top();
                break;
            }

            case ins::id<ins::Sti>(): {
                const auto op = std::get<ins::Sti>(instr);
                const auto value = stack.popLiteral();
                const auto ref = stack.popRef();
                *(Literal *)ref = value;
                stack.push(value);
                break;
            }

            case ins::id<ins::CJmp>(): {
                const auto op = std::get<ins::CJmp>(instr);
                const auto top = stack.pop();
                if (op.onNonZero && top != 0 || !op.onNonZero && top == 0) {
                    ip = op.ip;
                }
                break;
            }

            case ins::id<ins::Jmp>(): {
                const auto op = std::get<ins::Jmp>(instr);
                ip = op.ip;
                break;
            }

            case ins::id<ins::Begin>(): {
                const auto op = std::get<ins::Begin>(instr);
                auto frame = Frame(op.nArgs, op.nLocals);
                frames.push_back(std::move(frame));
                // Is not main
                if (!retIps.empty()) {
                    for (size_t iArg = 0; iArg < op.nArgs; ++iArg) {
                        frames.back().arg(iArg) = stack.pop();
                    }
                }
                break;
            }

            case ins::id<ins::End>(): {
                const auto op = std::get<ins::End>(instr);
                frames.pop_back();
                if (retIps.empty()) {
                    ip = instrs.size();
                } else {
                    ip = retIps.back();
                    retIps.pop_back();
                }
                break;
            }

            case ins::id<ins::Call>(): {
                const auto op = std::get<ins::Call>(instr);
                retIps.push_back(ip + 1);
                ip = op.ip;
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
                switch (op.index()) {
                    case ins::rt::id<ins::rt::CallRead>(): {
                        stack.push(Lread());
                        break;
                    }

                    case ins::rt::id<ins::rt::CallWrite>(): {
                        const auto res = Lwrite(stack.popLiteral());
                        stack.push(res);
                        break;
                    }

                    default:
                        assert(false && "Unknown runtime call");
                }
                break;
            }

            default:
                std::cout << "Unknown bytecode: "
                          << ins::instrsInfo.find(instr.index())->second.name
                          << std::endl;
                assert(false && "Unknown bytecode");
        }
    }
}
