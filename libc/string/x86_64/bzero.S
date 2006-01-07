#define __memset __bzero
#include "memset.S"
strong_alias(__bzero,bzero)
