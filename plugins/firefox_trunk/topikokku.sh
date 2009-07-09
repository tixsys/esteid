#!/bin/sh

pd1="Linux_x86_64-gcc3"
pd2="WINNT_x86-msvc"
pd3="Linux_x86-gcc3"
pd4="WINNT_x86_64-msvc"

nplatforms=2

if [ "$#" != $nplatforms ]; then
	echo -n "Usage: $1 "
	a=1; while [ $a -le $nplatforms ]; do
		eval echo -n \$pd$a ; echo -n " "
		a=`expr $a + 1`
	done
	echo
	exit 2
fi

a=1
while [ $a -le $nplatforms ]; do
	eval pdir=platform/\$pd$a
	eval sdir=\$$a
	mkdir -p $pdir/{components,plugins}
	cp $sdir/components/*.{so,dll} $pdir/components
	cp $sdir/plugins/*.{so,dll} $pdir/plugins
	a=`expr $a + 1`
done
