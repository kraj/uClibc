#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int seteuid(uid_t uid)
{
  switch (sizeof (uid_t))
  {
  case 2:
    if (uid == 65535)
    {
      __set_errno(EINVAL);
      return -1;
    }
    break;

  default:
    fprintf (stderr, "Uknown uid_t size and sign\n");
  }

  return setreuid(-1, uid);
}
