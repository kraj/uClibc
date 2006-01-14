/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define __USE_GNU
#include <sys/utsname.h>

libc_hidden_proto(strlen)
libc_hidden_proto(strcpy)
libc_hidden_proto(uname)

int
getdomainname(char *name, size_t len)
{
  struct utsname uts;

  if (name == NULL) {
    __set_errno(EINVAL);
    return -1;
  }

  if (uname(&uts) == -1) return -1;

  if (strlen(uts.domainname)+1 > len) {
    __set_errno(EINVAL);
    return -1;
  }
  strcpy(name, uts.domainname);
  return 0;
}
libc_hidden_proto(getdomainname)
libc_hidden_def(getdomainname)
