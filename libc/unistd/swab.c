#include <unistd.h>
#include <sys/types.h>

/* swab() swaps the position of two adjacent bytes, every two bytes.
 * Contributed by Kensuke Otake <kensuke@phreaker.net> */

void swab(const void *source, void *dest, ssize_t count) {
  const char *from = (const char *)source;
  char *to = (char *)dest;

  count &= ~((ssize_t)1);

  while (count > 1) {
    const char b0 = from[--count], b1 = from[--count];
    to[count] = b0;
    to[count + 1] = b1;
  }
}
