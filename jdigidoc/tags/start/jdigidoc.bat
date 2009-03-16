REM jdigidoc utility
set JDIGIDOC_HOME=C:\veiko\JDigiDoc
set CP=%JDIGIDOC_HOME%\lib\bcprov-jdk14-125.jar;%JDIGIDOC_HOME%\lib\iaikPkcs11Wrapper.jar;%JDIGIDOC_HOME%\lib\jakarta-log4j-1.2.6.jar;%JDIGIDOC_HOME%\lib\jce-1_2_2.zip;%JDIGIDOC_HOME%\lib\xmlsec.jar;%JDIGIDOC_HOME%\lib\xalan.jar;%JDIGIDOC_HOME%\lib\xercesImpl.jar;%JDIGIDOC_HOME%\lib\xml-apis.jar;%JDIGIDOC_HOME%\lib\xmlParserAPIs.jar;.
C:\j2sdk1.4.2\bin\java -Xmx512m -classpath %CP% ee.sk.test.jdigidoc -config %JDIGIDOC_HOME%\jdigidoc-win.cfg %1 %2 %3 %4 %5 %6 %7 %8 %9