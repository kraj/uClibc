     /* Copyright (C) 2003, 2004 Red Hat, Inc.
	Contributed by Alexandre Oliva <aoliva@redhat.com>
	Based on ../i386/dl-sysdep.h

This file is part of uClibc.

uClibc is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

uClibc is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with uClibc; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
USA.  */

/*
 * Various assembly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

/*
 * Define this if the system uses RELOCA.
 */
#undef ELF_USES_RELOCA

/*
 * Initialization sequence for a GOT.  Copy the resolver function
 * descriptor and the pointer to the elf_resolve/link_map data
 * structure.  Initialize the got_value in the module while at that.
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  (MODULE)->loadaddr.got_value = (GOT_BASE); \
  GOT_BASE[0] = ((unsigned long *)&_dl_linux_resolve)[0]; \
  GOT_BASE[1] = ((unsigned long *)&_dl_linux_resolve)[1]; \
  GOT_BASE[2] = (unsigned long) MODULE; \
}

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_CYGNUS_FRV
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "FR-V"

struct elf_resolve;

struct funcdesc_value
{
  void *entry_point;
  void *got_value;
} __attribute__((__aligned__(8)));


extern int _dl_linux_resolve(void) __attribute__((__visibility__("hidden")));

#define do_rem(result, n, base)  result = (n % base)

/* 4096 bytes alignment */
#define PAGE_ALIGN 0xfffff000
#define ADDR_ALIGN 0xfff
#define OFFS_ALIGN 0x7ffff000

struct funcdesc_ht;

/* We must force strings used early in the bootstrap into the data
   segment, such that they are referenced with GOTOFF instead of
   GPREL, because GPREL needs the GOT to have already been
   relocated.  */
#define SEND_EARLY_STDERR(S) \
  do { static char __s[] = (S); SEND_STDERR (__s); } while (0)

#include <bits/elf-fdpic.h>
#ifdef __USE_GNU
# include <link.h>
#else
# define __USE_GNU
# include <link.h>
# undef __USE_GNU
#endif
#include <dl-syscall.h>
#include <dl-string.h>

/* These are declared in ldso.h, after it includes dl-elf.h that
   includes ourselves.  */
extern void *_dl_malloc(int size);
extern void _dl_free(void *);
extern void _dl_dprintf(int, const char *, ...);


#ifndef _dl_assert
# define _dl_assert(expr)
#endif

/* Initialize a DL_LOADADDR_TYPE given a got pointer and a complete
   load map.  */
inline static void
__dl_init_loadaddr_map (struct elf32_fdpic_loadaddr *loadaddr, void *got_value,
			struct elf32_fdpic_loadmap *map)
{
  if (map->version != 0)
    {
      SEND_EARLY_STDERR ("Invalid loadmap version number\n");
      _dl_exit(-1);
    }
  if (map->nsegs == 0)
    {
      SEND_EARLY_STDERR ("Invalid segment count in loadmap\n");
      _dl_exit(-1);
    }
  loadaddr->got_value = got_value;
  loadaddr->map = map;
}

/* Figure out how many LOAD segments there are in the given headers,
   and allocate a block for the load map big enough for them.
   got_value will be properly initialized later on, with INIT_GOT.  */
inline static int
__dl_init_loadaddr (struct elf32_fdpic_loadaddr *loadaddr, Elf32_Phdr *ppnt,
		    int pcnt)
{
  int count = 0, i;
  size_t size;

  for (i = 0; i < pcnt; i++)
    if (ppnt[i].p_type == PT_LOAD)
      count++;

  loadaddr->got_value = 0;

  size = sizeof (struct elf32_fdpic_loadmap)
    + sizeof (struct elf32_fdpic_loadseg) * count;
  loadaddr->map = _dl_malloc (size);
  if (! loadaddr->map)
    _dl_exit (-1);

  loadaddr->map->version = 0;
  loadaddr->map->nsegs = 0;

  return count;
}

/* Incrementally initialize a load map.  */
inline static void
__dl_init_loadaddr_hdr (struct elf32_fdpic_loadaddr loadaddr, void *addr,
			Elf32_Phdr *phdr, int maxsegs)
{
  struct elf32_fdpic_loadseg *segdata;

  if (loadaddr.map->nsegs == maxsegs)
    _dl_exit (-1);

  segdata = &loadaddr.map->segs[loadaddr.map->nsegs++];
  segdata->addr = (Elf32_Addr) addr;
  segdata->p_vaddr = phdr->p_vaddr;
  segdata->p_memsz = phdr->p_memsz;

#if defined (__SUPPORT_LD_DEBUG__)
  {
    extern char *_dl_debug;
    extern int _dl_debug_file;
    if (_dl_debug)
      _dl_dprintf(_dl_debug_file, "%i: mapped %x at %x, size %x\n",
		  loadaddr.map->nsegs-1,
		  segdata->p_vaddr, segdata->addr, segdata->p_memsz);
  }
#endif
}

