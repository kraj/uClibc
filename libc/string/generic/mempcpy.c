/* Copy memory to memory until the specified number of bytes
   has been copied, return pointer to following byte.
   Overlap is NOT handled correctly.
*/

/* Ditch the glibc version and just wrap memcpy. */

#include <string.h>

#ifdef __USE_GNU
/* Experimentally off - libc_hidden_proto(mempcpy) */
/* Experimentally off - libc_hidden_proto(memcpy) */

void *mempcpy (void *dstpp, const void *srcpp, size_t len)
{
  memcpy(dstpp, srcpp, len);
  return (void *)(((char *)dstpp) + len);
}
libc_hidden_weak(mempcpy)
#endif
