/* Copyright (C) 1995,1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

#include <string.h>
#include <malloc.h>

/* This is a basic string package; it includes the most used functions

   strlen strcat strcpy strcmp strncat strncpy strncmp strchr strrchr strdup
   memcpy memccpy memset memmove

   These functions are in seperate files.
    strpbrk.o strsep.o strstr.o strtok.o strcspn.o
    strspn.o strcasecmp.o strncasecmp.o
 */

/********************** Function strlen ************************************/

#ifdef L_strlen
size_t strlen(const char * str)
{
   register char * p =(char *) str;
   while(*p) p++;
   return p-str;
}
#endif

/********************** Function strcat ************************************/

#ifdef L_strcat
char * strcat(char *d, const char * s)
{
   (void) strcpy(d+strlen(d), s);
   return d;
}
#endif

/********************** Function strcpy ************************************/

#ifdef L_strcpy
char * strcpy( char *d, const char * s)
{
   /* This is probably the quickest on an 8086 but a CPU with a cache will
    * prefer to do this in one pass */
   return memcpy(d, s, strlen(s)+1);
}
#endif

/********************** Function strcmp ************************************/

#ifdef L_strcmp
int strcmp(const char *d, const char * s)
{
    register const unsigned char *s1 = (const unsigned char *) d;
    register const unsigned char *s2 = (const unsigned char *) s;
    unsigned register char c1, c2;

    do
    {
	c1 = (unsigned char) *s1++;
	c2 = (unsigned char) *s2++;
	if (c1 == '\0')
	    return c1 - c2;
    }
    while (c1 == c2);

    return c1 - c2;
}
#endif

/********************** Function strncat ************************************/

#ifdef L_strncat
char * strncat( char *d, const char *s, size_t l)
{
   register char *s1=d+strlen(d), *s2;
   
   s2 = memchr(s, 0, l);
   if( s2 )
      memcpy(s1, s, s2-s+1);
   else
   {
      memcpy(s1, s, l);
      s1[l] = '\0';
   }
   return d;
}
#endif

/********************** Function strncpy ************************************/

#ifdef L_strncpy
char * strncpy ( char *s1, const char *s2, size_t n)
{
    register char c;
    char *s = s1;

    --s1;

    if (n >= 4)
    {
	size_t n4 = n >> 2;

	for (;;)
	{
	    c = *s2++;
	    *++s1 = c;
	    if (c == '\0')
		break;
	    c = *s2++;
	    *++s1 = c;
	    if (c == '\0')
		break;
	    c = *s2++;
	    *++s1 = c;
	    if (c == '\0')
		break;
	    c = *s2++;
	    *++s1 = c;
	    if (c == '\0')
		break;
	    if (--n4 == 0)
		goto last_chars;
	}
	n = n - (s1 - s) - 1;
	if (n == 0)
	    return s;
	goto zero_fill;
    }

last_chars:
    n &= 3;
    if (n == 0)
	return s;

    do
    {
	c = *s2++;
	*++s1 = c;
	if (--n == 0)
	    return s;
    }
    while (c != '\0');

zero_fill:
    do
	*++s1 = '\0';
    while (--n > 0);

    return s;
}
#endif

/********************** Function strncmp ************************************/

#ifdef L_strncmp
int strncmp (const char *s1, const char *s2, size_t n)
{
  unsigned register char c1 = '\0';
  unsigned register char c2 = '\0';

  if (n >= 4)
    {
      size_t n4 = n >> 2;
      do
	{
	  c1 = (unsigned char) *s1++;
	  c2 = (unsigned char) *s2++;
	  if (c1 == '\0' || c1 != c2)
	    return c1 - c2;
	  c1 = (unsigned char) *s1++;
	  c2 = (unsigned char) *s2++;
	  if (c1 == '\0' || c1 != c2)
	    return c1 - c2;
	  c1 = (unsigned char) *s1++;
	  c2 = (unsigned char) *s2++;
	  if (c1 == '\0' || c1 != c2)
	    return c1 - c2;
	  c1 = (unsigned char) *s1++;
	  c2 = (unsigned char) *s2++;
	  if (c1 == '\0' || c1 != c2)
	    return c1 - c2;
	} while (--n4 > 0);
      n &= 3;
    }

  while (n > 0)
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0' || c1 != c2)
	return c1 - c2;
      n--;
    }

  return c1 - c2;
}
#endif

/********************** Function strchr ************************************/

#ifdef L_strchr
char *
strchr(s, c)
const char * s;
int c;
{
   register char ch;
   for(;;)
   {
     if( (ch= *s) == c ) return (char*)s;
     if( ch == 0 ) return 0;
     s++;
   }
}
#endif

/********************** Function strrchr ************************************/

#ifdef L_strrchr
char * strrchr(s, c)
const char * s;
int c;
{
   register char * prev = 0;
   register char * p = (char*)s;
   /* For null it's just like strlen */
   if( c == '\0' ) return p+strlen(p);

   /* everything else just step along the string. */
   while( (p=strchr(p, c)) != 0 )
   {
      prev = p; p++;
   }
   return prev;
}
#endif

/********************** Function strdup ************************************/

#ifdef L_strdup
char * strdup(s)
const char * s;
{
   register size_t len;
   register char * p;

   len = strlen(s)+1;
   p = (char *) malloc(len);
   if(p) memcpy(p, s, len); /* Faster than strcpy */
   return p;
}
#endif

/********************** Function memcpy ************************************/

#ifdef L_memcpy
void *
memcpy(d, s, l)
void *d;
const void *s;
size_t l;
{
   register char *s1=d, *s2=(char *)s;
   for( ; l>0; l--) *((unsigned char*)s1++) = *((unsigned char*)s2++);
   return d;
}
#endif

/********************** Function memccpy ************************************/

#ifdef L_memccpy
void * memccpy(d, s, c, l)	/* Do we need a fast one ? */
void *d;
const void *s;
int c;
size_t l;
{
   register char *s1=d, *s2=(char*)s;
   while(l-- > 0)
      if((*s1++ = *s2++) == c )
         return s1;
   return 0;
}
#endif

/********************** Function memset ************************************/

#ifdef L_memset
void * memset(str, c, l)
void * str;
int c;
size_t l;
{
   register char *s1=str;
   while(l-->0) *s1++ = c;
   return str;
}
#endif

/********************** Function memmove ************************************/

#ifdef L_memmove
void *
memmove(d, s, l)
void *d;
const void *s;
size_t l;
{
   register char *s1=d, *s2=(char*)s;
   /* This bit of sneakyness c/o Glibc, it assumes the test is unsigned */
   if( s1-s2 >= l ) return memcpy(d,s,l);

   /* This reverse copy only used if we absolutly have to */
   s1+=l; s2+=l;
   while(l-- >0)
      *(--s1) = *(--s2);
   return d;
}
#endif


/********************** THE END ********************************************/

