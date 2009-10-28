#!/bin/sh

export MOZ_NO_REMOTE=1

trunkdir=$(cd "`dirname $0`"; pwd)
bdir="$trunkdir/build"
tstdir=`cd "$trunkdir/../../tests"; pwd`
[ "$1" ] && xpi="$bdir/$1" || xpi="$bdir"/esteid-*.xpi
tmpdir=`mktemp -d /tmp/tbprofile.XXXXXXXX`
trap 'rm -rf "$tmpdir"' INT
xdir="$tmpdir/extensions/{aa84ce40-4253-11da-8cd6-0800200c9a66}"
mkdir -p "$xdir"
unzip -d "$xdir" "$xpi" > /dev/null

thunderbird -profile "$tmpdir"

rm -rf "$tmpdir"
