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
#define __readdir(dirp)	    readdir64 (dirp)
#define __readdir64(dirp)   readdir64 (dirp)

#define glob_t glob64_t
#define glob(pattern, flags, errfunc, pglob) \
  glob64 (pattern, flags, errfunc, pglob)
#define globfree(pglob) globfree64 (pglob)

#undef stat
#define stat stat64
#undef __stat
#define __stat(file, buf) stat64(file, buf)

#define COMPILE_GLOB64	1

#include "glob.c"

