#!/bin/bash

JAVA=java

dir=`dirname $0`

for i in $dir/lib/*.jar $dir/dist/*.jar; do
	CLASSPATH="$CLASSPATH:$i"
done

# java -Xmx512m -classpath $CLASSPATH ee.sk.test.jdigidoc -config jar://jdigidoc.cfg "$@"
$JAVA -Xmx512m -classpath $CLASSPATH ee.sk.test.jdigidoc -config $dir/jdigidoc.cfg "$@"
