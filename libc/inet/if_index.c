/* Copyright (C) 1997, 1998, 1999, 2000, 2002, 2003, 2004, 2005
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.

   Reworked Dec 2002 by Erik Andersen <andersen@codepoet.org>
 */

#define strndup __strndup

#define __FORCE_GLIBC
#include <features.h>
#define __USE_GNU
#include <string.h>
#include <alloca.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <libc-internal.h>

extern int __opensock(void) attribute_hidden;

unsigned int
if_nametoindex(const char* ifname) 
{
#ifndef SIOCGIFINDEX
  __set_errno (ENOSYS);
  return 0;
#else
  struct ifreq ifr;
  int fd = __opensock();

  if (fd < 0)
    return 0;

  __strncpy (ifr.ifr_name, ifname, sizeof (ifr.ifr_name));
  if (__ioctl (fd, SIOCGIFINDEX, &ifr) < 0)
    {
      int saved_errno = errno;
      __close(fd);
      if (saved_errno == EINVAL)
	__set_errno(ENOSYS);
      return 0;
    }

  __close(fd);
  return ifr.ifr_ifindex;
#endif
}
hidden_strong_alias(if_nametoindex,__if_nametoindex)

void
if_freenameindex (struct if_nameindex *ifn)
{
  struct if_nameindex *ptr = ifn;
  while (ptr->if_name || ptr->if_index)
    {
      free (ptr->if_name);
      ++ptr;
    }
  free (ifn);
}

struct if_nameindex *
if_nameindex (void)
{
#ifndef SIOCGIFINDEX
  __set_errno (ENOSYS);
  return NULL;
#else
  int fd = __opensock ();
  struct ifconf ifc;
  unsigned int nifs, i;
  int rq_len;
  struct if_nameindex *idx = NULL;
# define RQ_IFS	4

  if (fd < 0)
    return NULL;

  ifc.ifc_buf = NULL;

  /* Guess on the correct buffer size... */
  rq_len = RQ_IFS * sizeof (struct ifreq);

  /* Read all the interfaces out of the kernel.  */
  /* Note: alloca's in this loop are diff from glibc because it's smaller */
  do
    {
      ifc.ifc_buf = extend_alloca (ifc.ifc_buf, rq_len, 2 * rq_len);
      ifc.ifc_len = rq_len;

      if (__ioctl (fd, SIOCGIFCONF, &ifc) < 0)
	{
	  __close (fd);
	  return NULL;
	}
    }
  while (ifc.ifc_len == rq_len);

  nifs = ifc.ifc_len / sizeof(struct ifreq);

  idx = malloc ((nifs + 1) * sizeof (struct if_nameindex));
  if (idx == NULL)
    {
      __close(fd);
      __set_errno(ENOBUFS);
      return NULL;
    }

  for (i = 0; i < nifs; ++i)
    {
      struct ifreq *ifr = &ifc.ifc_req[i];
      idx[i].if_name = __strdup (ifr->ifr_name);
      if (idx[i].if_name == NULL
	  || __ioctl (fd, SIOCGIFINDEX, ifr) < 0)
	{
	  int saved_errno = errno;
	  unsigned int j;

	  for (j =  0; j < i; ++j)
	    free (idx[j].if_name);
	  free(idx);
	  __close(fd);
	  if (saved_errno == EINVAL)
	    saved_errno = ENOSYS;
	  else if (saved_errno == ENOMEM)
	    saved_errno = ENOBUFS;
	  __set_errno (saved_errno);
	  return NULL;
	}
      idx[i].if_index = ifr->ifr_ifindex;
    }

  idx[i].if_index = 0;
  idx[i].if_name = NULL;

  __close(fd);
  return idx;
#endif
}

char *
if_indextoname (unsigned int ifindex, char *ifname)
{
#if !defined SIOCGIFINDEX
  __set_errno (ENOSYS);
  return NULL;
#else
# ifdef SIOCGIFNAME
  /* Use ioctl to avoid searching the list. */
  struct ifreq ifr;
  int fd;

  fd = __opensock ();

  if (fd < 0)
    return NULL;

  ifr.ifr_ifindex = ifindex;
  if (__ioctl (fd, SIOCGIFNAME, &ifr) < 0)
    {
      int serrno = errno;
      __close (fd);
      if (serrno == ENODEV)
	/* POSIX requires ENXIO.  */
	serrno = ENXIO;
      __set_errno (serrno);
      return NULL;
  }
  __close (fd);

  return __strncpy (ifname, ifr.ifr_name, IFNAMSIZ);
# else
  struct if_nameindex *idx;
  struct if_nameindex *p;
  char *result = NULL;

  idx = if_nameindex();

  if (idx != NULL)
    {
      for (p = idx; p->if_index || p->if_name; ++p)
	if (p->if_index == ifindex)
	  {
	    result = __strncpy (ifname, p->if_name, IFNAMSIZ);
	    break;
	  }

      if_freenameindex (idx);

      if (result == NULL)
	__set_errno (ENXIO);
    }
  return result;
# endif
#endif
}

