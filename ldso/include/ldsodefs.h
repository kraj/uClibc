#ifndef _LDSODEFS_H
#define _LDSODEFS_H     1

#include <features.h>
#include <tls.h>

/*
 * NPTL - These are supposed to be from 'sysdeps/generic/ldsodefs.h'
 *        and used with the ld.so global structure. I am not sure how
 *        this fits into uClibc yet, so they are empty.
 */
#define GL(x) _##x
#define GLRO(x) _##x

/*
 * NPTL - This was taken from 'sysdeps/generic/ldsodefs.h'. It is
 *        used for backtrace capability.
 */
/* Variable pointing to the end of the stack (or close to it).  This value
   must be constant over the runtime of the application.  Some programs
   might use the variable which results in copy relocations on some
   platforms.  But this does not matter, ld.so can always use the local
   copy.  */
extern void *__libc_stack_end attribute_relro;
rtld_hidden_proto (__libc_stack_end)

/* Determine next available module ID.  */
extern size_t _dl_next_tls_modid (void) internal_function attribute_hidden;

/* Calculate offset of the TLS blocks in the static TLS block.  */
extern void _dl_determine_tlsoffset (void) internal_function attribute_hidden;

/* Set up the data structures for TLS, when they were not set up at startup.
   Returns nonzero on malloc failure.
   This is called from _dl_map_object_from_fd or by libpthread.  */
extern int _dl_tls_setup (void) internal_function;
rtld_hidden_proto (_dl_tls_setup)

/* Allocate memory for static TLS block (unless MEM is nonzero) and dtv.  */
extern void *_dl_allocate_tls (void *mem) internal_function;
rtld_hidden_proto (_dl_allocate_tls)

/* Get size and alignment requirements of the static TLS block.  */
extern void _dl_get_tls_static_info (size_t *sizep, size_t *alignp)
     internal_function;

extern void _dl_allocate_static_tls (struct link_map *map)
     internal_function attribute_hidden;

/* These are internal entry points to the two halves of _dl_allocate_tls,
   only used within rtld.c itself at startup time.  */
extern void *_dl_allocate_tls_storage (void)
     internal_function attribute_hidden;
extern void *_dl_allocate_tls_init (void *) internal_function;
rtld_hidden_proto (_dl_allocate_tls_init)

/* Deallocate memory allocated with _dl_allocate_tls.  */
extern void _dl_deallocate_tls (void *tcb, bool dealloc_tcb) internal_function;
rtld_hidden_proto (_dl_deallocate_tls)

#if defined USE_TLS
extern void _dl_nothread_init_static_tls (struct link_map *) attribute_hidden;
#endif
  /* Keep the conditional TLS members at the end so the layout of the
     structure used by !USE_TLS code matches the prefix of the layout in
     the USE_TLS rtld.  Note that `struct link_map' is conditionally
     defined as well, so _dl_rtld_map needs to be last before this.  */
#ifdef USE_TLS
  /* Highest dtv index currently needed.  */
  extern size_t _dl_tls_max_dtv_idx;
  /* Flag signalling whether there are gaps in the module ID allocation.  */
  extern bool _dl_tls_dtv_gaps;
  /* Information about the dtv slots.  */
  extern struct dtv_slotinfo_list
  {
    size_t len;
    struct dtv_slotinfo_list *next;
    struct dtv_slotinfo
    {
      size_t gen;
      bool is_static;
      struct link_map *map;
    } slotinfo[0];
  } *_dl_tls_dtv_slotinfo_list;
  /* Number of modules in the static TLS block.  */
  extern size_t _dl_tls_static_nelem;
  /* Size of the static TLS block.  */
  extern size_t _dl_tls_static_size;
  /* Size actually allocated in the static TLS block.  */
  extern size_t _dl_tls_static_used;
  /* Alignment requirement of the static TLS block.  */
  extern size_t _dl_tls_static_align;

/* Number of additional entries in the slotinfo array of each slotinfo
   list element.  A large number makes it almost certain take we never
   have to iterate beyond the first element in the slotinfo list.  */
# define TLS_SLOTINFO_SURPLUS (62)

/* Number of additional slots in the dtv allocated.  */
# define DTV_SURPLUS	(14)

  /* Initial dtv of the main thread, not allocated with normal malloc.  */
  extern void *_dl_initial_dtv;
  /* Generation counter for the dtv.  */
  extern size_t _dl_tls_generation;

  extern void (*_dl_init_static_tls) (struct link_map *);
#endif

#endif
