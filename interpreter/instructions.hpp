#ifndef __INSTRUCTIONS_INCLUDE__
#define __INSTRUCTIONS_INCLUDE__

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "../byterun/lib.h"
#include "utils.hpp"

namespace ins {

using Literal = int;

using Label = size_t;

struct Drop final {};

struct Dup final {};

struct Swap final {};

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

/**
 * @brief Memory locations.
 */
namespace loc {

struct Global final {
    explicit Global(std::string && name) noexcept : name(std::move(name)) {}
    const std::string name;
};

struct Local final {
    explicit Local(size_t index) noexcept : index(index) {}
    const size_t index;
};

struct Arg final {
    explicit Arg(size_t index) noexcept : index(index) {}
    const size_t index;
};

struct Const final {
    explicit Const(Literal value) noexcept : value(value) {}
    const Literal value;
};

}  // namespace loc

using Location =
    std::variant<std::monostate, loc::Global, loc::Local, loc::Arg, loc::Const>;

/**
 * @brief LoaD value to operand stack from memory location.
 */
struct Ld final {
    const Location loc;
};

struct Lda final {
    const Location loc;
};

struct St final {
    const Location loc;
};

/**
 * @brief STore Indirect.
 *
 * Expects: (z:Ref (x):s).
 * Results: (z:s).
 * Effects: stores `z` to the `x` in state.
 */
struct Sti final {};

struct CJmp final {
    const bool onNonZero;
    const Label label;
};

struct Jmp final {
    const Label label;
};

struct Begin final {
    const size_t nArgs;
    const size_t nLocals;
};

/**
 * Not supported bytecode.
 */
struct CBegin final {
    const size_t nArgs;
    const size_t nLocals;
};

struct End final {};

/**
 * @brief Return.
 *
 * Not supported bytecode.
 */
struct Ret final {};

/**
 * Not supported bytecode.
 */
struct Callc final {
    const int dummy;
};

struct Call final {
    const std::string name;
    const size_t nArgs;
};

struct Tag final {
    const std::string tag;
    const size_t nValues;
};

struct Array final {
    const size_t size;
};

/**
 * @brief STore Array.
 *
 * Expects: (v:i:a:s).
 * Results: (v:s).
 * Effects: stores `v` to the array `a` by index `i`.
 */
struct Sta final {};

/**
 * @brief Element of array.
 *
 * Expects: (i:a:s).
 * Results: (v:s).
 */
struct Elem final {};

struct Sexp final {
    const std::string tag;
    const size_t nArgs;
};

/**
 * Not supported bytecode.
 */
struct Fail final {
    const int dummy1;
    const int dummy2;
};

struct Line final {
    const size_t number;
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

template <class Code>
constexpr size_t id() {
    return variant_index<Instr, Code>();
}

using ByteCode = std::vector<Instr>;

ByteCode convert(bytefile const *);

struct InstrInfo final {
    const std::string name;
    const bool hasParams;
};

extern const std::map<size_t, InstrInfo> instrsInfo;

}  // namespace ins

#endif  // __INSTRUCTIONS_INCLUDE__
