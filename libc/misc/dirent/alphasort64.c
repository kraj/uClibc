#include <features.h>
#define _FILE_OFFSET_BITS   64
#define __USE_LARGEFILE64
#define __USE_FILE_OFFSET64
#include <string.h>
#include "dirstream.h"

#ifdef __UCLIBC_HAVE_LFS__

int alphasort64(const void * a, const void * b)
{
    return strcmp ((*(const struct dirent64 **) a)->d_name,
	    (*(const struct dirent64 **) b)->d_name);
}
#endif /* __UCLIBC_HAVE_LFS__ */

