
#ifndef __MALLOC_H
#define __MALLOC_H
#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

extern void free __P((void *));
extern void *malloc __P((size_t));
extern void *realloc __P((void *, size_t));
extern void *alloca __P((size_t));

extern void *(*__alloca_alloc) __P((size_t));

__END_DECLS

#endif
