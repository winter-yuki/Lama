#ifndef __INSTRUCTIONS_INCLUDE__
#define __INSTRUCTIONS_INCLUDE__

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "../byterun/lib.h"

using Literal = int;

enum class Operator {
    PLUS,
    MINUS,
    MULT,
    DIV,
    MOD,
    LT,
    LE,
    GT,
    GE,
    EQ,
    NEQ,
    OR,
    AND
};

struct Binop final {
    const Operator op;
};

struct Const final {
    const Literal x;
};

struct String final {
    const std::string s;
};

struct Sexp final {
    const std::string tag;
    const size_t n_args;
};

struct Sti final {};

struct Sta final {};

struct Jmp final {
    const size_t label;
};

struct End final {};

struct Ret final {};

struct Drop final {};

struct Dup final {};

struct Swap final {};

struct Elem final {};

enum class Mem { G, L, A, C };

struct Ld final {
    const Mem mem;
    const int n;
};

struct Lda final {
    const Mem mem;
    const int n;
};

struct St final {
    const Mem mem;
    const int n;
};

struct CJmp final {
    const bool onNonZero;
    const size_t label;
};

struct Begin final {
    const int n;  // TODO
    const int m;  // TODO
};

struct CBegin final {
    const int n;  // TODO
    const int m;  // TODO
};

struct Callc final {
    const int n;  // TODO
};

struct Call final {
    const int n;  // TODO
    const int m;  // TODO
};

struct Tag final {
    const std::string tag;
    const int value;  // TODO
};

struct Array final {
    const size_t size;
};

struct Fail final {
    const int n;  // TODO
    const int m;  // TODO
};

struct Line final {
    const int number;
};

// TODO
struct Patt final {};

struct CallRead final {};

struct CallWrite final {};

struct CallLength final {};

struct CallString final {};

struct CallArray final {
    const size_t size;
};

using RuntimeCall =
    std::variant<CallRead, CallWrite, CallLength, CallString, CallArray>;

using Instr =
    std::variant<Binop, Const, String, Sexp, Sti, Sta, Jmp, End, Ret, Drop, Dup,
                 Swap, Elem, Ld, Lda, St, CJmp, Begin, CBegin, Callc, Call, Tag,
                 Array, Fail, Line, Patt, RuntimeCall>;
using ByteCode = std::vector<Instr>;

ByteCode convert(bytefile const *);

extern const std::map<size_t, std::string> codesWithParameters;

#endif  // __INSTRUCTIONS_INCLUDE__
