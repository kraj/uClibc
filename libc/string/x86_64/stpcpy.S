#define USE_AS_STPCPY
#define STRCPY __stpcpy

#include "strcpy.S"

weak_alias (__stpcpy, stpcpy)
