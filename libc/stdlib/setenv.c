/* Copyright (C) 1992, 1995 Free Software Foundation, Inc.
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#if defined(_REENTRENT) || defined(_THREAD_SAFE)
# include <pthread.h>

/* We need to initialize the mutex.  For this we use a method provided
   by pthread function 'pthread_once'.  For this we need a once block.  */
static pthread_once__t _once_block = pthread_once_init;

/* This is the mutex which protects the global environment of simultaneous
   modifications.  */
static pthread_mutex_t _setenv_mutex;

static void
DEFUN_VOID(_init_setenv_mutex)
{
  pthread_mutex_init(&_setenv_mutex, pthread_mutexattr_default);
}

# define LOCK() \
   do { pthread_once(&_once_block, _init_setenv_mutex);
        pthread_mutex_lock(&_setenv_mutex); } while (0)
# define UNLOCK() pthread_mutex_unlock(&_setenv_mutex)

#else /* !_REENTRENT && !_THREAD_SAFE */

# define LOCK()
# define UNLOCK()

#endif /* _REENTRENT || _THREAD_SAFE */

int setenv(const char *name, const char *value, int replace)
{
  register char **ep;
  register size_t size;
  const size_t namelen = strlen (name);
  const size_t vallen = strlen (value);
  int result = 0;

  LOCK();

  size = 0;
  for (ep = __environ; *ep != NULL; ++ep)
    if (!memcmp (*ep, name, namelen) && (*ep)[namelen] == '=')
      break;
    else
      ++size;
  
  if (*ep == NULL)
    {
      static char **last_environ = NULL;
      char **new_environ = (char **) malloc((size + 2) * sizeof(char *));
      if (new_environ == NULL)
	{
	  result = -1;
	  goto do_return;
	}
      (void) memcpy((__ptr_t) new_environ, (__ptr_t) __environ, size * sizeof(char *));

      new_environ[size] = malloc (namelen + 1 + vallen + 1);
      if (new_environ[size] == NULL)
	{
	  free (new_environ);
	  __set_errno(ENOMEM);
	  result = -1;
	  goto do_return;
	}
      memcpy (new_environ[size], name, namelen);
      new_environ[size][namelen] = '=';
      memcpy (&new_environ[size][namelen + 1], value, vallen + 1);

      new_environ[size + 1] = NULL;

      if (last_environ != NULL)
	free ((__ptr_t) last_environ);
      last_environ = new_environ;
      __environ = new_environ;
    }
  else if (replace)
    {
      size_t len = strlen (*ep);
      if (len < namelen + 1 + vallen)
	{
	  char *new = malloc (namelen + 1 + vallen + 1);
	  if (new == NULL)
	    {
	      result = -1;
	      goto do_return;
	    }
	  *ep = new;
	  memcpy (*ep, name, namelen);
	  (*ep)[namelen] = '=';
	}
      memcpy (&(*ep)[namelen + 1], value, vallen + 1);
    }

do_return:
  UNLOCK();
  return result;
}


int unsetenv (const char *name)
{
  register char **ep;
  register char **dp;
  const size_t namelen = strlen (name);

  LOCK();

  for (dp = ep = __environ; *ep != NULL; ++ep)
    if (memcmp (*ep, name, namelen) || (*ep)[namelen] != '=')
      {
	*dp = *ep;
	++dp;
      }
  *dp = NULL;

  UNLOCK();
  return 0;
}
