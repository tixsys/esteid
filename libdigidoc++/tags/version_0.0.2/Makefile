# Configuration.
BUILD_FLAGS = -O3 # Set to "-g" for debug and "-O3" for release.
LINK_FLAGS =      # Set to "-L/usr/lib/debug/usr/lib/" to link with debug libraries. 
ROOT = .
MINIZIP_ROOT = $(ROOT)/lib/zlib-1.2.3/contrib/minizip

# Directories.
ETC = $(ROOT)/etc
SRC = $(ROOT)/src
BUILD = $(ROOT)/build
OBJ = $(BUILD)/obj
DOXY = $(BUILD)/doxy
DIST = $(BUILD)/dist

# Tools.
CC = g++
LD = g++
CP = cp
RM = rm -rf
MKDIR = mkdir -p
TAR = tar
DOXYGEN = doxygen

# Flags.
CCFLAGS = $(BUILD_FLAGS) -Wall -fPIC
LDFLAGS = $(LINK_FLAGS) -L$(MINIZIP_ROOT)

#Includes.
INCLUDES= -I$(MINIZIP_ROOT)

# Libs.
SYSLIBS = -lstdc++ -lxerces-c -lcrypto -lssl -lp11
EXTERNALLIBS = -lminizip
DEMOLIBS = -ldigidoc

# Code and object files.
DIGIDOC_LIB_SRCS = BDoc.cpp \
	BDocException.cpp\
	Conf.cpp\
	Document.cpp\
	Exception.cpp\
	Signature.cpp\
	SignatureBES.cpp\
	SignatureException.cpp\
	crypto/SHA1Digest.cpp\
	crypto/ocsp/OCSP.cpp\
	crypto/ocsp/OCSPException.cpp\
	crypto/signer/DummySigner.cpp\
	crypto/signer/PKCS11Signer.cpp\
	crypto/signer/Signer.cpp\
	crypto/signer/SignException.cpp\
	io/IOException.cpp io/ISerialize.cpp\
	io/ZipSerialize.cpp util/String.cpp\
	util/File.cpp\
	util/DateTime.cpp\
	xml/OpenDocument_manifest.cxx\
	xml/xmldsig-core-schema.cxx\
	xml/XAdES.cxx
DIGIDOC_LIB_OBJS := $(DIGIDOC_LIB_SRCS:%.cpp=$(OBJ)/%.o)
DIGIDOC_LIB_OBJS := $(DIGIDOC_LIB_OBJS:%.cxx=$(OBJ)/%.o)



info: 
	@echo -e "Pleas specify a target to build."
	@echo -e "Available targets: \033[32minfo\033[0m, \033[32mdemo\033[0m, \033[31mlibdigidoc.so\033[0m, \033[32mdigidoc_pkg\033[0m, \033[32mdoxygen\033[0m, \033[32mclean\033[0m and \033[32mclean_all\033[0m."
	@echo -e "\033[33minfo\033[0m - Prints this info."
	@echo -e "\033[33mdemo\033[0m - Builds an example program, that uses the digidoc library."
	@echo -e "\033[33mlibdigidoc.so\033[0m - Builds the BDOC signing library."
	@echo -e "\033[33mdigidoc_pkg\033[0m - Creates archive digidoc.tar.gz, which contains demo application and all needed libraries."
	@echo -e "\033[33mdoxygen\033[0m - Creates documentation with doxygen and archives it to digidoc_doxy.tar.gz."
	@echo -e "\033[33mclean\033[0m - Deletes object files."
	@echo -e "\033[33mclean_all\033[0m - Deletes object files, builded library and executables."

init: 
	if [ ! -f $(MINIZIP_ROOT)/libminizip.a ]; then \
	    echo -e "\033[31mError\033[0m: Library libminizip.a does not exist in $(MINIZIP_ROOT) folder. Check that the parameter MINIZIP_ROOT points to minizip project root folder and the library (libminizip.a) is built."; \
	    false; \
	fi
	$(MKDIR) $(OBJ)/crypto/ocsp $(OBJ)/crypto/signer $(OBJ)/io $(OBJ)/util $(OBJ)/xml
	$(MKDIR) $(DIST)

demo: init main_demo.cpp libdigidoc.so
	$(LD) $(CCFLAGS) $(LDFLAGS) -L$(DIST) $(OBJ)/main_demo.o $(DEMOLIBS) -o $(DIST)/$@

libdigidoc.so: init $(DIGIDOC_LIB_SRCS)
	$(LD) -shared $(CCFLAGS) $(LDFLAGS) $(DIGIDOC_LIB_OBJS) $(SYSLIBS) $(EXTERNALLIBS) -o $(DIST)/$@

%.cpp:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $(SRC)/$*.cpp -o $(OBJ)/$*.o

%.cxx:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $(SRC)/$*.cxx -o $(OBJ)/$*.o

digidoc_pkg: demo
	@echo -e "TODO: implement"
#	$(MKDIR) $(DIST)/login_pkg
#	$(CP) $(BMCRYPTOLIB_ROOT)/dist/libbmcrypto.so $(DIST)/login_pkg
#	$(CP) $(SSLLIB_ROOT)/dist/libssl.so $(DIST)/login_pkg
#	$(CP) $(DIST)/libhttp.so $(DIST)/login_pkg
#	$(CP) $(DIST)/demo $(DIST)/login_pkg
#	$(CP) $(ROOT)/bin/login_pkg/run_login.sh $(DIST)/login_pkg
#	$(TAR) -czf $(DIST)/login_pkg.tar.gz -C $(DIST) login_pkg
#	$(RM) $(DIST)/login_pkg

doxygen:
	$(MKDIR) $(DOXY)
	$(MKDIR) $(DIST)
	$(DOXYGEN) $(ETC)/Doxyfile
	$(TAR) -czf $(DIST)/digidoc_doxy.tar.gz -C $(ROOT) doxy

clean:
	$(RM) $(OBJ)
	$(RM) $(DOXY)

clean_all:
	$(RM) $(BUILD)

# DO NOT DELETE THIS LINE -- make depend depends on it.
