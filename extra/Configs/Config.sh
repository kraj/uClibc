#
# For a description of the syntax of this configuration file,
# see Documentation/kbuild/config-language.txt.
#

mainmenu "uClibc C Library Configuration"

menu "Target Architecture Features and Options"

config HAVE_ELF
	bool
	default y

config C_SYMBOL_PREFIX
	string
	default ""

config ARCH_CFLAGS
	string
	default ""

config ARCH_LDFLAGS
	string
	default ""

config LIBGCC_CFLAGS
	string
	default ""

config HAVE_DOT_HIDDEN
        bool
	default y

choice
	prompt "Target Processor Type"
	default CONFIG_GENERIC_ARM
	help
	  This is the processor type of your CPU. This information is used for
	  optimizing purposes, as well as to determine if your CPU has an MMU,
	  an FPU, etc.  If you pick the wrong CPU type, there is no guarantee
	  that uClibc will work at all....

	  Here are the available choices:
	  - "SH2" Hitachi SH2
	  - "SH3" Hitachi SH3
	  - "SH4" Hitachi SH4
	  - "SH5" Hitachi SH5

config CONFIG_SH2
	bool "SH2"

config CONFIG_SH3
	bool "SH3"

config CONFIG_SH4
	bool "SH4"

config CONFIG_SH5
	bool "SH5"

endchoice

choice
	prompt "Target Processor Endianness"
	default ARCH_LITTLE_ENDIAN
	help
	  This is the endianness you wish to build use.  Choose either Big
	  Endian, or Little Endian.

config ARCH_LITTLE_ENDIAN
	bool "Little Endian"

config ARCH_BIG_ENDIAN
	bool "Big Endian"

endchoice


config ARCH_HAS_NO_MMU
	bool
	default y if CONFIG_SH2 || CONFIG_SH3

config CPU_CFLAGS
	string
	default "-DHIOS -ml -m2" if CONFIG_SH2 && ARCH_LITTLE_ENDIAN
	default "-DHIOS -mb -m2" if CONFIG_SH2 && ARCH_BIG_ENDIAN
	default "-DHIOS -ml -m3" if CONFIG_SH3 && ARCH_LITTLE_ENDIAN
	default "-DHIOS -mb -m3" if CONFIG_SH3 && ARCH_BIG_ENDIAN
	default "-DHIOS -ml -m4" if CONFIG_SH4 && ARCH_LITTLE_ENDIAN
	default "-DHIOS -mb -m4" if CONFIG_SH4 && ARCH_BIG_ENDIAN
	default "-DHIOS -ml -m5" if CONFIG_SH5 && ARCH_LITTLE_ENDIAN
	default "-DHIOS -mb -m5" if CONFIG_SH5 && ARCH_BIG_ENDIAN

source "extra/Configs/Config.in.arch"

endmenu

source "extra/Configs/Config.in"


