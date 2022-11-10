#include "instructions.hpp"

#include <cassert>
#include <stdexcept>
#include <string>

#include "../byterun/lib.h"

const std::map<size_t, std::string> codesWithParameters{{0, "Binop"},
                                                        {1, "Const"},
                                                        {2, "String"},
                                                        {3, "Sexp"},
                                                        //   {4, "Sti"},
                                                        //   {5, "Sta"},
                                                        {6, "Jmp"},
                                                        //   {7, "End"},
                                                        //   {8, "Ret"},
                                                        //   {9, "Drop"},
                                                        //   {10, "Dup"},
                                                        //   {11, "Swap"},
                                                        //   {12, "Elem"},
                                                        {13, "Ld"},
                                                        {14, "Lda"},
                                                        {15, "St"},
                                                        {16, "CJmp"},
                                                        {17, "Begin"},
                                                        {18, "CBegin"},
                                                        {19, "Callc"},
                                                        {20, "Call"},
                                                        {21, "Tag"},
                                                        {22, "Array"},
                                                        {23, "Fail"},
                                                        {24, "Line"},
                                                        //   {25, "Patt"},
                                                        {26, "RuntimeCall"}};

ByteCode convert(bytefile const * bf) {
    std::vector<Instr> res;

#define INT (ip += sizeof(int), *(int *)(ip - sizeof(int)))
#define SIZE (size_t(INT))
#define LABEL (size_t(INT))
#define BYTE *ip++
#define STRING get_string(bf, INT)
#define FAIL(line) assert(0 && line)
#define Q(x)              \
    {                     \
        res.push_back(x); \
        break;            \
    }

    char * ip = bf->code_ptr;
    Operator ops[] = {Operator::PLUS, Operator::MINUS, Operator::MULT,
                      Operator::DIV,  Operator::MOD,   Operator::LT,
                      Operator::LE,   Operator::GT,    Operator::GE,
                      Operator::EQ,   Operator::NEQ,   Operator::OR,
                      Operator::AND};
    std::string pats[] = {"=str", "#string", "#array", "#sexp",
                          "#ref", "#val",    "#fun"};

    do {
        char x = BYTE, h = (x & 0xF0) >> 4, l = x & 0x0F;
        switch (h) {
            case 15: {
                return res;
            }
            case 0:
                Q(Binop{ops[l - 1]})
            case 1:
                switch (l) {
                    case 0:
                        Q(Const{INT})
                    case 1:
                        Q(String{STRING})
                    case 2:
                        Q((Sexp{STRING, SIZE}))
                    case 3:
                        Q(Sti{})
                    case 4:
                        Q(Sta{})
                    case 5:
                        Q(Jmp{LABEL})
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
                Mem mem{};
                switch (l) {
                    case 0:
                        mem = Mem::G;
                        break;
                    case 1:
                        mem = Mem::L;
                        break;
                    case 2:
                        mem = Mem::A;
                        break;
                    case 3:
                        mem = Mem::C;
                        break;
                    default:
                        FAIL(__LINE__);
                }
                switch (h) {
                    case 2:
                        Q((Ld{mem, INT}))
                    case 3:
                        Q((Lda{mem, INT}))
                    case 4:
                        Q((St{mem, INT}))
                    default:
                        FAIL(__LINE__);
                }
                break;
            }
            case 5:
                switch (l) {
                    case 0:
                        Q((CJmp{false, LABEL}))
                    case 1:
                        Q((CJmp{true, LABEL}))
                    case 2:
                        Q((Begin{INT, INT}))
                    case 3:
                        Q((CBegin{INT, INT}))
                    case 4:
                        FAIL(__LINE__);  // TODO
                    case 5:
                        Q(Callc{INT})
                    case 6:
                        Q((Call{INT, INT}))
                    case 7:
                        Q((Tag{STRING, INT}))
                    case 8:
                        Q(Array{SIZE})
                    case 9:
                        Q((Fail{INT, INT}))
                    case 10:
                        Q(Line{INT})
                    default:
                        FAIL(__LINE__);
                }
                break;
            case 6:
                Q(Patt{})
            case 7:
                switch (l) {
                    case 0:
                        Q(RuntimeCall{CallRead{}})
                    case 1:
                        Q(RuntimeCall{CallWrite{}})
                    case 2:
                        Q(RuntimeCall(CallLength{}))
                    case 3:
                        Q(RuntimeCall{CallString{}})
                    case 4:
                        Q(RuntimeCall{CallArray{SIZE}})
                    default:
                        FAIL(__LINE__);
                }
                break;
            default:
                FAIL(__LINE__);
        }
    } while (true);
    return res;
}
