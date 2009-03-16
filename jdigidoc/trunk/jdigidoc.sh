#!/bin/bash
CLASSPATH=./JDigiDoc.jar:./lib/bcprov-jdk14-133.jar:./lib/jakarta-log4j-1.2.6.jar:./lib/iaikPkcs11Wrapper.jar:./lib/tinyxmlcanonicalizer-0.9.0.jar:.
$JAVA_HOME/bin/java -Xmx512m -classpath $CLASSPATH ee.sk.test.jdigidoc -config jar://jdigidoc.cfg $1 $2 $3 $4 $5 $6 $7 $8 $9 $10
