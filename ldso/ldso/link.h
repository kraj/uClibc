#ifndef _LINK_H
#define _LINK_H

#include "elf.h"

/* Header file that describes the internal data structures used by the
 * ELF dynamic linker.  */

struct link_map
{
  /* These entries must be in this order to be compatible with the
   * interface used by gdb to obtain the list of symbols. */
  unsigned long l_addr;	/* address at which object is mapped */
  char *l_name;		/* full name of loaded object */
  Elf32_Dyn *l_ld;	/* dynamic structure of object */
  struct link_map *l_next;
  struct link_map *l_prev;
};

/* The DT_DEBUG entry in the .dynamic section is given the address of
 * this structure. gdb can pick this up to obtain the correct list of
 * loaded modules. */
struct r_debug
{
  int r_version;		/* debugging info version no */
  struct link_map *r_map;	/* address of link_map */
  unsigned long r_brk;		/* address of update routine */
  enum
  {
    RT_CONSISTENT,
    RT_ADD,
    RT_DELETE
  } r_state;
  unsigned long r_ldbase;	/* base addr of ld.so */
};

#endif /* _LINK_H */
