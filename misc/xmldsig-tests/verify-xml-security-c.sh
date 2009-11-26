#!/bin/sh

XSC=`dirname $0`/parsers/xml-security-c-1.5.1

$XSC/bin/checksig -x "$1"
