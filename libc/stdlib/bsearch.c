/*
 * This file originally lifted in toto from 'Dlibs' on the atari ST  (RdeBath)
 *
 * 
 *    Dale Schumacher                         399 Beacon Ave.
 *    (alias: Dalnefre')                      St. Paul, MN  55104
 *    dal@syntel.UUCP                         United States of America
 *  "It's not reality that's important, but how you perceive things."
 *
 *  Reworked by Erik Andersen <andersen@uclibc.org>
 */
#include <stdio.h>

void * bsearch (const void *key, const void *base, size_t num, size_t size,
	         int (*cmp) (const void *, const void *))
{
    int dir;
    size_t a, b, c;
    const void *p;

    a = 0;
    b = num;
    while (a < b)
    {
	c = (a + b) >> 1;		/* == ((a + b) / 2) */
	p = (void *)(((const char *) base) + (c * size));
	dir = (*cmp)(key, p);
	if (dir < 0) {
	    b = c;
	} else if (dir > 0) {
	    a = c + 1;
	} else {
	    return (void *)p;
	}
    }

    return NULL;
}

