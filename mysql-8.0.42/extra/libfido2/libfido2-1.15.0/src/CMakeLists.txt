# Copyright (c) 2018-2022 Yubico AB. All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
# SPDX-License-Identifier: BSD-2-Clause

add_definitions(-D_FIDO_INTERNAL)

list(APPEND FIDO_SOURCES
	aes256.c
	assert.c
	authkey.c
	bio.c
	blob.c
	buf.c
	cbor.c
	compress.c
	config.c
	cred.c
	credman.c
	dev.c
	ecdh.c
	eddsa.c
	err.c
	es256.c
	es384.c
	hid.c
	info.c
	io.c
	iso7816.c
	largeblob.c
	log.c
	pin.c
	random.c
	reset.c
	rs1.c
	rs256.c
	time.c
	touch.c
	tpm.c
	types.c
	u2f.c
	util.c
)

if(FUZZ)
	list(APPEND FIDO_SOURCES ../fuzz/clock.c)
	list(APPEND FIDO_SOURCES ../fuzz/pcsc.c)
	list(APPEND FIDO_SOURCES ../fuzz/prng.c)
	list(APPEND FIDO_SOURCES ../fuzz/udev.c)
	list(APPEND FIDO_SOURCES ../fuzz/uniform_random.c)
	list(APPEND FIDO_SOURCES ../fuzz/wrap.c)
endif()

if(NFC_LINUX)
	list(APPEND FIDO_SOURCES netlink.c nfc.c nfc_linux.c)
endif()

if(USE_PCSC)
	list(APPEND FIDO_SOURCES nfc.c pcsc.c)
endif()

if(USE_HIDAPI)
	list(APPEND FIDO_SOURCES hid_hidapi.c)
	if(NOT WIN32 AND NOT APPLE)
		list(APPEND FIDO_SOURCES hid_unix.c)
	endif()
elseif(WIN32)
	list(APPEND FIDO_SOURCES hid_win.c)
	if(USE_WINHELLO)
		list(APPEND FIDO_SOURCES winhello.c)
	endif()
