/* locale.h
 * Support international type specific characters.
 */
#ifndef _LOCALE_H
#define _LOCALE_H       1

#include <features.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *) 0)
#endif
#endif

/* These are the possibilities for the first argument to setlocale.
   The code assumes that LC_ALL is the highest value, and zero the lowest.  */
#define LC_CTYPE        0
#define LC_NUMERIC      1
#define LC_TIME         2
#define LC_COLLATE      3
#define LC_MONETARY     4
#define LC_MESSAGES     5
#define LC_ALL          6

extern char *setlocale(int __category, __const char *__locale);

#endif /* locale.h  */
