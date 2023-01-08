#include "instructions.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../byterun/lib.h"
#include "../runtime/runtime.h"

namespace ins {

#define INFO(type, name, hasParams)               \
    {                                             \
        id<type>(), InstrInfo { name, hasParams } \
    }

const std::map<size_t, InstrInfo> instrsInfo{
    INFO(Drop, "Drop", false),      INFO(Dup, "Dup", false),
    INFO(Swap, "Swap", false),      INFO(Binop, "Binop", false),
    INFO(Const, "Const", true),     INFO(String, "String", true),
    INFO(Ld, "Ld", true),           INFO(Lda, "Lda", true),
    INFO(St, "St", true),           INFO(Sti, "Sti", false),
    INFO(RawCJmp, "RawCJmp", true), INFO(CJmp, "CJmp", true),
    INFO(RawJmp, "RawJmp", true),   INFO(Jmp, "Jmp", true),
    INFO(Begin, "Begin", true),     INFO(CBegin, "CBegin", true),
    INFO(End, "End", false),        INFO(Ret, "Ret", false),
    INFO(Callc, "Callc", true),     INFO(RawCall, "RawCall", true),
    INFO(Call, "Call", true),       INFO(Tag, "Tag", true),
    INFO(Array, "Array", true),     INFO(Sta, "Sta", false),
    INFO(Elem, "Elem", false),      INFO(Sexp, "Sexp", true),
    INFO(Fail, "Fail", true),       INFO(Line, "Line", true),
    INFO(Patt, "Patt", true),       INFO(RuntimeCall, "RuntimeCall", true)};

ByteCode convert(bytefile const * bf) {
    std::vector<Instr> res;
    std::unordered_map<Label, Ip> labelTranslationTable;
    char * ip = bf->code_ptr;

#define INT (ip += sizeof(int), *(int *)(ip - sizeof(int)))
#define SIZE (size_t(INT))
#define CONST (BOX(INT))
#define INDEX (size_t(INT))
#define LABEL (size_t(INT))

#define BYTE *ip++
#define STRING get_string(bf, INT)
#define FAIL(line) assert(0 && line)

#define UPDATE_TABLE                                       \
    {                                                      \
        const auto label = Label(ip - bf->code_ptr - 1);   \
        labelTranslationTable.insert({label, res.size()}); \
    }

#define Q(x)              \
    {                     \
        UPDATE_TABLE      \
        res.push_back(x); \
        break;            \
    }

    Operator ops[] = {Operator::PLUS, Operator::MINUS, Operator::MULT,
                      Operator::DIV,  Operator::MOD,   Operator::LT,
                      Operator::LE,   Operator::GT,    Operator::GE,
                      Operator::EQ,   Operator::NEQ,   Operator::OR,
                      Operator::AND};
    std::string pats[] = {"=str", "#string", "#array", "#sexp",
                          "#ref", "#val",    "#fun"};

    bool stop = false;
    do {
        char x = BYTE, h = (x & 0xF0) >> 4, l = x & 0x0F;
        switch (h) {
            case 15: {
                stop = true;
                break;
            }
            case 0:
                Q(Binop{ops[l - 1]})
            case 1:
                switch (l) {
                    case 0:
                        Q(Const{CONST})
                    case 1:
                        Q(String{STRING})
                    case 2:
                        Q((Sexp{STRING, SIZE}))
                    case 3:
                        Q(Sti{})
                    case 4:
                        Q(Sta{})
                    case 5:
                        Q(RawJmp{LABEL})
                    case 6:
                        Q(End{})
                    case 7:
                        Q(Ret{})
                    case 8:
                        Q(Drop{})
                    case 9:
                        Q(Dup{})
                    case 10:
                        Q(Swap{})
                    case 11:
                        Q(Elem{})
                    default:
                        FAIL(__LINE__);
                }
                break;
            case 2:
            case 3:
            case 4: {
                UPDATE_TABLE;
                Location loc;
                switch (l) {
                    case 0:
                        loc.emplace<loc::Global>(INDEX);
                        break;
                    case 1:
                        loc.emplace<loc::Local>(INDEX);
                        break;
                    case 2:
                        loc.emplace<loc::Arg>(INDEX);
                        break;
                    case 3:
                        loc.emplace<loc::Const>(CONST);
                        break;
                    default:
                        FAIL(__LINE__);
                }
                switch (h) {
                    case 2:
                        Q((Ld{loc}))
                    case 3:
                        Q((Lda{loc}))
                    case 4:
                        Q((St{loc}))
                    default:
                        FAIL(__LINE__);
                }
                break;
            }
            case 5:
                switch (l) {
                    case 0:
                        Q((RawCJmp{false, LABEL}))
                    case 1:
                        Q((RawCJmp{true, LABEL}))
                    case 2:
                        Q((Begin{SIZE, SIZE}))
                    case 3:
                        Q((CBegin{SIZE, SIZE}))
                    case 4:
                        FAIL(__LINE__);  // closure
                    case 5:
                        Q(Callc{INT})
                    case 6:
                        Q((RawCall{LABEL, SIZE}))
                    case 7:
                        Q((Tag{STRING, SIZE}))
                    case 8:
                        Q(Array{SIZE})
                    case 9:
                        Q((Fail{INT, INT}))
                    case 10:
                        Q(Line{SIZE})
                    default:
                        FAIL(__LINE__);
                }
                break;
            case 6:
                Q(Patt{})  // TODO
            case 7:
                switch (l) {
                    case 0:
                        Q(RuntimeCall{rt::CallRead{}})
                    case 1:
                        Q(RuntimeCall{rt::CallWrite{}})
                    case 2:
                        Q(RuntimeCall(rt::CallLength{}))
                    case 3:
                        Q(RuntimeCall{rt::CallString{}})
                    case 4:
                        Q(RuntimeCall{rt::CallArray{SIZE}})
                    default:
                        FAIL(__LINE__);
                }
                break;
            default:
                FAIL(__LINE__);
        }
    } while (!stop);

    for (auto & instr : res) {
        switch (instr.index()) {
            case ins::id<ins::RawCJmp>(): {
                const auto op = std::get<ins::RawCJmp>(instr);
                const auto search = labelTranslationTable.find(op.label);
                assert(search != labelTranslationTable.end() &&
                       "No ip for RawCJmp");
                instr.emplace<ins::CJmp>(op.onNonZero, search->second);
                break;
            }

            case ins::id<ins::RawJmp>(): {
                const auto op = std::get<ins::RawJmp>(instr);
                const auto search = labelTranslationTable.find(op.label);
                assert(search != labelTranslationTable.end() &&
                       "No ip for RawJmp");
                instr.emplace<ins::Jmp>(search->second);
                break;
            }

            case ins::id<ins::RawCall>(): {
                const auto op = std::get<ins::RawCall>(instr);
                const auto search = labelTranslationTable.find(op.label);
                assert(search != labelTranslationTable.end() &&
                       "No ip for RawCall");
                instr.emplace<ins::Call>(search->second, op.nArgs);
                break;
            }

            default:
                break;
        }
    }

    return res;
}

}  // namespace ins
