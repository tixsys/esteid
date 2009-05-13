# automake for digidoc library 

MAINTAINERCLEANFILES = Makefile.in

digidoc_certs = sk-esteid-ocsp-responder.pem  sk-klass3-ocsp-responder.pem  \
	sk-root.pem sk-test.pem sk-esteid.pem sk-klass3.pem \
	test_ocsp_2006.cer test_ocsp_020305-040305.pem test_ocsp_040305-080308.pem \
	sk-test-ocsp-responder-2005.pem sk-esteid-ocsp-responder-2005.pem \
	sk-klass3-ocsp-responder-2006.cer sk-eid-ocsp-responder-2006.cer \
	tsa_ns.crt ts_cacert.crt ocsp_hash_resp1.pem \
	esteid-sk-2007.pem eid-sk-2007.pem \
	esteid-sk-2007-responder.pem eid-sk-2007-responder.pem
	

EXTRA_DIST = digidoc.conf.in $(digidoc_certs) libesteid-pkcs11.so

certdir=${pkgdatadir}/certs

cert_DATA = $(digidoc_certs)

sysconf_DATA = digidoc.conf

