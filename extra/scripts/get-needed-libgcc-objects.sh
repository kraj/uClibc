#!/bin/sh
#
# Manuel Novoa III       Jan 2001
#
# The purpose of this script is to extract the object files from libgcc.a
# that are needed by the shared uClibc so that they won't be linked with
# each application.
#
# I'm sure people with better shell programming skills can improve this.
# Feel free!  ;-)  At this point though, it gets the job done for me.
#
# Possible problems (paranioa mode):  Are any of the objects in libgcc.a
# needed to actually load the shared library on any archs?

LIBGCC=`$CC -print-libgcc-file-name`

echo Finding missing symbols in libc.a ...
echo "    partial linking..."
rm -f libc.ldr
ld -r -o libc.ldr ../crt0.o *.o

if nm -s libc.ldr | grep -v " main$" | grep " U " > sym.need ; then
    rm -f obj.need
    touch obj.need
    for SYM in `cat sym.need | sed -e 's/ U //g'` ; do
	if nm -s $LIBGCC | grep -q $SYM" in " ; then
	    nm -s $LIBGCC | grep $SYM" in " | cut -d' ' -f3 >> obj.need
	else
	    echo Symbol $SYM needed by libc.a but not found in libgcc.a
	fi
    done
else
    echo No missing symbols found.
    exit 0
fi

rm -rf tmp-gcc
mkdir tmp-gcc
(cd tmp-gcc ; ar -x $LIBGCC)
rm libgcc.ldr

echo Extracting referenced libgcc.a objects ...

rm -f obj.need.0
touch obj.need.0
while [ -s obj.need ] && ! cmp -s obj.need obj.need.0 ; do
    (cd tmp-gcc ; cat ../obj.need | xargs ld -r -o ../libgcc.ldr)
    cp obj.need obj.need.0
    if nm -s libgcc.ldr | grep " U " > sym.need ; then
	for SYM in `cat sym.need | sed -e 's/ U //g'` ; do
	    if nm -s $LIBGCC | grep -q $SYM" in " ; then
		nm -s $LIBGCC | grep $SYM" in " | cut -d' ' -f3 >> obj.need
	    fi
	done
    fi
done

cat obj.need | sort | uniq > obj.need.0
(cd tmp-gcc ; cp `cat ../obj.need` ..)

echo Objects added from $LIBGCC:
cat obj.need.0

if [ -s sym.need ] ; then
    echo Symbols missing from libgcc.a:
    cat sym.need
else
    echo Done
fi
exit 0