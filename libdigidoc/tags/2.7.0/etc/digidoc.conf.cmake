#--------------------------------------------------
# @DIGIDOC_CONF_NAME@
# DigiDoc library global configuration file
#--------------------------------------------------

[ca]
CA_CERT_PATH=@DIGIDOC_DATA_DIR@/certs
CA_CERTS=8
CA_CERT_1=JUUR-SK.crt
CA_CERT_1_CN=Juur-SK
CA_CERT_2=ESTEID-SK.crt
CA_CERT_2_CN=ESTEID-SK
CA_CERT_3=TEST-SK.crt
CA_CERT_3_CN=TEST-SK
CA_CERT_4=KLASS3-SK.crt
CA_CERT_4_CN=KLASS3-SK
CA_CERT_5=EID-SK.crt
CA_CERT_5_CN=EID-SK
CA_CERT_6=ESTEID-SK 2007.crt
CA_CERT_6_CN=ESTEID-SK 2007
CA_CERT_7=EID-SK 2007.crt
CA_CERT_7_CN=EID-SK 2007
CA_CERT_8=KLASS3-SK 2010.crt
CA_CERT_8_CN=KLASS3-SK 2010


DIGIDOC_FORMAT=DIGIDOC-XML
DIGIDOC_VERSION=1.3

DIGIDOC_DEFAULT_DRIVER=1
DIGIDOC_DRIVERS=1
DIGIDOC_DRIVER_1_NAME=OpenSC
DIGIDOC_DRIVER_1_DESC=OpenSC projects PKCS#11 driver
DIGIDOC_DRIVER_1_FILE=@PKCS11_MODULE@
DIGIDOC_SIGNATURE_SLOT=1

SIGN_OCSP=0
USE_PROXY=0

DIGIDOC_OCSP_RESPONDER_CERTS=11

DIGIDOC_OCSP_RESPONDER_CERT_1=TEST-SK OCSP 2005.crt
DIGIDOC_OCSP_RESPONDER_CERT_1_CN=TEST-SK OCSP RESPONDER 2005
DIGIDOC_OCSP_RESPONDER_CERT_1_CA=TEST-SK
DIGIDOC_OCSP_RESPONDER_CERT_1_URL=http://www.openxades.org/cgi-bin/ocsp.cgi

DIGIDOC_OCSP_RESPONDER_CERT_2=KLASS3-SK OCSP 2009.crt
DIGIDOC_OCSP_RESPONDER_CERT_2_CN=KLASS3-SK OCSP RESPONDER 2009
DIGIDOC_OCSP_RESPONDER_CERT_2_CA=KLASS3-SK
DIGIDOC_OCSP_RESPONDER_CERT_2_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_3=ESTEID-SK OCSP 2005.crt
DIGIDOC_OCSP_RESPONDER_CERT_3_CN=ESTEID-SK OCSP RESPONDER 2005
DIGIDOC_OCSP_RESPONDER_CERT_3_CA=ESTEID-SK
DIGIDOC_OCSP_RESPONDER_CERT_3_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_4=ESTEID-SK 2007 OCSP.crt
DIGIDOC_OCSP_RESPONDER_CERT_4_CN=ESTEID-SK 2007 OCSP RESPONDER
DIGIDOC_OCSP_RESPONDER_CERT_4_CA=ESTEID-SK 2007
DIGIDOC_OCSP_RESPONDER_CERT_4_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_5=EID-SK 2007 OCSP.crt
DIGIDOC_OCSP_RESPONDER_CERT_5_CN=EID-SK 2007 OCSP RESPONDER
DIGIDOC_OCSP_RESPONDER_CERT_5_CA=EID-SK 2007
DIGIDOC_OCSP_RESPONDER_CERT_5_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_6=EID-SK OCSP 2006.crt
DIGIDOC_OCSP_RESPONDER_CERT_6_1=EID-SK OCSP.crt
DIGIDOC_OCSP_RESPONDER_CERT_6_CN=EID-SK OCSP RESPONDER
DIGIDOC_OCSP_RESPONDER_CERT_6_CA=EID-SK
DIGIDOC_OCSP_RESPONDER_CERT_6_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_7=ESTEID-SK OCSP.crt
DIGIDOC_OCSP_RESPONDER_CERT_7_CN=ESTEID-SK OCSP RESPONDER
DIGIDOC_OCSP_RESPONDER_CERT_7_CA=ESTEID-SK
DIGIDOC_OCSP_RESPONDER_CERT_7_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_8=KLASS3-SK OCSP 2006.crt
DIGIDOC_OCSP_RESPONDER_CERT_8_1=KLASS3-SK OCSP.crt
DIGIDOC_OCSP_RESPONDER_CERT_8_CN=KLASS3-SK OCSP RESPONDER
DIGIDOC_OCSP_RESPONDER_CERT_8_CA=KLASS3-SK
DIGIDOC_OCSP_RESPONDER_CERT_8_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_9=EID-SK 2007 OCSP 2010.crt
DIGIDOC_OCSP_RESPONDER_CERT_9_CN=EID-SK 2007 OCSP RESPONDER 2010
DIGIDOC_OCSP_RESPONDER_CERT_9_CA=EID-SK 2007
DIGIDOC_OCSP_RESPONDER_CERT_9_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_10=ESTEID-SK 2007 OCSP 2010.crt
DIGIDOC_OCSP_RESPONDER_CERT_10_CN=ESTEID-SK 2007 OCSP RESPONDER 2010
DIGIDOC_OCSP_RESPONDER_CERT_10_CA=ESTEID-SK 2007
DIGIDOC_OCSP_RESPONDER_CERT_10_URL=http://ocsp.sk.ee

DIGIDOC_OCSP_RESPONDER_CERT_11=KLASS3-SK 2010 OCSP.crt
DIGIDOC_OCSP_RESPONDER_CERT_11_CN=KLASS3-SK 2010 OCSP RESPONDER
DIGIDOC_OCSP_RESPONDER_CERT_11_CA=KLASS3-SK 2010
DIGIDOC_OCSP_RESPONDER_CERT_11_URL=http://ocsp.sk.ee

#DIGIDOC_OCSP_URL=http://www.openxades.org/cgi-bin/ocsp.cgi
DIGIDOC_OCSP_URL=http://ocsp.sk.ee
