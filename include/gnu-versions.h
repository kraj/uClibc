/* Header to ignore some special GNU libc interfaces not provided
 * by uClibc.  */

#ifndef _GNU_VERSIONS_H
#define	_GNU_VERSIONS_H	1

/* This file exists to avoid uClibc getting into trouble these macros
   such as the following:

       #define OBSTACK_INTERFACE_VERSION 1
       #if !defined (_LIBC) && defined (__GNU_LIBRARY__) && __GNU_LIBRARY__ > 1
       #include <gnu-versions.h>
       #if _GNU_OBSTACK_INTERFACE_VERSION == OBSTACK_INTERFACE_VERSION
       #define ELIDE_CODE
       #endif
       #endif

   By defining bogus interface versions, I believe we can safely get
   past such things while continuing to lie and define __GNU_LIBRARY__,
   thereby getting better compatibility with the majority of the code
   written for Linux.  */

#define _GNU_OBSTACK_INTERFACE_VERSION	-1 /* vs malloc/obstack.c */
#define _GNU_REGEX_INTERFACE_VERSION	-1 /* vs posix/regex.c */
#define _GNU_GLOB_INTERFACE_VERSION	-1 /* vs posix/glob.c */
#define _GNU_GETOPT_INTERFACE_VERSION	-1 /* vs posix/getopt.c and
					     posix/getopt1.c */

#endif	/* gnu-versions.h */
