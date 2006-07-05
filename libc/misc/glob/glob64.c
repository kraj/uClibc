/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <_lfs_64.h>

#include <dirent.h>
#include <glob.h>
#include <sys/stat.h>

#define dirent dirent64

#define glob_t glob64_t
#define glob(pattern, flags, errfunc, pglob) \
  glob64 (pattern, flags, errfunc, pglob)
#define globfree(pglob) globfree64 (pglob)

#undef stat
#define stat stat64

#define COMPILE_GLOB64    1

#include "glob.c"
