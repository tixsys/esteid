cmake_minimum_required( VERSION 2.6 )
project( EstEIDReader )

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

if( UNIX )
  find_package( LibDL REQUIRED )
endif( UNIX )

find_package( OpenSC REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( OpenSSLCrypto REQUIRED )

set( HEADERS
	BaseDigest.h
	EstEID.h
	EstEIDIncludes.h
	EstEIDReader.h
	EstEIDSession.h
	Sha1Digest.h
	Sha256Digest.h
	)
set( SOURCES
	${HEADERS}
	EstEID.cpp
	EstEIDSession.cpp
	EstIDReader.cpp
	Sha1Digest.cpp
	Sha256Digest.cpp
	)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${OPENSSLCRYPTO_INCLUDE_DIR}
        ${OPENSC_INCLUDE_DIR}
	)

add_library(
	EstEIDReader
	SHARED
	${SOURCES}
	)

if( WIN32 AND MSVC )
  set_target_properties( EstEIDReader PROPERTIES DEFINE_SYMBOL "ESTEID_READER_EXPORTS" )
endif( WIN32 AND MSVC )

target_link_libraries(
	EstEIDReader
	${OPENSSLCRYPTO_LIBRARIES}
        ${OPENSC_LIBRARY} 
        ${LIBDL_LIBRARIES}
	)

install( TARGETS EstEIDReader DESTINATION lib )
