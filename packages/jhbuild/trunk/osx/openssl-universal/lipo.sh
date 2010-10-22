#!/bin/sh

set -e

for f in $(cd $1; find . -type f -print)
do
    cmd="lipo -create "
    for d in "$@"; do
        cmd="$cmd $d/$f"
    done
    cmd="$cmd -output $f"
    echo $cmd
    $cmd
done