inline static void __dl_loadaddr_unmap
(struct elf32_fdpic_loadaddr loadaddr, struct funcdesc_ht *funcdesc_ht);

/* Figure out whether the given address is in one of the mapped
   segments.  */
inline static int
__dl_addr_in_loadaddr (void *p, struct elf32_fdpic_loadaddr loadaddr)
{
  struct elf32_fdpic_loadmap *map = loadaddr.map;
  int c;

  for (c = 0; c < map->nsegs; c++)
    if ((void*)map->segs[c].addr <= p
	&& (char*)p < (char*)map->segs[c].addr + map->segs[c].p_memsz)
      return 1;

  return 0;
}

inline static void * _dl_funcdesc_for (void *entry_point, void *got_value);

#define DL_LOADADDR_TYPE struct elf32_fdpic_loadaddr

#define DL_RELOC_ADDR(ADDR, LOADADDR) \
  (__reloc_pointer ((void*)(ADDR), (LOADADDR).map))

#define DL_ADDR_TO_FUNC_PTR(ADDR, LOADADDR) \
  ((void(*)(void)) _dl_funcdesc_for ((void*)(ADDR), (LOADADDR).got_value))

#define _dl_stabilize_funcdesc(val) \
  ({ asm ("" : "+m" (*(val))); (val); })

#define DL_CALL_FUNC_AT_ADDR(ADDR, LOADADDR, SIGNATURE, ...) \
  ({ struct funcdesc_value fd = { (void*)(ADDR), (LOADADDR).got_value }; \
     void (*pf)(void) = (void*) _dl_stabilize_funcdesc (&fd); \
     (* SIGNATURE pf)(__VA_ARGS__); })

#define DL_INIT_LOADADDR_BOOT(LOADADDR, BASEADDR) \
  (__dl_init_loadaddr_map (&(LOADADDR), dl_boot_got_pointer, \
			   dl_boot_ldsomap ?: dl_boot_progmap))

#define DL_INIT_LOADADDR_PROG(LOADADDR, BASEADDR) \
  (__dl_init_loadaddr_map (&(LOADADDR), 0, dl_boot_progmap))

#define DL_INIT_LOADADDR_EXTRA_DECLS \
  int dl_init_loadaddr_load_count;
#define DL_INIT_LOADADDR(LOADADDR, BASEADDR, PHDR, PHDRCNT) \
  (dl_init_loadaddr_load_count = \
     __dl_init_loadaddr (&(LOADADDR), (PHDR), (PHDRCNT)))
#define DL_INIT_LOADADDR_HDR(LOADADDR, ADDR, PHDR) \
  (__dl_init_loadaddr_hdr ((LOADADDR), (ADDR), (PHDR), \
			   dl_init_loadaddr_load_count))
#define DL_LOADADDR_UNMAP(LOADADDR, LEN) \
  (__dl_loadaddr_unmap ((LOADADDR), (NULL)))
#define DL_LIB_UNMAP(LIB, LEN) \
  (__dl_loadaddr_unmap ((LIB)->loadaddr, (LIB)->funcdesc_ht))
#define DL_LOADADDR_BASE(LOADADDR) \
  ((LOADADDR).got_value)

#define DL_ADDR_IN_LOADADDR(ADDR, TPNT, TFROM) \
  (! (TFROM) && __dl_addr_in_loadaddr ((void*)(ADDR), (TPNT)->loadaddr))

/* We only support loading FDPIC independently-relocatable shared
   libraries.  It probably wouldn't be too hard to support loading
   shared libraries that require relocation by the same amount, but we
   don't know that they exist or would be useful, and the dynamic
   loader code could leak the whole-library map unless we keeping a
   bit more state for DL_LOADADDR_UNMAP and DL_LIB_UNMAP, so let's
   keep things simple for now.  */
