#include <string.h>
#include "dirstream.h"

int alphasort(const __ptr_t __e1, const __ptr_t __e2)
{
    const struct dirent *a = __e1;
    const struct dirent *b = __e2;
    return (strcmp(a->d_name, b->d_name));
}

