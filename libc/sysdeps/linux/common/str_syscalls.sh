#!/bin/sh

echo "#include <stdio.h>"
echo "#include <stdlib.h>"
echo "#include \"../include/asm/unistd.h\""
echo
echo "int main(void) {"
echo
echo "#define __NR__exit __NR_exit"
echo "#define __NR___open __NR_open"
echo "#define __NR___ipc __NR_ipc"
echo "#define __NR__ioctl __NR_ioctl"
echo "#define __NR__fcntl __NR_fcntl"
echo "#define __NR__reboot __NR_reboot"
echo "#define __NR__mmap __NR_mmap"
echo "#define __NR__syslog __NR_syslog"
echo "#define __NR__stat __NR_stat"
echo "#define __NR__lstat __NR_lstat"
echo "#define __NR__fstat __NR_fstat"
echo "#define __NR__getdents __NR_getdents"
echo
sed -ne 's/^[^\/]*_syscall[0-9].*([^,]*, *\([^,)]*\).*/printf("#define __STR_NR_\1 \\\"%d\\\"\\n", __NR_\1);/gp' syscalls.c
echo
echo "printf(\"#define __STR_NR_exit     __STR_NR__exit\n\");"
echo "printf(\"#define __STR_NR_open    __STR_NR___open\n\");"
echo "printf(\"#define __STR_NR_ioctl    __STR_NR__ioctl\n\");"
echo "printf(\"#define __STR_NR_fcntl    __STR_NR__fcntl\n\");"
echo "printf(\"#define __STR_NR_reboot   __STR_NR__reboot\n\");"
echo "printf(\"#define __STR_NR_mmap     __STR_NR__mmap\n\");"
echo "printf(\"#define __STR_NR_syslog   __STR_NR__syslog\n\");"
echo "printf(\"#define __STR_NR_stat     __STR_NR__stat\n\");"
echo "printf(\"#define __STR_NR_lstat    __STR_NR__lstat\n\");"
echo "printf(\"#define __STR_NR_fstat    __STR_NR__fstat\n\");"
echo "printf(\"#define __STR_NR_getdents __STR_NR__getdents\n\");"
echo
echo "return EXIT_SUCCESS; }"
