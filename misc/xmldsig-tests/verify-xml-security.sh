#!/bin/sh
dir=`dirname "$0"`
XS="$dir/parsers/xml-security-1_4_3/"

for i in "$XS"/libs/*.jar; do
	CP="$CP:$i"
done

cp -f "$1" signature.xml && \
java -classpath "$CP" -Djava.util.logging.config.file="$dir"/logging.properties org.apache.xml.security.samples.signature.VerifySignature

rm -f signature.xml
