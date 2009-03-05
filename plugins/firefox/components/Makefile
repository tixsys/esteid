GECKO_SDK := /usr/lib64/xulrunner-sdk-1.9
NSPR_INCLUDES := $(shell pkg-config nspr --cflags)
NSPR_LIBS := $(shell pkg-config nspr --libs)
CARDLIB := ../../../esteidutil/trunk/src/cardlib

FF_LDFLAGS := -L${GECKO_SDK}/lib
IDL_FLAGS := -v -w -I${GECKO_SDK}/idl

CPPFLAGS += -fPIC -Wall ${NSPR_INCLUDES} -I${GECKO_SDK}/include
CPPFLAGS += -include ${GECKO_SDK}/xpcom-config.h
CPPFLAGS += -I/usr/include/PCSC -I${CARDLIB}
CXXFLAGS += \
        -fno-rtti \
        -fshort-wchar \
        -fmessage-length=0
#        -fno-exceptions

COMPONENT_NAME = esteid

all: ${COMPONENT_NAME}.so ${COMPONENT_NAME}.xpt #${COMPONENT_NAME}.js

%.xpt: %.idl
	xpidl $(IDL_FLAGS) -m typelib -e $@ $<

%.h: %.idl
	xpidl $(IDL_FLAGS) -m header -e $@ $<

${COMPONENT_NAME}.so: nsEstEID.o nsEstEIDService.o EstEIDServiceBase.o \
			EstEIDModule.o
	${CXX} -Wl,--discard-all -Wl,-Bsymbolic -Wl,--version-script=${COMPONENT_NAME}.version_script -Wl,-z,defs -shared $(FF_LDFLAGS) -L./ -o $@ $^ -lcardlib -lxpcomglue_s -lxpcom ${NSPR_LIBS}

${COMPONENT_NAME}.xpt: nsIEstEID.xpt nsIEstEID.xpt
	xpt_link $@ $+

${COMPONENT_NAME}.js: nsEstEID.js
	cat $+ > $@

nsEstEID.o:	nsIEstEID.h

clean:
	rm -f *.xpt *.o *.so nsIEstEID.h ${COMPONENT_NAME}.js
