#ifndef _LIBC_SYMBOLS_H
#define _LIBC_SYMBOLS_H 1

#ifndef __SYMBOL_PREFIX
# ifdef NO_UNDERSCORES
#  define __SYMBOL_PREFIX
# else
#  define __SYMBOL_PREFIX "_"
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
#define __libc_freeres_fn_section \
  __attribute__ ((section ("__libc_freeres_fn")))
#define libc_freeres_fn(name)	\
  static void name (void) __attribute_used__ __libc_freeres_fn_section;	\
  text_set_element (__libc_subfreeres, name);				\
  static void name (void)

#if 0
# ifndef __ASSEMBLER__
#  if !defined HAVE_VISIBILITY_ATTRIBUTE \
      || defined HAVE_BROKEN_VISIBILITY_ATTRIBUTE
#   define __hidden_proto_hiddenattr(attrs...)
#  else
#   define __hidden_proto_hiddenattr(attrs...) \
  __attribute__ ((visibility ("hidden"), ##attrs))
#  endif
#  define hidden_proto(name, attrs...) \
  __hidden_proto (name, __GI_##name, ##attrs)
#  define __hidden_proto(name, internal, attrs...) \
  extern __typeof (name) name __asm__ (__hidden_asmname (#internal)) \
  __hidden_proto_hiddenattr (attrs);
#  define __hidden_asmname(name) \
  __hidden_asmname1 (__USER_LABEL_PREFIX__, name)
#  define __hidden_asmname1(prefix, name) __hidden_asmname2(prefix, name)
#  define __hidden_asmname2(prefix, name) #prefix name
#  define __hidden_ver1(local, internal, name) \
  extern __typeof (name) __EI_##name __asm__(__hidden_asmname (#internal)); \
  extern __typeof (name) __EI_##name \
	__attribute__((alias (__hidden_asmname (#local))))
#  define hidden_ver(local, name)	__hidden_ver1(local, __GI_##name, name);
#  define hidden_data_ver(local, name)	hidden_ver(local, name)
#  define hidden_def(name)		__hidden_ver1(__GI_##name, name, name);
#  define hidden_data_def(name)		hidden_def(name)
#  define hidden_weak(name) \
	__hidden_ver1(__GI_##name, name, name) __attribute__((weak));
#  define hidden_data_weak(name)	hidden_weak(name)
# else
/* For assembly, we need to do the opposite of what we do in C:
   in assembly gcc __REDIRECT stuff is not in place, so functions
   are defined by its normal name and we need to create the
   __GI_* alias to it, in C __REDIRECT causes the function definition
   to use __GI_* name and we need to add alias to the real name.
   There is no reason to use hidden_weak over hidden_def in assembly,
   but we provide it for consistency with the C usage.
   hidden_proto doesn't make sense for assembly but the equivalent
   is to call via the HIDDEN_JUMPTARGET macro instead of JUMPTARGET.  */
#  define hidden_def(name)	strong_alias (name, __GI_##name)
#  define hidden_weak(name)	hidden_def (name)
#  define hidden_ver(local, name) strong_alias (local, __GI_##name)
#  define hidden_data_def(name)	strong_data_alias (name, __GI_##name)
#  define hidden_data_weak(name)	hidden_data_def (name)
#  define hidden_data_ver(local, name) strong_data_alias (local, __GI_##name)
#  ifdef HAVE_ASM_GLOBAL_DOT_NAME
#   define HIDDEN_JUMPTARGET(name) .__GI_##name
#  else
#   define HIDDEN_JUMPTARGET(name) __GI_##name
#  endif
# endif
#else
# ifndef __ASSEMBLER__
#  define hidden_proto(name, attrs...)
# else
#  define HIDDEN_JUMPTARGET(name) JUMPTARGET(name)
# endif /* Not  __ASSEMBLER__ */
# define hidden_weak(name)
# define hidden_def(name)
# define hidden_ver(local, name)
# define hidden_data_weak(name)
# define hidden_data_def(name)
# define hidden_data_ver(local, name)
#endif

#if defined NOT_IN_libc && defined IS_IN_rtld
# define rtld_hidden_proto(name, attrs...) hidden_proto (name, ##attrs)
# define rtld_hidden_def(name) hidden_def (name)
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
# define libc_hidden_def(name) hidden_def (name)
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

#ifdef HAVE_Z_RELRO
# define attribute_relro __attribute__ ((section (".data.rel.ro")))
#else
# define attribute_relro
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

#endif /* _LIBC_SYMBOLS_H */
