/* Copyright (C) 2003, 2004 Red Hat, Inc.
 * Contributed by Alexandre Oliva <aoliva@redhat.com>
 * Copyright (C) 2006-2011 Analog Devices, Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <bfin_sram.h>

#define __dl_loadaddr_unmap __dl_loadaddr_unmap

#include "../fdpic/dl-inlines.h"

void
__dl_loadaddr_unmap (struct elf32_fdpic_loadaddr loadaddr,
		     struct funcdesc_ht *funcdesc_ht)
{
  int i;

  for (i = 0; i < loadaddr.map->nsegs; i++)
    {
      struct elf32_fdpic_loadseg *segdata;
      ssize_t offs;
      segdata = loadaddr.map->segs + i;

      /* FIXME:
        A more cleaner way is to add type for struct elf32_fdpic_loadseg,
        and release the memory according to the type.
        Currently, we hardcode the memory address of L1 SRAM.  */
      if ((segdata->addr & 0xff800000) == 0xff800000)
       {
         _dl_sram_free ((void *)segdata->addr);
         continue;
       }

      offs = (segdata->p_vaddr & ADDR_ALIGN);
      _dl_munmap ((void*)segdata->addr - offs,
		  segdata->p_memsz + offs);
    }
  /* _dl_unmap is only called for dlopen()ed libraries, for which
     calling free() is safe, or before we've completed the initial
     relocation, in which case calling free() is probably pointless,
     but still safe.  */
  _dl_free (loadaddr.map);
  if (funcdesc_ht)
    htab_delete (funcdesc_ht);
}

static __always_inline int
__dl_is_special_segment (Elf32_Ehdr *epnt,
			 Elf32_Phdr *ppnt)
{
  if (ppnt->p_type != PT_LOAD)
    return 0;

  if ((epnt->e_flags & EF_BFIN_CODE_IN_L1)
      && !(ppnt->p_flags & PF_W)
      && (ppnt->p_flags & PF_X))
    return 1;

  if ((epnt->e_flags & EF_BFIN_DATA_IN_L1)
      && (ppnt->p_flags & PF_W)
      && !(ppnt->p_flags & PF_X))
    return 1;

  /* 0xfeb00000, 0xfec00000, 0xff700000, 0xff800000, 0xff900000,
     and 0xffa00000 are also used in GNU ld and linux kernel.
     They need to be kept synchronized.  */
  if (ppnt->p_vaddr == 0xff700000
      || ppnt->p_vaddr == 0xff800000
      || ppnt->p_vaddr == 0xff900000
      || ppnt->p_vaddr == 0xffa00000
      || ppnt->p_vaddr == 0xfeb00000
      || ppnt->p_vaddr == 0xfec00000)
    return 1;

  return 0;
}

static __always_inline char *
__dl_map_segment (Elf32_Ehdr *epnt,
		  Elf32_Phdr *ppnt,
		  int infile,
		  int flags)
{
  char *status, *tryaddr, *addr;
  size_t size;

  if (((epnt->e_flags & EF_BFIN_CODE_IN_L1) || ppnt->p_vaddr == 0xffa00000)
      && !(ppnt->p_flags & PF_W)
      && (ppnt->p_flags & PF_X)) {
    status = (char *) _dl_mmap
      (tryaddr = 0,
       size = (ppnt->p_vaddr & ADDR_ALIGN) + ppnt->p_filesz,
       LXFLAGS(ppnt->p_flags),
       flags | MAP_EXECUTABLE | MAP_DENYWRITE,
       infile, ppnt->p_offset & OFFS_ALIGN);
    if (_dl_mmap_check_error(status)
	|| (tryaddr && tryaddr != status))
      return NULL;
    addr = (char *) _dl_sram_alloc (ppnt->p_filesz, L1_INST_SRAM);
    if (addr != NULL)
      _dl_dma_memcpy (addr, status + (ppnt->p_vaddr & ADDR_ALIGN), ppnt->p_filesz);
    _dl_munmap (status, size);
    if (addr == NULL)
      _dl_dprintf(2, "%s:%i: L1 allocation failed\n", _dl_progname, __LINE__);
    return addr;
  }

  if (((epnt->e_flags & EF_BFIN_DATA_IN_L1)
       || ppnt->p_vaddr == 0xff700000
       || ppnt->p_vaddr == 0xff800000
       || ppnt->p_vaddr == 0xff900000)
      && (ppnt->p_flags & PF_W)
      && !(ppnt->p_flags & PF_X)) {
    if (ppnt->p_vaddr == 0xff800000)
      addr = (char *) _dl_sram_alloc (ppnt->p_memsz, L1_DATA_A_SRAM);
    else if (ppnt->p_vaddr == 0xff900000)
      addr = (char *) _dl_sram_alloc (ppnt->p_memsz, L1_DATA_B_SRAM);
    else
      addr = (char *) _dl_sram_alloc (ppnt->p_memsz, L1_DATA_SRAM);
    if (addr == NULL) {
      _dl_dprintf(2, "%s:%i: L1 allocation failed\n", _dl_progname, __LINE__);
    } else {
      if (_DL_PREAD (infile, addr, ppnt->p_filesz, ppnt->p_offset) != ppnt->p_filesz) {
        _dl_sram_free (addr);
        return NULL;
      }
      if (ppnt->p_filesz < ppnt->p_memsz)
       _dl_memset (addr + ppnt->p_filesz, 0, ppnt->p_memsz - ppnt->p_filesz);
    }
    return addr;
  }

  if (ppnt->p_vaddr == 0xfeb00000
      || ppnt->p_vaddr == 0xfec00000) {
    addr = (char *) _dl_sram_alloc (ppnt->p_memsz, L2_SRAM);
    if (addr == NULL) {
      _dl_dprintf(2, "%s:%i: L2 allocation failed\n", _dl_progname, __LINE__);
    } else {
      if (_DL_PREAD (infile, addr, ppnt->p_filesz, ppnt->p_offset) != ppnt->p_filesz) {
        _dl_sram_free (addr);
        return NULL;
      }
      if (ppnt->p_filesz < ppnt->p_memsz)
       _dl_memset (addr + ppnt->p_filesz, 0, ppnt->p_memsz - ppnt->p_filesz);
    }
    return addr;
  }

  return 0;
}
