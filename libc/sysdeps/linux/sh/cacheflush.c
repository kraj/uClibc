/*
 * cacheflush syscall for SUPERH
 *
 * Copyright (C) 2009 STMicroelectronics Ltd
 * Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
 */
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

#ifdef __NR_cacheflush
_syscall3(int, cacheflush, char *, addr, int, nbytes, int, op)
#endif