elseif(APPLE)
	list(APPEND FIDO_SOURCES hid_osx.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	list(APPEND FIDO_SOURCES hid_linux.c hid_unix.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
	list(APPEND FIDO_SOURCES hid_netbsd.c hid_unix.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
	list(APPEND FIDO_SOURCES hid_openbsd.c hid_unix.c)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
    CMAKE_SYSTEM_NAME STREQUAL "MidnightBSD")
	list(APPEND FIDO_SOURCES hid_freebsd.c hid_unix.c)
else()
	message(FATAL_ERROR "please define a hid backend for your platform")
endif()

if(NOT MSVC)
	set_source_files_properties(${FIDO_SOURCES}
	    PROPERTIES COMPILE_FLAGS "${EXTRA_CFLAGS}")
endif()

list(APPEND COMPAT_SOURCES
	../openbsd-compat/bsd-asprintf.c
	../openbsd-compat/bsd-getpagesize.c
	../openbsd-compat/clock_gettime.c
	../openbsd-compat/endian_win32.c
	../openbsd-compat/explicit_bzero.c
	../openbsd-compat/explicit_bzero_win32.c
	../openbsd-compat/freezero.c
	../openbsd-compat/recallocarray.c
	../openbsd-compat/strlcat.c
	../openbsd-compat/timingsafe_bcmp.c
)

# Windows wants only major.minor
IF(WIN32)
  SET(FIDO_LIBRARY_VERSION ${FIDO_MAJOR}.${FIDO_MINOR})
ELSE()
  SET(FIDO_LIBRARY_VERSION ${FIDO_MAJOR}.${FIDO_MINOR}.${FIDO_PATCH})
ENDIF()

IF(WIN32)
  IF(USE_WINHELLO)
    list(APPEND BASE_LIBRARIES webauthn)
  ENDIF()
ENDIF()

list(APPEND BASE_LIBRARIES ${LIBRT})

# Add our own shared library:

DOWNGRADE_OPENSSL3_DEPRECATION_WARNINGS(
  cbor.c
  ecdh.c
  es256.c
  es384.c
  rs256.c
  )

# Silencing deprecated OpenSSL API usage in libfido2
MY_CHECK_CXX_COMPILER_WARNING("-Wdeprecated-declarations" HAS_WARN_FLAG)
IF (HAS_WARN_FLAG)
  STRING_APPEND(CMAKE_C_FLAGS " ${HAS_WARN_FLAG}")
ENDIF()

ADD_SHARED_LIBRARY(fido2
  ${FIDO_SOURCES} ${COMPAT_SOURCES}
  SKIP_INSTALL
  VERSION ${FIDO_LIBRARY_VERSION} SOVERSION ${FIDO_MAJOR}
  LINK_LIBRARIES
    ext::cbor
    ${BASE_LIBRARIES}
    ${CRYPTO_LIBRARY}
    ${HIDAPI_LIBRARIES}
    ${UDEV_LIBRARIES}
    ext::zlib
  NO_UNDEFINED
  )
IF(WIN32)
  TARGET_LINK_LIBRARIES(fido2 wsock32 wsock32 ws2_32 bcrypt setupapi hid)
ELSEIF(APPLE)
  TARGET_LINK_LIBRARIES(fido2 "-framework CoreFoundation" "-framework IOKit")
ENDIF()

IF(WITH_AUTHENTICATION_FIDO OR WITH_AUTHENTICATION_CLIENT_PLUGINS)
  INSTALL_PRIVATE_LIBRARY(fido2)
ENDIF()

# On Linux libcrypto.so.1.1 will be installed in the same
# directory as libfido2.so.1
IF(CRYPTO_CUSTOM_LIBRARY)
  ADD_INSTALL_RPATH(fido2 "\$ORIGIN")
ENDIF()
SET_PATH_TO_CUSTOM_SSL_FOR_APPLE(fido2)
RETURN()
################################################################

if(WIN32)
	list(APPEND BASE_LIBRARIES wsock32 ws2_32 bcrypt setupapi hid)
	if(USE_PCSC)
		list(APPEND BASE_LIBRARIES winscard)
	endif()
elseif(APPLE)
	list(APPEND BASE_LIBRARIES "-framework CoreFoundation"
	    "-framework IOKit")
	if(USE_PCSC)
		list(APPEND BASE_LIBRARIES "-framework PCSC")
	endif()
endif()

list(APPEND TARGET_LIBRARIES
	${CBOR_LIBRARIES}
	${CRYPTO_LIBRARIES}
	${UDEV_LIBRARIES}
	${BASE_LIBRARIES}
	${HIDAPI_LIBRARIES}
	${ZLIB_LIBRARIES}
	${PCSC_LIBRARIES}
)

# static library
if(BUILD_STATIC_LIBS)
	add_library(fido2 STATIC ${FIDO_SOURCES} ${COMPAT_SOURCES})
	if(WIN32 AND NOT MINGW)
		set_target_properties(fido2 PROPERTIES OUTPUT_NAME fido2_static)
	endif()
	target_link_libraries(fido2 ${TARGET_LIBRARIES})
	install(TARGETS fido2 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

# dynamic library
if(BUILD_SHARED_LIBS)
	add_library(fido2_shared SHARED ${FIDO_SOURCES} ${COMPAT_SOURCES})
	set_target_properties(fido2_shared PROPERTIES OUTPUT_NAME fido2
		VERSION ${FIDO_VERSION} SOVERSION ${FIDO_MAJOR})
	target_link_libraries(fido2_shared ${TARGET_LIBRARIES})
	install(TARGETS fido2_shared
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

IF(DISABLE_THESE_LINES)
install(FILES fido.h DESTINATION include)
install(DIRECTORY fido DESTINATION include)

configure_file(libfido2.pc.in libfido2.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libfido2.pc"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
ENDIF(DISABLE_THESE_LINES)
