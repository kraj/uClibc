
#include <stdlib.h>

#ifdef __UCLIBC_HAS_FLOATS__
#define MAX_NDIGIT 17
char * gcvt(double number, size_t ndigit, char* buf)
{
    sprintf(buf, "%.*g", (ndigit > MAX_NDIGIT)? MAX_NDIGIT : ndigit, number);
    return buf;
}
#endif