#define DL_CHECK_LIB_TYPE(epnt, piclib, _dl_progname, libname) \
do \
{ \
  if (((epnt)->e_flags & EF_FRV_FDPIC) && ! ((epnt)->e_flags & EF_FRV_PIC)) \
    (piclib) = 2; \
  else \
    { \
      _dl_internal_error_number = LD_ERROR_NOTDYN; \
      _dl_dprintf(2, "%s: '%s' is not an FDPIC shared library" \
		  "\n", (_dl_progname), (libname)); \
      _dl_close(infile); \
      return NULL; \
    } \
} \
while (0)  

/* We want want to apply all relocations in the interpreter during
   bootstrap.  Because of this, we have to skip the interpreter
   relocations in _dl_parse_relocation_information(), see
   elfinterp.c.  */
#define DL_SKIP_BOOTSTRAP_RELOC(SYMTAB, INDEX, STRTAB) 0

#ifdef __NR_pread
#define _DL_PREAD(FD, BUF, SIZE, OFFSET) \
  (_dl_pread((FD), (BUF), (SIZE), (OFFSET)))
#endif

#include <dl-hash.h>

/* The hashcode handling code below is heavily inspired in libiberty's
   hashtab code, but with most adaptation points and support for
   deleting elements removed.

   Copyright (C) 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
   Contributed by Vladimir Makarov (vmakarov@cygnus.com).  */

inline static unsigned long
higher_prime_number (unsigned long n)
{
  /* These are primes that are near, but slightly smaller than, a
     power of two.  */
  static const unsigned long primes[] = {
    (unsigned long) 7,
    (unsigned long) 13,
    (unsigned long) 31,
    (unsigned long) 61,
    (unsigned long) 127,
    (unsigned long) 251,
    (unsigned long) 509,
    (unsigned long) 1021,
    (unsigned long) 2039,
    (unsigned long) 4093,
    (unsigned long) 8191,
    (unsigned long) 16381,
    (unsigned long) 32749,
    (unsigned long) 65521,
    (unsigned long) 131071,
    (unsigned long) 262139,
    (unsigned long) 524287,
    (unsigned long) 1048573,
    (unsigned long) 2097143,
    (unsigned long) 4194301,
    (unsigned long) 8388593,
    (unsigned long) 16777213,
    (unsigned long) 33554393,
    (unsigned long) 67108859,
    (unsigned long) 134217689,
    (unsigned long) 268435399,
    (unsigned long) 536870909,
    (unsigned long) 1073741789,
    (unsigned long) 2147483647,
					/* 4294967291L */
    ((unsigned long) 2147483647) + ((unsigned long) 2147483644),
  };

  const unsigned long *low = &primes[0];
  const unsigned long *high = &primes[sizeof(primes) / sizeof(primes[0])];

  while (low != high)
    {
      const unsigned long *mid = low + (high - low) / 2;
      if (n > *mid)
	low = mid + 1;
      else
	high = mid;
    }

#if 0
  /* If we've run out of primes, abort.  */
  if (n > *low)
    {
      fprintf (stderr, "Cannot find prime bigger than %lu\n", n);
      abort ();
    }
#endif

  return *low;
}

struct funcdesc_ht
{
  /* Table itself.  */
  struct funcdesc_value **entries;

  /* Current size (in entries) of the hash table */
  size_t size;

  /* Current number of elements.  */
  size_t n_elements;
};  

inline static int
hash_pointer (const void *p)
{
  return (int) ((long)p >> 3);
}

inline static struct funcdesc_ht *
htab_create (void)
{
  struct funcdesc_ht *ht = _dl_malloc (sizeof (struct funcdesc_ht));

  if (! ht)
    return NULL;
  ht->size = 3;
  ht->entries = _dl_malloc (sizeof (struct funcdesc_ht_value *) * ht->size);
  if (! ht->entries)
    return NULL;
  
  ht->n_elements = 0;

  _dl_memset (ht->entries, 0, sizeof (struct funcdesc_ht_value *) * ht->size);
  
  return ht;
}

inline static void
htab_delete (struct funcdesc_ht *htab)
{
  int i;

  for (i = htab->size - 1; i >= 0; i--)
    if (htab->entries[i])
      _dl_free (htab->entries[i]);

  _dl_free (htab->entries);
  _dl_free (htab);
}

/* Similar to htab_find_slot, but without several unwanted side effects:
    - Does not call htab->eq_f when it finds an existing entry.
    - Does not change the count of elements/searches/collisions in the
      hash table.
   This function also assumes there are no deleted entries in the table.
   HASH is the hash value for the element to be inserted.  */

