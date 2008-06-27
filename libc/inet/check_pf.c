/* Determine protocol families for which interfaces exist.  Generic version.
   Copyright (C) 2003, 2006 Free Software Foundation, Inc.
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
   02111-1307 USA.  */

#include <features.h>
#include <ifaddrs.h>
#include <netdb.h>


void
attribute_hidden
__check_pf (bool *seen_ipv4, bool *seen_ipv6)
{
  *seen_ipv4 = false;
  *seen_ipv6 = false;
#if __UCLIBC_SUPPORT_AI_ADDRCONFIG__
  {
    /* Get the interface list via getifaddrs.  */
    struct ifaddrs *ifa = NULL;
    struct ifaddrs *runp;
    if (getifaddrs (&ifa) != 0)
    {
      /* We cannot determine what interfaces are available.  Be
      optimistic.  */
      *seen_ipv4 = true;
#if __UCLIBC_HAS_IPV6__
      *seen_ipv6 = true;
#endif /* __UCLIBC_HAS_IPV6__ */
      return;
    }

    for (runp = ifa; runp != NULL; runp = runp->ifa_next)
      if (runp->ifa_addr->sa_family == PF_INET)
        *seen_ipv4 = true;
#if __UCLIBC_HAS_IPV6__
      else if (runp->ifa_addr->sa_family == PF_INET6)
        *seen_ipv6 = true;
#endif /* __UCLIBC_HAS_IPV6__ */

    (void) freeifaddrs (ifa);
  }
#else
  /* AI_ADDRCONFIG is disabled, assume both ipv4 and ipv6 available. */
  *seen_ipv4 = true;
#if __UCLIBC_HAS_IPV6__
  *seen_ipv6 = true;
#endif /* __UCLIBC_HAS_IPV6__ */

#endif /* __UCLIBC_SUPPORT_AI_ADDRCONFIG__ */
}
