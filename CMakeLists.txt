cmake_minimum_required(VERSION 2.8.9)

project (mbpoll)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include (GitVersion)
GetGitVersion(MBPOLL_VERSION)
set(MBPOLL_VERSION
  ${MBPOLL_VERSION_MAJOR}.${MBPOLL_VERSION_MINOR}.${MBPOLL_VERSION_PATCH})
message("MBPOLL_VERSION=${MBPOLL_VERSION}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version-git.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version-git.h"
    IMMEDIATE @ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-function")
#set(CMAKE_BUILD_TYPE Debug)

set(MBPOLL_PKG_VERSION "${MBPOLL_VERSION}")

set(MBPOLL_SRCS src/mbpoll.c
    3rdparty/modbus/modbus.c
    3rdparty/modbus/modbus-data.c
    3rdparty/modbus/modbus-tcp.c
    3rdparty/modbus/modbus-rtu.c
    3rdparty/sysio/delay.c
    3rdparty/sysio/serial.c)

add_executable(mbpoll ${MBPOLL_SRCS})

target_include_directories(mbpoll PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(mbpoll PUBLIC .)
target_include_directories(mbpoll PUBLIC src)
target_include_directories(mbpoll PUBLIC 3rdparty)
#target_link_libraries(mbpoll ${FTDI_LIBRARIES})
install(TARGETS mbpoll RUNTIME DESTINATION bin)

### Debian Package generation
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION "${MBPOLL_PKG_VERSION}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Franz Flasch")
include(CPack)
