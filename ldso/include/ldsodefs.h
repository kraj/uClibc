#ifndef _LDSODEFS_H
#define _LDSODEFS_H     1

#include <features.h>
#include <tls.h>

/*
 * MIPS NPTL - These were taken from 'string/test-string.h'
 */
#define GL(x) _##x
#define GLRO(x) _##x

/*
 * MIPS NPTL - This was taken from 'sysdeps/generic/ldsodefs.h'. It is
 *             most likely used for backtrace capability.
 */
extern void *__libc_stack_end;

/* Get size and alignment requirements of the static TLS block.  */
extern void _dl_get_tls_static_info (size_t *sizep, size_t *alignp)
	internal_function;

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
