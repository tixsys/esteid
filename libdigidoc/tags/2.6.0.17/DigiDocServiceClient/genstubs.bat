REM generate soap stubs
del ddsC.c ddsClient.c ddsClientLib.c ddsH.h ddsDtub.h *.nsmap *.wsdl *.xsd
c:\Toolkits\gsoap-win32-2.7\bin\soapcpp2.exe -pdds -C -1 -x -c DigiDocService.h
del ddsClientLib.c