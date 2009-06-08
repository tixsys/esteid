<?xml version="1.0" encoding="UTF-8"?>
<!--Set location of this file to BDOCLIB_CONF_XML environment variable-->
<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="schema/conf.xsd">
    <!--algorithm to use for digest calculation when creating new Bdoc
        see http://www.ietf.org/rfc/rfc4051.txt-->
    <param name="digest.uri">http://www.w3.org/2000/09/xmldsig#sha1</param>
    <!--pkcs11 driver location-->
    <param name="pkcs11.driver.path">/usr/lib/opensc-pkcs11.so</param>
    <!-- OCSP responder url. Used for validating signing certificates and generating BDoc-TM signatures-->
    <param name="ocsp.url">http://www.openxades.org/cgi-bin/ocsp.cgi</param>
    <!--Trusted CA certs in PEM format-->
    <param name="cert.store.path">@CONF_DIR@/certs</param>
    <!--OCSP responder public certificate for validating OCSP response-->
    <param name="ocsp.certs.file">@CONF_DIR@/certs/sk-test-ocsp-responder-2005.pem</param>

    <param name="manifest.xsd.path">file://@CONF_DIR@/schema/OpenDocument_manifest.xsd</param>
    <param name="xades.xsd.path">file://@CONF_DIR@/schema/XAdES.xsd</param>
    <param name="dsig.xsd.path">file://@CONF_DIR@/schema/xmldsig-core-schema.xsd</param>
</configuration>
