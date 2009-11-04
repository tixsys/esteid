#!/bin/sh

find -type f | grep -v -e "/\.svn/" -e "/tags/" | while read file ; do [ "`head -c3 -- "$file"`" == $'\xef\xbb\xbf' ] && echo -n "found BOM in: $file : " && file -b "$file" ; done
