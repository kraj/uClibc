/* Copyright (C) 1996 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <ansidecl.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/param.h>

static int __linux_kernel_version = -1;

static inline int
asc2int (const char *s)
{
  int result = 0;

  for (; *s >= '0' && *s <= '9'; s++)
  {
    result = result * 10 + (*s - '0');
  }

  return result;
}

static int
set_linux_kernel_version (void)
{
  struct utsname uts;
  char *version = NULL, *patchlevel = NULL, *sublevel = NULL;

  if (uname (&uts))
  {
    __linux_kernel_version = 0;
    return __linux_kernel_version;
  }

  version = uts.release;
  if (version != NULL)
  {
    patchlevel = strchr (version, '.');
    if (patchlevel != NULL)
    {
      *patchlevel = '\0';
      patchlevel++;
      sublevel = strchr (patchlevel, '.');
      if (sublevel != NULL)
      {
	*sublevel = '\0';
	sublevel++;
      }
    }

    __linux_kernel_version =
	GET_LINUX_KERNEL_VERSION (asc2int (version));
    if (patchlevel != NULL)
    {
      __linux_kernel_version |=
	GET_LINUX_KERNEL_PATCHLEVEL (asc2int (patchlevel));
    }
    if (sublevel != NULL)
    {
      __linux_kernel_version |=
	GET_LINUX_KERNEL_SUBLEVEL (asc2int (sublevel));
    }
  }
  else
  {
    __linux_kernel_version = 0;
  }

  return __linux_kernel_version;
}

int
__get_linux_kernel_version (void)
{
  if (__linux_kernel_version != -1)
    return __linux_kernel_version;
    
  return set_linux_kernel_version ();
}
