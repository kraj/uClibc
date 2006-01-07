#define USE_AS_STPCPY
#define STRCPY __stpcpy
#include "strcpy.S"
strong_alias(__stpcpy,stpcpy)
