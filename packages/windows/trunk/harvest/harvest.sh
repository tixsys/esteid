#!/bin/sh

OPTS="-suid -dr APPLICATIONFOLDER -srd -var var.IESupportSourceDir"

for f in esteidcsp npesteid ; do
	echo "heat file $f.dll -out $f-Win32.wxs $OPTS"
done