inline static struct funcdesc_value **
find_empty_slot_for_expand (struct funcdesc_ht *htab, int hash)
{
  size_t size = htab->size;
  unsigned int index = hash % size;
  struct funcdesc_value **slot = htab->entries + index;
  int hash2;

  if (! *slot)
    return slot;

  hash2 = 1 + hash % (size - 2);
  for (;;)
    {
      index += hash2;
      if (index >= size)
	index -= size;

      slot = htab->entries + index;
      if (! *slot)
	return slot;
    }
}

/* The following function changes size of memory allocated for the
   entries and repeatedly inserts the table elements.  The occupancy
   of the table after the call will be about 50%.  Naturally the hash
   table must already exist.  Remember also that the place of the
   table entries is changed.  If memory allocation failures are allowed,
   this function will return zero, indicating that the table could not be
   expanded.  If all goes well, it will return a non-zero value.  */

inline static int
htab_expand (struct funcdesc_ht *htab)
{
  struct funcdesc_value **oentries;
  struct funcdesc_value **olimit;
  struct funcdesc_value **p;
  struct funcdesc_value **nentries;
  size_t nsize;

  oentries = htab->entries;
  olimit = oentries + htab->size;

  /* Resize only when table after removal of unused elements is either
     too full or too empty.  */
  if (htab->n_elements * 2 > htab->size)
    nsize = higher_prime_number (htab->n_elements * 2);
  else
    nsize = htab->size;

  nentries = _dl_malloc (sizeof (struct funcdesc_value *) * nsize);
  _dl_memset (nentries, 0, sizeof (struct funcdesc_value *) * nsize);
  if (nentries == NULL)
    return 0;
  htab->entries = nentries;
  htab->size = nsize;

  p = oentries;
  do
    {
      if (*p)
	*find_empty_slot_for_expand (htab, hash_pointer ((*p)->entry_point))
	  = *p;

      p++;
    }
  while (p < olimit);

  _dl_free (oentries);
  return 1;
}

/* This function searches for a hash table slot containing an entry
   equal to the given element.  To delete an entry, call this with
   INSERT = 0, then call htab_clear_slot on the slot returned (possibly
   after doing some checks).  To insert an entry, call this with
   INSERT = 1, then write the value you want into the returned slot.
   When inserting an entry, NULL may be returned if memory allocation
   fails.  */

inline static struct funcdesc_value **
htab_find_slot (struct funcdesc_ht *htab, void *ptr)
{
  unsigned int index;
  int hash, hash2;
  size_t size;
  struct funcdesc_value **entry;

  if (htab->size * 3 <= htab->n_elements * 4
      && htab_expand (htab) == 0)
    return NULL;

  hash = hash_pointer (ptr);

  size = htab->size;
  index = hash % size;

  entry = &htab->entries[index];
  if (!*entry)
    goto empty_entry;
  else if ((*entry)->entry_point == ptr)
    return entry;
      
  hash2 = 1 + hash % (size - 2);
  for (;;)
    {
      index += hash2;
      if (index >= size)
	index -= size;
      
      entry = &htab->entries[index];
      if (!*entry)
	goto empty_entry;
      else if ((*entry)->entry_point == ptr)
	return entry;
    }

 empty_entry:
  htab->n_elements++;
  return entry;
}

void *
_dl_funcdesc_for (void *entry_point, void *got_value)
{
  struct elf_resolve *tpnt = ((void**)got_value)[2];
  struct funcdesc_ht *ht = tpnt->funcdesc_ht;
  struct funcdesc_value **entry;

  _dl_assert (got_value == tpnt->loadaddr.got_value);

  if (! ht)
    {
      ht = htab_create ();
      if (! ht)
	return (void*)-1;
      tpnt->funcdesc_ht = ht;
    }

  entry = htab_find_slot (ht, entry_point);
  if (*entry)
    {
      _dl_assert ((*entry)->entry_point == entry_point);
      return _dl_stabilize_funcdesc (*entry);
    }

  *entry = _dl_malloc (sizeof (struct funcdesc_value));
  (*entry)->entry_point = entry_point;
  (*entry)->got_value = got_value;

  return _dl_stabilize_funcdesc (*entry);
}

void
__dl_loadaddr_unmap (struct elf32_fdpic_loadaddr loadaddr,
		     struct funcdesc_ht *funcdesc_ht)
{
  int i;

  for (i = 0; i < loadaddr.map->nsegs; i++)
    _dl_munmap ((void*)loadaddr.map->segs[i].addr,
		loadaddr.map->segs[i].p_memsz);

  _dl_free (loadaddr.map);
  if (funcdesc_ht)
    htab_delete (funcdesc_ht);
}
