#ifndef _BITS_KERNEL_TYPES_H
#define _BITS_KERNEL_TYPES_H

/* Sigh.  We need to carefully wrap this one... */
#warning You really should include a proper bits/kernel_types.h for your architecture 

#ifndef __GLIBC__
#define __GLIBC__ 2
#include <asm/posix_types.h>
#undef __GLIBC__
#else
#include <asm/posix_types.h>
#endif


#endif /* _BITS_KERNEL_TYPES_H */
