#define uname __uname

#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <errno.h>

int attribute_hidden
__gethostname(char *name, size_t len)
{
  struct utsname uts;

  if (name == NULL) {
    __set_errno(EINVAL);
    return -1;
  }

  if (uname(&uts) == -1) return -1;

  if (__strlen(uts.nodename)+1 > len) {
    __set_errno(EINVAL);
    return -1;
  }
  __strcpy(name, uts.nodename);
  return 0;
}
strong_alias(__gethostname,gethostname)
