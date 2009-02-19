#!/bin/sh
# Parameters:
# $1 = source dir
# $2 = dst dir
# $top_builddir = well you guessed it

die_if_not_dir()
{
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
if ! test -x "$top_builddir/extra/scripts/unifdef"; then
	echo "Error: need '$top_builddir/extra/scripts/unifdef' executable"
	exit 1
fi


# Sanitize and copy uclibc headers
(
# We must cd, or else we'll prepend "$1" to filenames!
cd "$1" || exit 1
find . ! -name '.' -a ! -path '*/.*' | sed -e 's/^\.\///' -e '/^config\//d' \
	-e '/^config$/d'
) | \
(
IFS=''
while read -r filename; do
	if test -d "$1/$filename"; then
		mkdir -p "$2/$filename" 2>/dev/null
	else
		# NB: unifdef exits with 1 if output is not
		# exactly the same as input. That's ok.
		# Do not abort the script if unifdef "fails"!
		"$top_builddir/extra/scripts/unifdef" -UUCLIBC_INTERNAL "$1/$filename" \
		    | sed -e '/^\(rtld\|lib\(c\|m\|resolv\|dl\|intl\|rt\|nsl\|util\|crypt\|pthread\)\)_hidden_proto[ 	]*([a-zA-Z0-9_]*)$/d' >"$2/$filename"
	fi
done
)


# Fix mode/owner bits
cd "$2" || exit 1
chmod -R u=rwX,go=rX . >/dev/null 2>&1
chown -R `id | sed 's/^uid=\([0-9]*\).*gid=\([0-9]*\).*$/\1:\2/'` . >/dev/null 2>&1
