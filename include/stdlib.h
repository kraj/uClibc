/* stdlib.h  */
#ifndef __STDLIB_H
#define __STDLIB_H

#include <features.h>
#include <sys/types.h>
#include <limits.h>

__BEGIN_DECLS

/* Don't overwrite user definitions of NULL */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/* We define these the same for all machines.
 * Changes from this to the outside world should be done in `_exit'.  */
#define EXIT_FAILURE    1       /* Failing exit status.  */
#define EXIT_SUCCESS    0       /* Successful exit status.  */

/* The largest number rand will return */
#define RAND_MAX        INT_MAX

/* Maximum length of a multibyte character in the current locale.  */
#define MB_CUR_MAX  1

typedef struct
{
    int quot;                   /* Quotient.  */
    int rem;                    /* Remainder.  */
} div_t;

typedef struct
{
    long int quot;              /* Quotient.  */
    long int rem;               /* Remainder.  */
} ldiv_t;

/*  comparison function used by bsearch() and qsort() */
typedef int (*__compar_fn_t) __P ((__const __ptr_t, __const __ptr_t));
typedef __compar_fn_t comparison_fn_t;


/* String to number conversion functions */
extern double atof(const char *nptr);
extern int atoi(const char *nptr);
extern long atol(const char *nptr);
extern long long atoll(const char *nptr);
extern long strtol __P ((const char * nptr, char ** endptr, int base));
extern unsigned long strtoul __P ((const char * nptr, char ** endptr, int base));
extern long long strtoll __P ((const char * nptr, char ** endptr, int base));
extern unsigned long long strtoull __P ((const char * nptr, char ** endptr, int base));
#ifdef __UCLIBC_HAS_FLOATS__
/*TODO: extern char * gcvt __P ((double number, size_t ndigit, char * buf)); */
extern double strtod __P ((const char * nptr, char ** endptr));
#endif



/* Random number functions */
extern int rand __P ((void));
extern void srand __P ((unsigned int seed));
extern long int random(void);
extern void srandom(unsigned int seed);

/* Memory management functions */
extern __ptr_t calloc __P ((size_t, size_t));
extern __ptr_t malloc __P ((size_t));
extern __ptr_t realloc __P ((__ptr_t, size_t));
extern void free __P ((__ptr_t));
/* Allocate a block on the stack that will be freed 
 * when the calling function exits.  We use gcc's
 * version to make life better... */
#undef	alloca
extern __ptr_t alloca __P ((size_t __size));
#define alloca(size)	__builtin_alloca (size)

#ifdef DEBUG_MALLOC
extern __ptr_t calloc_dbg __P ((size_t, size_t, char* func, char* file, int line));
extern __ptr_t malloc_dbg __P ((size_t, char* func, char* file, int line));
extern __ptr_t realloc_dbg __P ((__ptr_t, size_t, char* func, char* file, int line));
extern void free_dbg __P ((__ptr_t, char* func, char* file, int line));
#define calloc(x,y) calloc_dbg((x),(y),__FUNCTION__,__FILE__,__LINE__)
#define malloc(x) malloc_dbg((x),__FUNCTION__,__FILE__,__LINE__)
#define realloc(x,y) realloc_dbg((x),(y),__FUNCTION__,__FILE__,__LINE__)
#define free(x) free_dbg((x),__FUNCTION__,__FILE__,__LINE__)
#endif



/* System and environment functions */
extern void abort __P ((void)) __attribute__ ((__noreturn__));
extern int atexit __P ((void (*__func) (void)));
extern void exit __P ((int __status)) __attribute__ ((__noreturn__));
extern void _exit __P ((int __status)) __attribute__ ((__noreturn__));
extern char *getenv __P ((__const char *__name));
extern int putenv __P ((__const char *__string));
extern char *realpath __P ((__const char *__restrict __name,
	    char *__restrict __resolved));
extern int setenv __P ((__const char *__name, __const char *__value,
                        int __replace));
extern int system __P ((__const char *__command));
extern void unsetenv __P ((__const char *__name));

/* The following is used by uClibc in atexit.c and sysconf.c */
#define __UCLIBC_MAX_ATEXIT     20

/* Search and sort functions */
extern __ptr_t bsearch __P ((__const __ptr_t __key, __const __ptr_t __base,
			   size_t __nmemb, size_t __size, __compar_fn_t __compar));
extern void qsort __P ((__ptr_t __base, size_t __nmemb, size_t __size,
			  __compar_fn_t __compar));



/* Integer math functions */
extern int abs __P ((int __x)) __attribute__ ((__const__));
extern div_t div __P ((int __numer, int __denom)) __attribute__ ((__const__));
extern long int labs __P ((long int __x)) __attribute__ ((__const__));
extern ldiv_t ldiv __P ((long int __numer, long int __denom)) __attribute__ ((__const__));

/* Generate a unique temporary file name from TEMPLATE. */
extern char *mktemp __P ((char *__template));
extern int mkstemp __P ((char *__template));

__END_DECLS

#endif /* __STDLIB_H */
