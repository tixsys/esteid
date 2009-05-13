# Makefile for DigiDocService client lib project

#GSOAP_HOME=/opt/gsoap-linux-2.7
GSOAP_HOME=/usr/local
#SOAPCPP2=$(GSOAP_HOME)/soapcpp2
SOAPCPP2=$(GSOAP_HOME)/bin/soapcpp2

CFLAGS=-DWITH_SOAPDEFS_H -D_POSIX_THREADS -I$(GSOAP_HOME) -I/usr/include/libxml2 -I/usr/local/include -I/usr/local/include/libdigidoc -I/usr/local/ssl/include -I. -I..

LDFLAGS=-L/usr/local/lib -L/usr/local/ssl/lib -L$(GSOAP_HOME)/lib -L../libdigidoc/.libs -lxml2 -ldigidoc -lssl -lcrypto -lpthread -lgsoapssl
GENFLAGS=-pdds -1 -C -c -x

CLIENTOBJS=ddsdoc.o

CLIENT_LIB_OBJS= ddsC.o \
	ddsClient.o DigiDocDds.o

GENSRC=ddsStub.h ddsH.h ddsC.c ddsClient.c ddsClientLib.c


GENFILES=$(GENSRC) *.xsd *.xml *.nsmap

CLIENT=ddsdoc
DDS_CLIENTLIB=libddsclient.a

all: $(DDS_CLIENTLIB) $(CLIENT)

$(DDS_CLIENTLIB): $(GENSRC) $(CLIENT_LIB_OBJS)
	$(AR) $(ARFLAGS) $(DDS_CLIENTLIB) $(CLIENT_LIB_OBJS)
	ranlib $(DDS_CLIENTLIB)

$(GENSRC): DigiDocService.h
	$(SOAPCPP2) $(GENFLAGS) DigiDocService.h


$(CLIENTOBJS): $(GENSRC)

%o : %c
	gcc -g -c $(CFLAGS) $< -o $@

%o : %cpp
	g++ -g -c $(CFLAGS) $< -o $@


$(CLIENT): $(CLIENTOBJS) $(DDS_CLIENTLIB) 
	g++ -o $(CLIENT) $(CLIENTOBJS) $(DDS_CLIENTLIB) $(LDFLAGS)  

clean:
	rm -rf *.o *~ $(GENFILES) *.xml $(CLIENT) *.log ddocs/* *.wsdl *.a

install:
	cp $(SERVICE) /var/www/cgi-bin
