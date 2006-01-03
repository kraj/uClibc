#define __memcpy __mempcpy
#include "memcpy.S"
strong_alias(__mempcpy,mempcpy)
