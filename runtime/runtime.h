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

# define WORD_SIZE (CHAR_BIT * sizeof(int))

void failure (char *s, ...);

int Lread ();
int Lwrite (int n);

# ifdef __cplusplus
}
# endif // __cplusplus

# define UNBOXED(x)  (((int) (x)) &  0x0001)
# define UNBOX(x)    (((int) (x)) >> 1)
# define BOX(x)      ((((int) (x)) << 1) | 0x0001)

# endif
