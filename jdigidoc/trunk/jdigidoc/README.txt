DigiDoc Java library
--------------------
JDigiDoc is a Java library for manipulating Estonian DDOC and BDOC
digital signature container files.

It offers the functionality for creating digitally signed  files in
DIGIDOC-XML 1.4, 1.3, 1.2, 1.1 and BDOC 1.0 formats, adding new signatures,
verifying signatures, timestamps and adding confirmations in OCSP format.

DigiDoc documents are XML files based on the international standards XML-DSIG
and ETSI TS 101 903. DigiDoc documents and the JDigiDoc library implement a
subset of XML-DSIG and ETSI TS 101 903.

Related standards and schemas https://id.eesti.ee/trac/wiki/XMLDSIG

Building
--------
1. Get Maven2 from http://maven.apache.org
2. Run
     mvn install

Generating API documentation
----------------------------
Run mvn javadoc:jar

Generating Maven bundle
-----------------------
Run mvn repository:bundle-create
