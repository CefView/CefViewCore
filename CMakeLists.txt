# 
# The main config file for CefViewCore
#
cmake_minimum_required(VERSION 3.19.1)
project(CefViewCore)

option(USE_SANDBOX      "Enable CEF Sandbox"                  OFF)
option(STATIC_CRT       "Use MultiThreaded linkage for MSVC"  OFF)

# Determine the project architecture.
if(NOT DEFINED PROJECT_ARCH)
  if(OS_WINDOWS AND "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "arm64")
    set(PROJECT_ARCH "arm64")
  elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOS 1)
  set(OS_POSIX 1)
  add_definitions(-DOS_MACOS=1 -DOS_POSIX=1)

  # Target architecture.
  if(PROJECT_ARCH STREQUAL "x86_64")
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
  elseif(PROJECT_ARCH STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
  else()
    set(CMAKE_OSX_ARCHITECTURES "i386")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
  add_definitions(-DOS_LINUX=1 -DOS_POSIX=1)
  add_compile_options(-Wno-unknown-pragmas)

  # Target architecture.
  if(PROJECT_ARCH STREQUAL "x86_64")
    # x86 64-bit architecture.
    add_compile_options(-m64 -march=x86-64)
    add_link_options(-m64)
  elseif(PROJECT_ARCH STREQUAL "x86")
    # x86 32-bit architecture.
    add_compile_options(-m32)
    add_link_options(-m32)
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
  add_definitions(-DOS_WINDOWS=1)
endif()

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# C standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib)

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
  if (USE_SANDBOX)
    # cef_sandbox.lib is MT already, must keep the same with it
    set(CEF_RUNTIME_LIBRARY_FLAG "/MT" CACHE STRING "Use static runtime")
    add_compile_options("/MT$<$<CONFIG:Debug>:d>")
  else()
    # either MT or MD is supported
    message(STATUS "===== " ${STATIC_CRT})
    set(CEF_RUNTIME_LIBRARY_FLAG "/M$<IF:$<BOOL:${STATIC_CRT}>,T,D>" CACHE STRING "Use static runtime" FORCE)
    add_compile_options("/M$<IF:$<BOOL:${STATIC_CRT}>,T,D>$<$<CONFIG:Debug>:d>")
  endif()
else ()
  add_compile_options(
    "-g"
    "$<$<CONFIG:DEBUG>:-O0>"
    "$<$<CONFIG:RELEASE>:-O3>"
  )
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
  # message(STATUS "cef_sandbox_lib path:" "${CEF_SANDBOX_LIB_DEBUG}," "${CEF_SANDBOX_LIB_RELEASE}" )
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

if(OS_MACOS)
  # CEF Helper app suffixes, format: "<name suffix>:<target suffix>:<plist suffix>"
  set(CEF_HELPER_APP_SUFFIXES
    "::"
    " (GPU):_gpu:.gpu"
    " (Plugin):_plugin:.plugin"
    " (Renderer):_renderer:.renderer"
  )
endif()

add_subdirectory(src)

if(OS_MACOS)
    set(CefViewCore_HELPER_TARGETS "CefViewWing;CefViewWing_gpu;CefViewWing_plugin;CefViewWing_renderer")
else()
    set(CefViewCore_HELPER_TARGETS "CefViewWing")
endif()

get_directory_property(IS_CURRENT_IN_SUBDIRECTORY PARENT_DIRECTORY)
if(IS_CURRENT_IN_SUBDIRECTORY)
  set(CefViewCore_EXPORT_INCLUDE_PATH ${CefViewCore_INCLUDE_PATH} PARENT_SCOPE)
  set(CefViewCore_HELPER_APP_TARGETS ${CefViewCore_HELPER_TARGETS} PARENT_SCOPE)
endif()
