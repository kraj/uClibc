#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

struct chunkControl {
	size_t nodeCount;
	size_t chunkSize;
};

struct nodeControl {
	struct chunkControl *chunk;
	size_t nodeSize;
};

#define ROUND_UP_LENGTH(len) ((len+7) & ~0x07)

extern struct nodeControl *mallocNextNode;

#ifdef L_malloc
/* This variable is a pointer to the next place to allocate from.
 * Note: This variable makes the code NOT thread save. */
struct nodeControl *mallocNextNode = 0;
static size_t PageSize = 0;

#endif

#ifdef L_calloc_dbg

void *
calloc_dbg(size_t num, size_t size, char * function, char * file, int line)
{
	void * ptr;
	fprintf(stderr, "calloc of %d bytes at %s @%s:%d = ", num*size, function, file, line);
	ptr = calloc(num,size);
	fprtinf(stderr, "%p\n", ptr);
	return ptr;
}

#endif

#ifdef L_malloc_dbg

void *
malloc_dbg(size_t len, char * function, char * file, int line)
{
	void * result;
	fprintf(stderr, "malloc of %d bytes at %s @%s:%d = ", len, function, file, line);
	result = malloc(len);
	fprintf(stderr, "%p\n", result);    
	return result;
}

#endif

#ifdef L_free_dbg

void
free_dbg(void * ptr, char * function, char * file, int line)
{
	fprintf(stderr, "free of %p at %s @%s:%d\n", ptr, function, file, line);
  	free(ptr);
}

#endif


#ifdef L_calloc

void *
calloc(size_t num, size_t size)
{
	void * ptr = malloc(num*size);
	if (ptr)
		memset(ptr, 0, num*size);
	return ptr;
}

#endif

#ifdef L_malloc

void *
malloc(size_t len)
{
	void *result;
	struct chunkControl *chunk;
	struct nodeControl  *next;
	size_t size;

	/* round len up to keep things on even boundaries */
	len = ROUND_UP_LENGTH(len);

	if (len == 0)
		return 0;

TryAgain:
	if (mallocNextNode != 0) {
		/* first see if this request will fit on this chunk */
		next  = mallocNextNode;
		chunk = next->chunk;
		if (((char *)next + sizeof(struct nodeControl)*2 + len) < 
		    ((char *)chunk + chunk->chunkSize))
		{
			/* this request will fit, so simply move the next
			 * pointer ahead and update chunk node count */
			next->nodeSize = len;
			result = (char *)next + sizeof(struct nodeControl);
			chunk->nodeCount++;
			next = (struct nodeControl *)
			          ((char *)next + (sizeof(struct nodeControl) + len));
			next->chunk = chunk;
			next->nodeSize = 0;
			mallocNextNode = next;

			return result; /* normal return path */
		}
		
	}
	
	/* the request will not fit on this chunk, so get another chunk */
	if (PageSize == 0) {
		PageSize = getpagesize();
	}
	size = len + (sizeof(struct chunkControl) + (sizeof(struct nodeControl) * 2));
	if (size < PageSize * 2) {
		size = PageSize * 2;
	}
	size = (size + (PageSize-1)) & ~(PageSize-1);

	chunk = mmap((void *)0, size, PROT_READ | PROT_WRITE,
	              MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (chunk == (void*)-1)
		return 0;

	chunk->chunkSize = size;
	chunk->nodeCount = 0;
	next             = (struct nodeControl *)
		                 ((char *)chunk + sizeof(struct chunkControl));
	next->chunk      = chunk;
	mallocNextNode   = next;

	goto TryAgain;
}

#endif

#ifdef L_free

void
free(void * ptr)
{
	struct chunkControl *chunk;
	struct nodeControl  *node;
	
	if (ptr == 0) {
		return;
	}
	/* get a pointer to the control information for this memory node
	 * and the chunk it belongs to */
	node  = (struct nodeControl *)ptr - 1;
	chunk = node->chunk;
	/* decrement the node count and if it is zero free the chunk */
	chunk->nodeCount--;
	if (chunk->nodeCount == 0) {
		if ((void *)mallocNextNode >= (void *)chunk && 
		     ((void *)mallocNextNode < (void *)((char *)chunk + chunk->chunkSize)))
		{
			mallocNextNode = 0;
		}
		munmap(chunk, chunk->chunkSize);
	}
}

#endif

#ifdef L_realloc

void *
realloc(void *ptr, size_t len)
{
	struct nodeControl *node;
	size_t oldSize;
	void *new;
	
	
	if (ptr == 0) {
		return malloc(len);
	}
	if (len == 0) {
		free(ptr);
		return 0;
	}
	node    = (struct nodeControl *)ptr - 1;
	oldSize = node->nodeSize;
	if (oldSize >= len) {
		return ptr;
	}
	
	new = malloc(len);
	memcpy(new, ptr, len);
	free(ptr);
	return new;
}

#endif

