# ifndef __LAMA_RUNTIME__
# define __LAMA_RUNTIME__

# ifndef __cplusplus

# include <stdio.h>
# include <stdio.h>
# include <string.h>
# include <stdarg.h>
# include <stdlib.h>
# include <sys/mman.h>
# include <assert.h>
# include <errno.h>
# include <regex.h>
# include <time.h>
# include <limits.h>
# include <ctype.h>

# endif // __cplusplus

# ifdef __cplusplus
extern "C"
{
# endif // __cplusplus

# define UNBOXED(x)  (((int) (x)) &  0x0001)
# define UNBOX(x)    (((int) (x)) >> 1)
# define BOX(x)      ((((int) (x)) << 1) | 0x0001)

# define STRING_TAG  0x00000001
# define ARRAY_TAG   0x00000003
# define SEXP_TAG    0x00000005
# define CLOSURE_TAG 0x00000007
# define UNBOXED_TAG 0x00000009 // Not actually a tag; used to return from LkindOf

# define WORD_SIZE (CHAR_BIT * sizeof(int))

void failure (char *s, ...);
void printValue (void *p);

int Lread ();
int Lwrite (int n);

/**
 * @brief Check kind of the value.
 *
 * @param p
 * @return value kind (Sexp, array, unboxed...)
 */
int LkindOf (void *p);

/**
 * @brief Creates array of given length.
 *
 * @param length
 * @return created array
 */
void* LmakeArray (int length);

/**
 * @brief Accesses value by index in the array-like structure.
 *
 * @param p
 * @param i
 * @return value on required position, not a pointer!
 */
void* Belem (void *p, int i);

/**
 * @brief Assigns value to the element of the array-like structure.
 *
 * @param v value
 * @param i position index
 * @param x structure
 * @return assigned value
 */
void* Bsta (void *v, int i, void *x);

/**
 * @brief Determines length of the array-like structure.
 *
 * @param p pointer to the structure
 * @return length
 */
int Llength (void *p);

/**
 * @brief Creates LAMA array-like string from C-string.
 *
 * @param p pointer to C-string
 * @return created string
 */
void* Bstring (void *p);

/**
 * @brief Create S-expression with the given tag and number of arguments.
 *
 * @param tag
 * @param bn number of values (one stands for tag as one of them)
 * @return created S-expression
 */
void* Bsexp1 (int tag, int bn);

/**
 * @brief Checks if S-expression satisfies requirements.
 *
 * @param d data - S-expression
 * @param t tag
 * @param n number of fields (values + tag)
 * @return boolean: if data satisfies tag and length
 */
int Btag (void *d, int t, int n);

/**
 * @brief Calculates string hash to use as S-expression tag.
 *
 * @return Hash of the string.
 */
int LtagHash (char*);

# ifdef __cplusplus
}
# endif // __cplusplus

# endif
