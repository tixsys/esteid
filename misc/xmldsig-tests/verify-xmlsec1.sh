#!/bin/sh

XMLSEC=xmlsec1

$XMLSEC --verify --print-debug "$1"
