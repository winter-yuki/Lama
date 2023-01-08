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
    explicit OperandsStack() {
        // Prevent reallocation
        stack.reserve(100000);
    }

    void push(Literal value) { stack.push_back(Word(value)); }

    void push(Ref ref) { stack.push_back(Word(ref)); }

    void push(Word value) { stack.push_back(Word(value)); }

    Literal topLiteral() const {
        assert(!stack.empty());
        return Literal(stack.back());
    }

    Ref topRef() const {
        assert(!stack.empty());
        return Ref(stack.back());
    }

    Word top() const {
        assert(!stack.empty());
        return Word(stack.back());
    }

    Literal popLiteral() {
        assert(!stack.empty());
        const auto top = topLiteral();
        stack.pop_back();
        return top;
    }

    Ref popRef() {
        assert(!stack.empty());
        const auto top = topRef();
        stack.pop_back();
        return top;
    }

    Word pop() {
        assert(!stack.empty());
        const auto top_ = top();
        stack.pop_back();
        return top_;
    }

    size_t size() const { return stack.size(); }

    void print() {
        for (auto x : stack) {
            if (UNBOXED(x)) {
                std::cout << UNBOX(x);
            } else {
                std::cout << "(0)";
                // std::cout << "(" << LkindOf((void *)x) << ")";
                // printValue((void *)x);
            }
            std::cout << " ";
        }
        std::cout << std::endl;
    }

   private:
    std::vector<Word> stack;
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
    std::vector<Word> globals(bf->global_area_size);

    auto memRef = [&](ins::Location loc) -> Word * {
        switch (loc.index()) {
            case ins::loc::id<ins::loc::Global>(): {
                const auto global = std::get<ins::loc::Global>(loc);
                return &globals.at(global.index);
            }

            case ins::loc::id<ins::loc::Local>(): {
                const auto local = std::get<ins::loc::Local>(loc);
                return &frames.back().local(local.index);
            }

            case ins::loc::id<ins::loc::Arg>(): {
                const auto arg = std::get<ins::loc::Arg>(loc);
                return &frames.back().arg(arg.index);
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
                return *memRef(loc);
        }
    };

    Ip ip = 0;
    while (ip < instrs.size()) {
        const auto & instr = instrs.at(ip++);
#ifdef INTDBG
        std::cout << " " << (ip - 1) << " : "
                  << ins::instrsInfo.find(instr.index())->second.name
                  << std::endl;
        std::cout << "\t";
        stack.print();
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
                std::cout << "\tx = " << x << "; y = " << y << std::endl;
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
                const auto res = Bstring((void *)op.s);
                stack.push(res);
                break;
            }

            case ins::id<ins::Ld>(): {
                const auto op = std::get<ins::Ld>(instr);
                stack.push(mem(op.loc));
                break;
            }

            case ins::id<ins::Lda>(): {
                const auto op = std::get<ins::Lda>(instr);
                stack.push(memRef(op.loc));
                break;
            }

            case ins::id<ins::St>(): {
                const auto op = std::get<ins::St>(instr);
                *memRef(op.loc) = stack.top();
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
                const auto top = UNBOX(stack.pop());
#ifdef INTDBG
                std::cout << "\tCJMP" << (op.onNonZero ? "nz" : "z") << ": "
                          << op.ip << "; top = " << top << std::endl;
#endif  // INTDBG
                if (op.onNonZero && top != 0 || !op.onNonZero && top == 0) {
                    ip = op.ip;
                }
                break;
            }

            case ins::id<ins::Jmp>(): {
                const auto op = std::get<ins::Jmp>(instr);
#ifdef INTDBG
                std::cout << "\tJMP"
                          << ": " << op.ip << std::endl;
#endif  // INTDBG
                ip = op.ip;
                break;
            }

            case ins::id<ins::Begin>(): {
                const auto op = std::get<ins::Begin>(instr);
                auto frame = Frame(op.nArgs, op.nLocals);
#ifdef INTDBG
                std::cout << "\tips"
                          << ": ";
                for (const auto ip : retIps) {
                    std::cout << ip << " ";
                }
                std::cout << std::endl;
#endif  // INTDBG
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
#ifdef INTDBG
                std::cout << "\tips"
                          << ": ";
                for (const auto ip : retIps) {
                    std::cout << ip << " ";
                }
                std::cout << std::endl;
#endif  // INTDBG
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
                retIps.push_back(ip);
                ip = op.ip;
                break;
            }

            case ins::id<ins::Tag>(): {
                const auto op = std::get<ins::Tag>(instr);
                const auto hash = LtagHash((char *)op.tag);
                const auto ref = stack.popRef();
                const auto res = Btag(ref, hash, BOX(op.nValues));
#ifdef INTDBG
                std::cout << "\thash = " << UNBOX(hash) << "; ref = " << ref
                          << "; res = " << UNBOX(res) << std::endl;
#endif  // INTDBG
                stack.push(res);
                break;
            }

            case ins::id<ins::Array>(): {
                const auto op = std::get<ins::Array>(instr);
                const auto arr = LmakeArray(BOX(op.size));
                for (size_t i = 0; i < op.size; ++i) {
                    Bsta(stack.popRef(), BOX(i), arr);
                }
                stack.push(arr);
                break;
            }

            case ins::id<ins::Sta>(): {
                const auto op = std::get<ins::Sta>(instr);
                const auto value = stack.popRef();
                const auto index = stack.pop();
                const auto arr = stack.popRef();
                const auto res = Bsta(value, index, arr);
                stack.push(res);
                break;
            }

            case ins::id<ins::Elem>(): {
                const auto op = std::get<ins::Elem>(instr);
                const auto index = stack.pop();
                const auto arr = stack.popRef();
                const auto res = Literal(Belem(arr, index));
                stack.push(res);
                break;
            }

            case ins::id<ins::Sexp>(): {
                const auto op = std::get<ins::Sexp>(instr);
                const auto hash = LtagHash((char *)op.tag);
                const auto sexp = Bsexp1(hash, BOX(op.nArgs + 1));
#ifdef INTDBG
                std::cout << "\tnArgs = " << op.nArgs << std::endl;
#endif  // INTDBG
                for (size_t i = 0; i < op.nArgs; ++i) {
                    Bsta(stack.popRef(), BOX(op.nArgs - i - 1), sexp);
                }
                stack.push(sexp);
                break;
            }

            case ins::id<ins::Line>():
                // Do nothing
                break;

            case ins::id<ins::RuntimeCall>(): {
                const auto op = std::get<ins::RuntimeCall>(instr);
                switch (op.index()) {
                    case ins::rt::id<ins::rt::CallRead>(): {
                        stack.push(Lread());
                        break;
                    }

                    case ins::rt::id<ins::rt::CallWrite>(): {
                        Lwrite(stack.popLiteral());
                        stack.push(BOX(0));  // TODO
                        break;
                    }

                    case ins::rt::id<ins::rt::CallLength>(): {
                        const auto res = Llength(stack.popRef());
                        stack.push(res);
                        break;
                    }

                    default:
                        std::cout << "Unknown runtime function: " << op.index()
                                  << std::endl;
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
