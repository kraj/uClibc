/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>

#if defined __USE_BSD || (defined __USE_XOPEN && !defined __USE_UNIX98)
/* Experimentally off - libc_hidden_proto(strlen) */
/* Experimentally off - libc_hidden_proto(strcpy) */
libc_hidden_proto(uname)

#if !defined __UCLIBC_BSD_SPECIFIC__
extern int getdomainname (char *__name, size_t __len)
	__THROW __nonnull ((1)) __wur;
#endif
extern __typeof(getdomainname) __libc_getdomainname;
libc_hidden_proto(__libc_getdomainname)
int __libc_getdomainname(char *name, size_t len)
{
  struct utsname uts;

  if (name == NULL) {
    __set_errno(EINVAL);
    return -1;
  }

  if (uname(&uts) == -1) return -1;

#ifdef __USE_GNU
  if (strlen(uts.domainname)+1 > len) {
#else
  if (strlen(uts.__domainname)+1 > len) {
#endif
    __set_errno(EINVAL);
    return -1;
  }
#ifdef __USE_GNU
  strcpy(name, uts.domainname);
#else
  strcpy(name, uts.__domainname);
#endif
  return 0;
}
libc_hidden_def(__libc_getdomainname)
#if defined __UCLIBC_BSD_SPECIFIC__
libc_hidden_proto(getdomainname)
weak_alias(__libc_getdomainname,getdomainname)
libc_hidden_weak(getdomainname)
#endif /* __UCLIBC_BSD_SPECIFIC__ */
#endif
