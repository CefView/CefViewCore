# 
# The main config file for CefViewCore
#
cmake_minimum_required(VERSION 3.19.1)
project(CefViewCore)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Determine the project architecture.
if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()

  if(OS_MACOSX)
    # PROJECT_ARCH should be specified on Mac OS X.
    message(WARNING "No PROJECT_ARCH value specified, using ${PROJECT_ARCH}")
  endif()
endif()

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# C standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOS 1)
  set(OS_POSIX 1)
  add_definitions(-DOS_MACOS=1 -DOS_POSIX=1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
  add_definitions(-DOS_LINUX=1 -DOS_POSIX=1)
  add_compile_options(-Wno-unknown-pragmas)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
  add_definitions(-DOS_WINDOWS=1)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/$<CONFIG>/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/$<CONFIG>/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/$<CONFIG>/lib)

# Config the CEF
###############################################################
include(CefConfig.cmake)

if (${CMAKE_VERSION} GREATER "3.11")
  cmake_policy(SET CMP0074 NEW)
endif()

if (${CMAKE_VERSION} GREATER "3.12")
  cmake_policy(SET CMP0077 NEW)
endif()

if (OS_WINDOWS)
  add_link_options(/DEBUG)
  set(CEF_RUNTIME_LIBRARY_FLAG "/MD" CACHE STRING "Use dynamic runtime")
endif()

# Disable the sandbox
if ((NOT DEFINED USE_SANDBOX) OR (USE_SANDBOX STREQUAL "") 
    OR (${USE_SANDBOX} MATCHES "(FALSE|false|0|OFF)"))
    option(USE_SANDBOX "Enable CEF Sandbox" OFF)
elseif(${USE_SANDBOX} MATCHES "(TRUE|true|1|null|ON)")
    option(USE_SANDBOX "Enable CEF Sandbox" ON)
else()
    message(FATAL_ERROR "++++++++++ INVALID FLAG USE_SANDBOX=" ${USE_SANDBOX})
endif()

# Set CEF root dir and append it to CMAKE_MODULE_PATH
set(CEF_ROOT "${CEF_SDK_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Find the CEF package
find_package(CEF REQUIRED)

# Add libcef dll wrapper
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

if(USE_SANDBOX AND (OS_WINDOWS OR OS_MACOS))
  add_definitions(-DCEF_USE_SANDBOX)
  message(STATUS "cef_sandbox_lib path:" "${CEF_SANDBOX_LIB_DEBUG}," "${CEF_SANDBOX_LIB_RELEASE}" )
  # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
endif()

PRINT_CEF_CONFIG()
###############################################################

set(CMAKE_SUPPRESS_REGENERATION TRUE)

###############################################################
set(CefViewCore_INCLUDE_PATH
  "${CEF_INCLUDE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Shared"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_subdirectory(src)

get_directory_property(IS_CURRENT_IN_SUBDIRECTORY PARENT_DIRECTORY)
if(IS_CURRENT_IN_SUBDIRECTORY)
  set(CefViewCore_EXPORT_INCLUDE_PATH ${CefViewCore_INCLUDE_PATH} PARENT_SCOPE)
endif()
