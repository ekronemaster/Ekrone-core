# Install script for directory: C:/Users/Administrator/Desktop/core/external/miniupnpc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Administrator/Desktop/core/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Strawberry/c/bin/objdump.exe")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/miniupnpc" TYPE FILE FILES
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/miniupnpc.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/miniwget.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/upnpcommands.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/igd_desc_parse.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/upnpreplyparse.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/upnperrors.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/upnpdev.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/miniupnpctypes.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/portlistingparse.h"
    "C:/Users/Administrator/Desktop/core/external/miniupnpc/miniupnpc_declspec.h"
    )
endif()

