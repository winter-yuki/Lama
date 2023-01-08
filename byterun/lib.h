#ifndef __LAMA_BYTERUN__
#define __LAMA_BYTERUN__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/* The unpacked representation of bytecode file */
typedef struct {
  char *string_ptr;              /* A pointer to the beginning of the string table */
  int  *public_ptr;              /* A pointer to the beginning of publics table    */
  char *code_ptr;                /* A pointer to the bytecode itself               */
  int  *global_ptr;              /* A pointer to the global area                   */
  int   stringtab_size;          /* The size (in bytes) of the string table        */
  int   global_area_size;        /* The size (in words) of global area             */
  int   public_symbols_number;   /* The number of public symbols                   */
  char  buffer[0];
} bytefile;

/* Gets a string from a string table by an index */
char* get_string (bytefile const *f, int pos);

/* Gets a name for a public symbol */
char* get_public_name (bytefile *f, int i);

/* Gets an offset for a publie symbol */
int get_public_offset (bytefile *f, int i);

bytefile* read_file (char *fname);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LAMA_BYTERUN__
