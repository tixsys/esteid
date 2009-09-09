#!/bin/bash -x

g++ -MMD -O2 -I.. -g -Ipkcs11 -I/usr/include/PCSC -c esteidpkcs11.cpp  -fPIC -o obj/Release/esteidpkcs11.o

g++ -MMD -O2 -I.. -g -Ipkcs11 -I/usr/include/PCSC -c PKCS11Context.cpp -fPIC -o obj/Release/PKCS11Context.o

g++ -MMD -O2 -I.. -g -Ipkcs11 -I/usr/include/PCSC -c precompiled.cpp  -fPIC -o obj/Release/precompiled.o

g++ -o libesteidpkcs11.so obj/Release/PKCS11Context.o obj/Release/precompiled.o obj/Release/esteidpkcs11.o  -shared -s -L../cardlib -L../utility -lcardlib -lutility 

pkcs11-tool --module ./libesteidpkcs11.so -I
