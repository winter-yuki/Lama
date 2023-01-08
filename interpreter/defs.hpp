#ifndef __DEFS_INCLUDE__
#define __DEFS_INCLUDE__

#include <cstddef>

using Word = size_t;

using Literal = int;

/**
 * @brief Code label in distributed bytecode format.
 */
using Label = size_t;

/**
 * @brief Instruction Pointer.
 */
using Ip = size_t;

using Ref = void *;

/**
 * @brief INTerpreter DeBuG.
 */
// #define INTDBG

#endif  // __DEFS_INCLUDE__
