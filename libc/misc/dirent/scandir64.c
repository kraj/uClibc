/* -*- Mode: C; c-file-style: "gnu" -*- */
/*
   Copyright (c) 2000 Petter Reinholdtsen

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <features.h>
#ifdef __UCLIBC_HAVE_LFS__
#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS != 64 
#undef _FILE_OFFSET_BITS
#define	_FILE_OFFSET_BITS   64
#endif
#ifndef __USE_LARGEFILE64
# define __USE_LARGEFILE64	1
#endif
/* We absolutely do _NOT_ want interfaces silently
 * renamed under us or very bad things will happen... */
#ifdef __USE_FILE_OFFSET64
# undef __USE_FILE_OFFSET64
#endif
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dirstream.h"


int scandir64(const char *dir, struct dirent64 ***namelist,
			 int (*selector) (const struct dirent64 *),
			 int (*compar) (const __ptr_t, const __ptr_t))
{
    DIR *d = opendir(dir);
    struct dirent64 *current;
    struct dirent64 **names;
    int count = 0;
    int pos = 0;
    int result = -1;

    if (NULL == d)
        return -1;

    while (NULL != readdir64(d))
        count++;

    if (!(names = malloc(sizeof (struct dirent64 *) * count))) {
	closedir(d);
	return -1;
    }

    rewinddir(d);

    while (NULL != (current = readdir64(d))) {
        if (NULL == selector || selector(current)) {
            struct dirent64 *copyentry = malloc(current->d_reclen);

            memcpy(copyentry, current, current->d_reclen);

            names[pos] = copyentry;
            pos++;
        }
    }
    result = closedir(d);

    if (pos != count) {
	struct dirent64 **tmp;
	if (!(tmp = realloc(names, sizeof (struct dirent64 *) * pos))) {
	    free(names);
	    return -1;
	}
	names = tmp;
    }


    if (compar != NULL) {
	qsort(names, pos, sizeof (struct dirent64 *), compar);
    }

    *namelist = names;

    return pos;
}
#endif /* __UCLIBC_HAVE_LFS__ */

