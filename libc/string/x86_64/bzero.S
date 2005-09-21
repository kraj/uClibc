#define memset __bzero
#include "memset.S"
weak_alias (__bzero, bzero)
