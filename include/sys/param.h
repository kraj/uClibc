/* Copyright (C) 1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#ifndef _PARAM_H
#define _PARAM_H

#include <features.h>
#include <limits.h>
#include <linux/limits.h>
#include <linux/param.h>

#include <sys/types.h>

#define MAXPATHLEN PATH_MAX

#ifndef NR_OPEN
#define NR_OPEN 32
#endif
#ifndef NR_FILE
#define NR_FILE 32
#endif

/* Number of Bits per BYte */
#define NBBY		CHAR_BIT

/* Bit map related macros.  */
#define        setbit(a,i)     ((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define        clrbit(a,i)     ((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define        isset(a,i)      ((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define        isclr(a,i)      (((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

/* Macros for counting and rounding.  */
#ifndef howmany
#define        howmany(x, y)   (((x)+((y)-1))/(y))
#endif
#define        roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#define powerof2(x)    ((((x)-1)&(x))==0)

/* Macros for min/max.  */
#define        MIN(a,b) (((a)<(b))?(a):(b))
#define        MAX(a,b) (((a)>(b))?(a):(b))


/* Unit of `st_blocks'.  */
#define DEV_BSIZE       512 

#endif /* _PARAM_H */
