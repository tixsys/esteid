#!/bin/sh

CRTPATH=/etc/digidocpp/certs
tmpf=/tmp/id.crt
SKRESPONDER=http://ocsp.sk.ee
OPENXADES=http://www.openxades.org/cgi-bin/ocsp.cgi

# Read certificate from card if not specified on command line
if [ -z "$1" ]; then
	echo -n "Reading cert ... "
		pkcs15-tool -r 2 > $tmpf
	echo
else
	cp "$1" "$tmpf"
fi

if [ ! -s $tmpf ]; then
	echo "Invalid certificate"
	rm -f $tmpf
	exit 2
fi

# Find proper responder Cert & URL
text=`openssl x509 -in /tmp/id.crt -text -noout`
if echo "$text" | grep -q '^ *Issuer: .*ESTEID.*$'; then
	ca="ESTEID-SK 2007.crt"
	resp="ESTEID-SK 2007 OCSP.crt"
	url="$SKRESPONDER"
fi
if echo "$text" | grep -q '^ *Issuer: .*KLASS3-SK.*$'; then
	ca="KLASS3-SK.crt"
	resp="KLASS3-SK OCSP 2009.crt"
	url="$SKRESPONDER"
fi
if echo "$text" | grep -q '^ *Issuer: .*TEST-SK.*$'; then
	ca="TEST-SK.crt"
	resp="TEST-SK OCSP 2005.crt"
	url="$OPENXADES"
fi

if [ -z "$ca" ]; then
	echo -n "Unknown "
	echo "$text" | grep '^ *Issuer:'
	rm -f $tmpf
	exit 2
fi

echo "$text" | grep '^ *Subject:\|^ *Issuer:'
echo

echo "Using certs: $ca $resp"
echo "Sending OCSP request to $url"

openssl ocsp	-issuer "$CRTPATH/$ca" \
		-cert "$tmpf" \
		-url "$url" \
		-VAfile "$CRTPATH/$resp"
