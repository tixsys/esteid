<?xml version="1.0" encoding="UTF-8"?>
<!--Set location of this file to BDOCLIB_CONF_XML environment variable-->
<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="schema/conf.xsd">
    <!--algorithm to use for digest calculation when creating new Bdoc
        see http://www.ietf.org/rfc/rfc4051.txt-->
    <param name="digest.uri">http://www.w3.org/2000/09/xmldsig#sha1</param>
    <!--pkcs11 driver location-->
    <param name="pkcs11.driver.path">@PKCS11_MODULE@</param>
    <!--Trusted CA certs in PEM format-->
    <param name="cert.store.path">certs</param>
    <param name="manifest.xsd.path">schema/OpenDocument_manifest.xsd</param>
    <param name="xades.xsd.path">schema/XAdES.xsd</param>
    <param name="dsig.xsd.path">schema/xmldsig-core-schema.xsd</param>
    <!--<param name="proxy.host"></param>-->
    <!--<param name="proxy.port"></param>-->
    <!--<param name="pkcs12.cert"></param>-->
    <!--<param name="pkcs12.pass"></param>-->

    <!-- OCSP responder url. Used for validating signing certificates and generating BDoc-TM signatures-->
    <ocsp issuer="ESTEID-SK">
        <url>http://91.198.206.66</url>
        <cert>certs/ESTEID-SK OCSP 2005.crt</cert>
    </ocsp>
    <ocsp issuer="ESTEID-SK 2007">
        <url>http://91.198.206.66</url>
        <cert>certs/ESTEID-SK 2007 OCSP.crt</cert>
    </ocsp>
    <ocsp issuer="EID-SK">
        <url>http://91.198.206.66</url>
        <cert>certs/EID-SK OCSP 2006.crt</cert>
    </ocsp>
    <ocsp issuer="EID-SK 2007">
        <url>http://91.198.206.66</url>
        <cert>certs/EID-SK 2007 OCSP.crt</cert>
    </ocsp>
    <ocsp issuer="KLASS3-SK">
        <url>http://91.198.206.66</url>
        <cert>certs/KLASS3-SK OCSP 2009.crt</cert>
    </ocsp>
    <ocsp issuer="TEST-SK">
        <url>http://www.openxades.org/cgi-bin/ocsp.cgi</url>
        <cert>certs/TEST-SK OCSP 2005.crt</cert>
    </ocsp>
</configuration>
