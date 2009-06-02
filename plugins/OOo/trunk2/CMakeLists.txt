##################################################
##################################################
cmake_minimum_required(VERSION 2.6)
SET (SDKVERSION "3.0")
SET (BUILDID "300m15(Build:9379)")
SET (COMP_NAME "complextoolbarcontrols")

#SET (OS "WIN")
SET (OS "LINUX")

#set(CMAKE_BUILD_TYPE Debug)
project (${COMP_NAME})

set(MKDIR ${CMAKE_COMMAND} -E make_directory)
set(RM ${CMAKE_COMMAND} -E remove)
set(CP ${CMAKE_COMMAND} -E copy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

#-------------------------------------------------
#----------------Init OS specific-----------------
#-------------fix later for all OS----------------
#..Common..
SET (OUT "${CMAKE_CURRENT_BINARY_DIR}/${OS}.out")
SET (OUT_BIN "${OUT}/bin")
SET (OUT_INC "${OUT}/inc")
SET (OUT_SLO "${OUT}/slo")
SET (OUT_LIB "${OUT}/lib")
SET (OUT_SLB "${OUT}/slb")
SET (OUT_MISC "${OUT}/misc")
SET (OUT_OBJ "${OUT}/obj")
SET (OUT_CLASS "${OUT}/class")

#IF (WIN32)
#	SET (UNOPKG_PLATFORM "Windows")
#	SET (SHAREDLIB_EXT "dll")
#	SET (EXE_EXT ".exe")
#	SET (OO_SDK_HOME "C:/Program Files/OpenOffice.org_3.0_SDK/sdk")
#	SET (OO_SDK_URE_HOME "C:/Program Files/OpenOffice.org 3/URE")
#	SET (OO_SDK_URE_LIB_DIR "${OO_SDK_URE_HOME}/lib")
#	SET (OFFICE_BASE_PROGRAM_PATH "C:/Program Files/OpenOffice.org 3/Basis/program")

#	SET (URE_TYPES "${OO_SDK_URE_HOME}/misc/types.rdb")
#	SET (OFFICE_TYPES "${OFFICE_BASE_PROGRAM_PATH}/offapi.rdb")
#
#	SET (COMP_UNOPKG_MANIFEST "${OUT_MISC}/${COMP_NAME}/META-INF/manifest.xml")

#	SET (SETTINGS "${OO_SDK_HOME}/settings")
#ENDIF (WIN32)

#IF (LINUX)
	SET (UNOPKG_PLATFORM "Linux_x86")
	SET (SHAREDLIB_EXT "so")
	SET (EXE_EXT "")
#	SET (PRJ "/usr/lib/openoffice/basis3.0/sdk")
	SET (OO_SDK_HOME "/usr/lib/openoffice/basis3.0/sdk")
	SET (OO_SDK_URE_HOME "/usr/lib/ure")
	SET (OO_SDK_URE_LIB_DIR "/usr/lib/ure/lib")
	SET (OFFICE_BASE_PROGRAM_PATH "/usr/lib/openoffice/basis3.0/program")
	SET (OOSDK_INCLUDE_DIR "${OO_SDK_HOME}/include")
	
#	SET (OO_SDK_OUT "/home/mark/complextoolbarcontrols")
#	SET (OO_SDK_ZIP_HOME "/usr/bin")
	
	SET (SDKTYPEFLAG "${OUT_MISC}/oosdk_cpp_types.flag")

	SET (URE_TYPES "${OO_SDK_URE_HOME}/share/misc/types.rdb")
	SET (OFFICE_TYPES "${OFFICE_BASE_PROGRAM_PATH}/offapi.rdb")
	SET (COMP_UNOPKG_MANIFEST "${OUT_MISC}/${COMP_NAME}/META-INF/manifest.xml")

	SET (SETTINGS "${OO_SDK_HOME}/settings")
#ENDIF(LINUX)



#-------------------------------------------------
#----------------Load modules Include-------------

find_package( OOoSDK REQUIRED )
find_package( LibDigiDoc REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( OpenSSLCrypto REQUIRED )
find_package( ZLIB REQUIRED )
find_package( LibP11 REQUIRED )
find_package( XercesC REQUIRED )
find_package( XmlSecurityC REQUIRED )
find_package( XSD REQUIRED )



#ADD_CUSTOM_COMMAND(
#	OUTPUT ${OUT_INC}/com/sun/star/lang/XMultiServiceFactory.hpp		
#	COMMAND ${OO_SDK_HOME}/bin/cppumaker${EXE_EXT} -Gc -BUCR -O${OUT_INC} ${URE_TYPES} ${OFFICE_TYPES}
#	)
ADD_CUSTOM_COMMAND(
   OUTPUT ${OUT_INC}/com/sun/star/lang/XMultiServiceFactory.hpp
   COMMAND ${OO_SDK_HOME}/bin/cppumaker -Gc -BUCR -O${OUT_INC} ${URE_TYPES} ${OFFICE_TYPES}
   )

INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
)

set(UNO_INCLUDES 
	${OO_SDK_HOME}/include
	${OO_SDK_HOME}/include/stl
	${OUT_INC}
)

set(BDOC_INCLUDES
	${OPENSSL_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${LIBP11_INCLUDE_DIR}
	${XERCESC_INCLUDE_DIR}
	${XSD_INCLUDE_DIR}
	${PCSCLITE_INCLUDE_DIR}
	${LIBDIGIDOC_INCLUDE_DIR}
)

# FIXME: Find a sane way to construct these!!
FOREACH(iDir ${UNO_INCLUDES})
	set(UNO_INCLUDE_FLAGS "${UNO_INCLUDE_FLAGS} -I${iDir}")
ENDFOREACH(iDir)
FOREACH(iDir ${BDOC_INCLUDES})
	set(BDOC_INCLUDE_FLAGS "${BDOC_INCLUDE_FLAGS} -I${iDir}")
ENDFOREACH(iDir)

#-------------------------------------------------
#----------------Set Output-----------------------
SET(UNOFILES
	${OUT_INC}/com/sun/star/lang/XMultiServiceFactory.hpp    
#	SimpleBdocApp.cpp
	MyProtocolHandler.cxx
	MyJob.cxx
	MyListener.cxx
	WriterListener.cxx
	CalcListener.cxx
	ListenerHelper.cxx
	exports.cxx
	)

SET(BDOCFILES
        MyBdocBridge.cpp
)
   
#TODO:add all definitions from settinks mk
#IF (WIN32)
#	SET (CC_DEFINES "-DWIN32 -DWNT -DCPPU_ENV=msci")
#	SET (CC_FLAGS "-wd4251 -wd4275 -wd4290 -wd4675 -wd4786 -wd4800")
#	add_definitions(${CC_DEFINES} ${CC_FLAGS})
#ENDIF (WIN32)
#IF (LINUX)
        set(UNO_DEFINITIONS "-DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 -DGXX_INCLUDE_PATH=${SDK_GXX_INCLUDE_PATH}")
#ENDIF (LINUX)

set(BDOC_DEFINITIONS "-DBDOCLIB_CONF_PATH=\\\"${LIBDIGIDOC_CONF}\\\"" )

#if(MSVC)
#	STRING(REPLACE "/MD"  "/MT"  CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
#	STRING(REPLACE "/MD"  "/MT"  CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
#	STRING(REPLACE "/MDd"  "/MT"  CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
#	STRING(REPLACE "/MDd"  "/MT"  CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#	STRING(REPLACE "/MD"  "/MT"  CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
#	STRING(REPLACE "/MD"  "/MT"  CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
#endif(MSVC)

set(UNO_CPP_FLAGS "${UNO_DEFINITIONS} ${UNO_INCLUDE_FLAGS}")
set(BDOC_CPP_FLAGS "${BDOC_DEFINITIONS} ${BDOC_INCLUDE_FLAGS}")

set_source_files_properties(${UNOFILES}
  PROPERTIES
  COMPILE_FLAGS "${UNO_CPP_FLAGS}")

set_source_files_properties(${BDOCFILES}
  PROPERTIES
  COMPILE_FLAGS "${BDOC_CPP_FLAGS}")

add_library(${COMP_NAME}.uno MODULE ${UNOFILES} ${BDOCFILES})

#-------------------------------------------------
#---------------Link to the Libraries-------------
#----------------This is win specific-------------
target_link_libraries(${COMP_NAME}.uno 
	${LIBDIGIDOC_LIBRARY}
	${OPENSSL_LIBRARIES}
	${OPENSSLCRYPTO_LIBRARIES}
	${ZLIB_LIBRARIES}
	${LIBP11_LIBRARIES}
	${XERCESC_LIBRARIES}
	${XMLSECURITYC_LIBRARIES}
	
	${CPPU_LIBRARY} 
	${CPPUHELPER_LIBRARY} 
	${REG_LIBRARY} 
	${RMCXT_LIBRARY} 
	${SAL_LIBRARY} 
	${SALHELPER_LIBRARY} 
	${STORE_LIBRARY} 
	${STLPORT_LIBRARY}
	)
SET(LINK_FLAG_SCRIPT "-Wl,-z,defs -Wl,--version-script,${SETTINGS}/component.uno.map -L${OUT_LIB} -L${OO_SDK_URE_LIB_DIR}")
#SET(LINK_FLAG_SCRIPT "-Wl,--version-script,${SETTINGS}/component.uno.map -L${OUT_LIB} -L${OO_SDK_URE_LIB_DIR}")

set_target_properties(${COMP_NAME}.uno PROPERTIES
				PREFIX ""
#win specific#			LINK_FLAGS "/DEF:${OO_SDK_HOME}/settings/component.uno.def"
				LINK_FLAGS "${LINK_FLAG_SCRIPT}"
				SKIP_BUILD_RPATH true
				LIBRARY_OUTPUT_DIRECTORY "${OUT_LIB}")


##setup and zip oxt package
CONFIGURE_FILE (manifest.xml.in ${COMP_UNOPKG_MANIFEST})
#CONFIGURE_FILE(${OUT_LIB}/${COMP_NAME}.uno.${SHAREDLIB_EXT} ${OUT_MISC}/${COMP_NAME}/${UNOPKG_PLATFORM}/${COMP_NAME}.uno.${SHAREDLIB_EXT COPYONLY})
CONFIGURE_FILE(Addons.xcu ${OUT_MISC}/${COMP_NAME}/Addons.xcu COPYONLY)
CONFIGURE_FILE(ProtocolHandler.xcu ${OUT_MISC}/${COMP_NAME}/ProtocolHandler.xcu COPYONLY)
CONFIGURE_FILE(WriterWindowState.xcu ${OUT_MISC}/${COMP_NAME}/WriterWindowState.xcu COPYONLY)
CONFIGURE_FILE(CalcWindowState.xcu ${OUT_MISC}/${COMP_NAME}/CalcWindowState.xcu COPYONLY)
CONFIGURE_FILE(logo_small.png ${OUT_MISC}/${COMP_NAME}/logo_small.png COPYONLY)
CONFIGURE_FILE(logo_big.png ${OUT_MISC}/${COMP_NAME}/logo_big.png COPYONLY)

add_custom_target( zip-oxt ALL
	COMMAND ${CP} ${OUT_LIB}/${COMP_NAME}.uno.${SHAREDLIB_EXT} 
		${OUT_MISC}/${COMP_NAME}/${UNOPKG_PLATFORM}/${COMP_NAME}.uno.${SHAREDLIB_EXT} 

#	COMMAND ${CP} ${OUT_LIB}/Debug/${COMP_NAME}.uno.${SHAREDLIB_EXT} #VC++ lükkab debugi või releasi folderi vahele
#		${OUT_MISC}/${COMP_NAME}/${UNOPKG_PLATFORM}/${COMP_NAME}.uno.${SHAREDLIB_EXT} 

	WORKING_DIRECTORY ${OUT_MISC}/${COMP_NAME}
	COMMAND ${MKDIR} ${OUT_BIN}
	COMMAND zip -r ${OUT_BIN}/${COMP_NAME}.oxt . 	
	)
