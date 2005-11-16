#!/bin/sh

logfile=tst-tls6.out

LC_ALL=C
export LC_ALL
LANG=C
export LANG

> $logfile
fail=0

for aligned in a e f; do
  echo "preload tst-tls5mod{$aligned,b,c,d}.so" >> $logfile
  echo "===============" >> $logfile
  LD_PRELOAD=`echo tst-tls5mod{$aligned,b,c,d}.so \
	      | sed 's/:$//;s/: /:/g'` ${tst_tls5} >> $logfile || fail=1
  echo >> $logfile

  echo "preload tst-tls5mod{b,$aligned,c,d}.so" >> $logfile
  echo "===============" >> $logfile
  LD_PRELOAD=`echo tst-tls5mod{b,$aligned,c,d}.so \
	      | sed 's/:$//;s/: /:/g'` ${tst_tls5} >> $logfile || fail=1
  echo >> $logfile

  echo "preload tst-tls5mod{b,c,d,$aligned}.so" >> $logfile
  echo "===============" >> $logfile
  LD_PRELOAD=`echo tst-tls5mod{b,c,d,$aligned}.so \
	      | sed 's/:$//;s/: /:/g'` ${tst_tls5} >> $logfile || fail=1
  echo >> $logfile
done

echo "preload tst-tls5mod{d,a,b,c,e}" >> $logfile
echo "===============" >> $logfile
LD_PRELOAD=`echo tst-tls5mod{d,a,b,c,e}.so \
	    | sed 's/:$//;s/: /:/g'` ${tst_tls5} >> $logfile || fail=1
echo >> $logfile

echo "preload tst-tls5mod{d,a,b,e,f}" >> $logfile
echo "===============" >> $logfile
LD_PRELOAD=`echo tst-tls5mod{d,a,b,e,f}.so \
	    | sed 's/:$//;s/: /:/g'` ${tst_tls5} >> $logfile || fail=1
echo >> $logfile

exit $fail
