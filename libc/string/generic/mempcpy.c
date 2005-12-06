/* Copy memory to memory until the specified number of bytes
   has been copied, return pointer to following byte.
   Overlap is NOT handled correctly.
*/

/* Ditch the glibc version and just wrap memcpy. */

#include <string.h>

#undef mempcpy

void attribute_hidden *__mempcpy (void *dstpp, const void *srcpp, size_t len)
{
  __memcpy(dstpp, srcpp, len);
  return (void *)(((char *)dstpp) + len);
}

strong_alias(__mempcpy,mempcpy)
