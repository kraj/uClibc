#ifndef _BSWAP_H
#define	_BSWAP_H 1

#if !defined(__BYTE_ORDER) && defined(BYTE_ORDER)
#  define __BYTE_ORDER = BYTE_ORDER
#endif

#ifndef __BYTE_ORDER
#ifdef __linux__
#include <endian.h>
#else
#define	__LITTLE_ENDIAN	1234	/* least-significant byte first (vax, pc) */
#define	__BIG_ENDIAN	4321	/* most-significant byte first (IBM, net) */
#define	__PDP_ENDIAN	3412	/* LSB first in word, MSW first in long (pdp) */

#if defined(sun386) || defined(i386)
#define	__BYTE_ORDER	__LITTLE_ENDIAN
#endif

#if defined(sparc)
#define	__BYTE_ORDER	__BIG_ENDIAN
#endif

#endif /* __linux__ */
#endif /* __BYTE_ORDER */


#ifndef __BYTE_ORDER
# error "Undefined __BYTE_ORDER"
#endif

#ifdef __linux__
#include <byteswap.h>
#else

static inline uint16_t bswap_16(uint16_t x)
{
	return ((((x) & 0xff00) >> 8) | \
	        (((x) & 0x00ff) << 8));
}
static inline uint32_t bswap_32(uint32_t x)
{
	return ((((x) & 0xff000000) >> 24) | \
	        (((x) & 0x00ff0000) >>  8) | \
	        (((x) & 0x0000ff00) <<  8) | \
	        (((x) & 0x000000ff) << 24));
}
#endif

#endif
