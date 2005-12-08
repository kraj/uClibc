#ifndef _LIBC_SYMBOLS_H
#define _LIBC_SYMBOLS_H 1

#ifndef __SYMBOL_PREFIX
# ifndef C_SYMBOL_PREFIX
#  define __SYMBOL_PREFIX
# else
#  define __SYMBOL_PREFIX	C_SYMBOL_PREFIX
# endif
#endif

/* Handling on non-exported internal names.  We have to do this only
   for shared code.  */
#ifdef SHARED
# define INTUSE(name) name##_internal
# define INTDEF(name) strong_alias (name, name##_internal)
# define INTVARDEF(name) \
  _INTVARDEF (name, name##_internal)
# if defined HAVE_VISIBILITY_ATTRIBUTE
#  define _INTVARDEF(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name), \
						   visibility ("hidden")));
# else
#  define _INTVARDEF(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));
# endif
# define INTDEF2(name, newname) strong_alias (name, newname##_internal)
# define INTVARDEF2(name, newname) _INTVARDEF (name, newname##_internal)
#else
# define INTUSE(name) name
# define INTDEF(name)
# define INTVARDEF(name)
# define INTDEF2(name, newname)
# define INTVARDEF2(name, newname)
#endif

/* These are all done the same way in ELF.
   There is a new section created for each set.  */
#  ifdef SHARED
/* When building a shared library, make the set section writable,
   because it will need to be relocated at run time anyway.  */
#   define _elf_set_element(set, symbol) \
  static const void *__elf_set_##set##_element_##symbol##__ \
    __attribute__ ((used, section (#set))) = &(symbol)
#  else
#   define _elf_set_element(set, symbol) \
  static const void *const __elf_set_##set##_element_##symbol##__ \
    __attribute__ ((used, section (#set))) = &(symbol)
#  endif

#define text_set_element(set, symbol) _elf_set_element(set, symbol)
#define __sec_comment "\n\t#"

# ifndef __ASSEMBLER__
#  define hidden_proto(name, attrs...)
# else
#  define HIDDEN_JUMPTARGET(name) JUMPTARGET(name)
# endif /* Not  __ASSEMBLER__ */
# define hidden_weak(name)
# define hidden_ver(local, name)
# define hidden_data_weak(name)
# define hidden_data_def(name)
# define hidden_data_ver(local, name)

#if defined NOT_IN_libc && defined IS_IN_rtld
# define rtld_hidden_proto(name, attrs...) hidden_proto (name, ##attrs)
# define rtld_hidden_def(name)
# define rtld_hidden_weak(name) hidden_weak (name)
# define rtld_hidden_ver(local, name) hidden_ver (local, name)
# define rtld_hidden_data_def(name) hidden_data_def (name)
# define rtld_hidden_data_weak(name) hidden_data_weak (name)
# define rtld_hidden_data_ver(local, name) hidden_data_ver (local, name)
#else
# define rtld_hidden_proto(name, attrs...)
# define rtld_hidden_def(name)
# define rtld_hidden_weak(name)
# define rtld_hidden_ver(local, name)
# define rtld_hidden_data_def(name)
# define rtld_hidden_data_weak(name)
# define rtld_hidden_data_ver(local, name)
#endif

#if !defined NOT_IN_libc
# define libc_hidden_proto(name, attrs...) hidden_proto (name, ##attrs)
# define libc_hidden_def(name)
# define libc_hidden_weak(name) hidden_weak (name)
# define libc_hidden_ver(local, name) hidden_ver (local, name)
# define libc_hidden_data_def(name) hidden_data_def (name)
# define libc_hidden_data_weak(name) hidden_data_weak (name)
# define libc_hidden_data_ver(local, name) hidden_data_ver (local, name)
#else
# define libc_hidden_proto(name, attrs...)
# define libc_hidden_def(name)
# define libc_hidden_weak(name)
# define libc_hidden_ver(local, name)
# define libc_hidden_data_def(name)
# define libc_hidden_data_weak(name)
# define libc_hidden_data_ver(local, name)
#endif

#ifdef HAVE_TLS_MODEL_ATTRIBUTE
# define attribute_tls_model_ie __attribute__ ((tls_model ("initial-exec")))
#else
# define attribute_tls_model_ie
#endif

/* Define SET as a symbol set.  This may be required (it is in a.out) to
   be able to use the set's contents.  */
#  define symbol_set_define(set)	symbol_set_declare(set)

/* Declare SET for use in this module, if defined in another module.
   In a shared library, this is always local to that shared object.
   For static linking, the set might be wholly absent and so we use
   weak references.  */
#  define symbol_set_declare(set) \
  extern char const __start_##set[] __symbol_set_attribute; \
  extern char const __stop_##set[] __symbol_set_attribute;

#  define __symbol_set_attribute attribute_hidden

/* Return a pointer (void *const *) to the first element of SET.  */
#  define symbol_set_first_element(set)	((void *const *) (&__start_##set))

/* Return true iff PTR (a void *const *) has been incremented
   past the last element in SET.  */
#  define symbol_set_end_p(set, ptr) ((ptr) >= (void *const *) &__stop_##set)

/* Not compiling ELF shared libraries at all, so never any old versions.  */
# define SHLIB_COMPAT(lib, introduced, obsoleted)	0

/* No versions to worry about, just make this the global definition.  */
# define versioned_symbol(lib, local, symbol, version) \
  weak_alias (local, symbol)

/* This should not appear outside `#if SHLIB_COMPAT (...)'.  */
# define compat_symbol(lib, local, symbol, version) ...

/* Move compatibility symbols out of the way by placing them all in a
 *    special section.  */
#ifndef __ASSEMBLER__
# define attribute_compat_text_section \
	    __attribute__ ((section (".text.compat")))
# define attribute_compat_data_section \
	    __attribute__ ((section (".data.compat")))
#else
# define compat_text_section .section ".text.compat", "ax";
# define compat_data_section .section ".data.compat", "aw";
#endif

#endif /* _LIBC_SYMBOLS_H */
