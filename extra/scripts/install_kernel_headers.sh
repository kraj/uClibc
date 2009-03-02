#!/bin/sh
# Parameters:
# $1 = source dir
# $2 = dst dir
# $top_builddir = well you guessed it

die_if_not_dir()
{
	local dir
	for dir in "$@"; do
		test -d "$dir" && continue
		echo "Error: '$dir' is not a directory"
		exit 1
	done
}


# Ensure that created dirs/files have 755/644 perms
umask 022


# Sanity tests
die_if_not_dir "$1"
mkdir -p "$2" 2>/dev/null
die_if_not_dir "$2"
die_if_not_dir "$top_builddir"


# Just copy (no sanitization) some kernel headers.
eval `grep ^KERNEL_HEADERS "$top_builddir/.config"`
if ! test "$KERNEL_HEADERS" \
|| ! test -d "$KERNEL_HEADERS/asm" \
|| ! test -d "$KERNEL_HEADERS/linux" \
; then
	echo "Error: '$KERNEL_HEADERS' is not a directory containing kernel headers."
	echo "Check KERNEL_HEADERS= in your .config file."
	exit 1
fi
# Do the copying only if src and dst dirs are not the same.
# Be thorough: do not settle just for textual compare,
# and guard against "pwd" being handled as shell builtin.
# Double quoting looks weird, but it works (even bbox ash too).
if test "`(cd "$KERNEL_HEADERS"; env pwd)`" != "`(cd "$2"; env pwd)`"; then
	# NB: source or target files and directories may be symlinks,
	# and for all we know, good reasons.
	# We must work correctly in these cases. This includes "do not replace
	# target symlink with real directory" rule. So, no rm -rf here please.
	mkdir -p "$2/asm"   2>/dev/null
	mkdir -p "$2/linux" 2>/dev/null
	# Exists, but is not a dir? That's bad, bail out
	die_if_not_dir "$2/asm" "$2/linux"
	# cp -HL creates regular destination files even if sources are symlinks.
	# This is intended.
	# (NB: you need busybox 1.11.x for this. earlier ones are slightly buggy)
	cp -RHL "$KERNEL_HEADERS/asm"/*   "$2/asm"   || exit 1
	cp -RHL "$KERNEL_HEADERS/linux"/* "$2/linux" || exit 1
	# Linux 2.4 doesn't have it
	if test -d "$KERNEL_HEADERS/asm-generic"; then
		mkdir -p "$2/asm-generic" 2>/dev/null
		die_if_not_dir "$2/asm-generic"
		cp -RHL "$KERNEL_HEADERS/asm-generic"/* "$2/asm-generic" || exit 1
	fi
	# For paranoid reasons, we use explicit list of directories
	# which may be here. List last updated for linux-2.6.27:
	for dir in drm mtd rdma sound video; do
		if test -d "$KERNEL_HEADERS/$dir"; then
			mkdir -p "$2/$dir" 2>/dev/null
			die_if_not_dir "$2/$dir"
			cp -RHL "$KERNEL_HEADERS/$dir"/* "$2/$dir" || exit 1
		fi
	done
	if ! test -f "$2/linux/version.h"; then
		echo "Warning: '$KERNEL_HEADERS/linux/version.h' is not found"
		echo "in kernel headers directory specified in .config."
		echo "Some programs won't like that. Consider fixing it by hand."
	fi
fi


# Fix mode/owner bits
cd "$2" || exit 1
chmod -R u=rwX,go=rX . >/dev/null 2>&1
chown -R `id | sed 's/^uid=\([0-9]*\).*gid=\([0-9]*\).*$/\1:\2/'` . >/dev/null 2>&1
