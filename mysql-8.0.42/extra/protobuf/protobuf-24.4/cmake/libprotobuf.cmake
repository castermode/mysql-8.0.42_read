# CMake definitions for libprotobuf (the "full" C++ protobuf runtime).

include(${protobuf_SOURCE_DIR}/src/file_lists.cmake)
include(${protobuf_SOURCE_DIR}/cmake/protobuf-configure-target.cmake)

add_library(libprotobuf ${protobuf_SHARED_OR_STATIC}
  ${libprotobuf_srcs}
  ${libprotobuf_hdrs}
  ${protobuf_version_rc_file})

ADD_OBJDUMP_TARGET(show_libprotobuf "$<TARGET_FILE:libprotobuf>"
  DEPENDS libprotobuf)
ADD_LIBRARY(ext::libprotobuf ALIAS libprotobuf)

# Disable this, we have our own version script
if(FALSE AND protobuf_HAVE_LD_VERSION_SCRIPT)
  if(${CMAKE_VERSION} VERSION_GREATER 3.13 OR ${CMAKE_VERSION} VERSION_EQUAL 3.13)
    target_link_options(libprotobuf PRIVATE -Wl,--version-script=${protobuf_SOURCE_DIR}/src/libprotobuf.map)
  elseif(protobuf_BUILD_SHARED_LIBS)
    target_link_libraries(libprotobuf PRIVATE -Wl,--version-script=${protobuf_SOURCE_DIR}/src/libprotobuf.map)
  endif()
  set_target_properties(libprotobuf PROPERTIES
    LINK_DEPENDS ${protobuf_SOURCE_DIR}/src/libprotobuf.map)
endif()
target_link_libraries(libprotobuf PRIVATE ${CMAKE_THREAD_LIBS_INIT})
if(protobuf_WITH_ZLIB)
  target_link_libraries(libprotobuf PRIVATE ${ZLIB_LIBRARIES})
endif()
if(protobuf_LINK_LIBATOMIC)
  target_link_libraries(libprotobuf PRIVATE atomic)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  target_link_libraries(libprotobuf PRIVATE log)
endif()
target_include_directories(libprotobuf PUBLIC ${protobuf_SOURCE_DIR}/src)
target_link_libraries(libprotobuf PUBLIC ${protobuf_ABSL_USED_TARGETS})
protobuf_configure_target(libprotobuf)
if(protobuf_BUILD_SHARED_LIBS)
  target_compile_definitions(libprotobuf
    PUBLIC  PROTOBUF_USE_DLLS
    PRIVATE LIBPROTOBUF_EXPORTS)
endif()
set_target_properties(libprotobuf PROPERTIES
    VERSION ${protobuf_VERSION}
    OUTPUT_NAME ${LIB_PREFIX}protobuf
    DEBUG_POSTFIX "${protobuf_DEBUG_POSTFIX}"
    # For -fvisibility=hidden and -fvisibility-inlines-hidden
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
add_library(protobuf::libprotobuf ALIAS libprotobuf)

target_link_libraries(libprotobuf PRIVATE utf8_validity)

################################################################

IF(protobuf_BUILD_SHARED_LIBS)
  SET_TARGET_PROPERTIES(libprotobuf PROPERTIES
    DEBUG_POSTFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/library_output_directory
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/library_output_directory
    )

  IF(LINUX)
    SET_TARGET_PROPERTIES(libprotobuf
      PROPERTIES LINK_FLAGS
      "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/libprotobuf.ver"
      )
    SET_TARGET_PROPERTIES(libprotobuf
      PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libprotobuf.ver
      )
  ENDIF()
  IF(APPLE)
    TARGET_LINK_OPTIONS(libprotobuf PRIVATE LINKER:-no_warn_duplicate_libraries)
  ENDIF()

  IF(WIN32)
    ADD_CUSTOM_COMMAND(TARGET libprotobuf POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_BINARY_DIR}/library_output_directory/${CMAKE_CFG_INTDIR}/$<TARGET_FILE_NAME:libprotobuf>"
      "${CMAKE_BINARY_DIR}/runtime_output_directory/${CMAKE_CFG_INTDIR}/$<TARGET_FILE_NAME:libprotobuf>"
      )

    SET_TARGET_PROPERTIES(libprotobuf PROPERTIES
      DEBUG_POSTFIX "-debug")
    INSTALL_DEBUG_TARGET(libprotobuf
      DESTINATION "${INSTALL_BINDIR}"
      COMPONENT SharedLibraries
      )
  ENDIF()

  INSTALL_PRIVATE_LIBRARY(libprotobuf)

ENDIF()
