#
# The main config file for CefViewCore
#
cmake_minimum_required(VERSION 3.19.1)
project(CefViewCore)

option(USE_SANDBOX "Enable CEF Sandbox (Deprecated since CEF 138)" OFF)

option(STATIC_CRT "Use MultiThreaded linkage for MSVC" OFF)

option(USE_GPU_OPTIMUS "Enable GPU Optimus (Only for Windows and usually not needed)" OFF)

# add camke modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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

  if(USE_GPU_OPTIMUS)
    add_definitions(-DENABLE_GPU_OPTIMUS=1)
  endif()
endif()

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# C standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib)

# set CefView Helper Process Name
if(NOT CEFVIEW_WING_NAME)
  set(DEFAULT_CEFVIEW_WING_NAME "CefViewWing")
  message(STATUS
    "CEFVIEW_WING_NAME is empty, use default version ${DEFAULT_CEFVIEW_WING_NAME}\n"
    "You can change the name by adding -DCEFVIEW_WING_NAME=xxx to commandline for generation")
  set(CEFVIEW_WING_NAME ${DEFAULT_CEFVIEW_WING_NAME} CACHE STRING "CefViewWing Helper Process Name" FORCE)
endif()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/CefViewCoreProtocol.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/CefViewCoreProtocol.h"
  @ONLY
)

# Config the CEF
# ##############################################################
message(STATUS "Detecting CEF_SDK_VERSION: ${CEF_SDK_VERSION}")

if(NOT CEF_SDK_VERSION)
  # set CEF version to be used
  # https://cef-builds.spotifycdn.com/index.html
  # https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md
  # set(DEFAULT_CEF_SDK_VERSION "89.0.18+gb36241d+chromium-89.0.4389.114")
  # set(DEFAULT_CEF_SDK_VERSION "90.6.7+g19ba721+chromium-90.0.4430.212")
  # set(DEFAULT_CEF_SDK_VERSION "91.1.23+g04c8d56+chromium-91.0.4472.164")
  # set(DEFAULT_CEF_SDK_VERSION "92.0.27+g274abcf+chromium-92.0.4515.159")
  # set(DEFAULT_CEF_SDK_VERSION "93.1.14+gf38ce34+chromium-93.0.4577.82")
  # set(DEFAULT_CEF_SDK_VERSION "94.4.11+gc4d96f0+chromium-94.0.4606.81")
  # set(DEFAULT_CEF_SDK_VERSION "95.7.18+g0d6005e+chromium-95.0.4638.69")
  # set(DEFAULT_CEF_SDK_VERSION "96.0.18+gfe551e4+chromium-96.0.4664.110")
  # set(DEFAULT_CEF_SDK_VERSION "97.1.9+ga00bca5+chromium-97.0.4692.99")
  # set(DEFAULT_CEF_SDK_VERSION "98.2.1+g29d6e22+chromium-98.0.4758.109")
  # set(DEFAULT_CEF_SDK_VERSION "99.2.15+g71e9523+chromium-99.0.4844.84")     # BAD:IME poisition incorrect
  # set(DEFAULT_CEF_SDK_VERSION "100.0.24+g0783cf8+chromium-100.0.4896.127")  # BAD:IME poisition incorrect
  # set(DEFAULT_CEF_SDK_VERSION "101.0.18+g367b4a0+chromium-101.0.4951.67")   # BAD:IME poisition incorrect
  # set(DEFAULT_CEF_SDK_VERSION "102.0.10+gf249b2e+chromium-102.0.5005.115")  # BAD:IME poisition incorrect
  # set(DEFAULT_CEF_SDK_VERSION "103.0.12+g8eb56c7+chromium-103.0.5060.134")  # BAD:IME poisition incorrect
  # set(DEFAULT_CEF_SDK_VERSION "104.4.26+g4180781+chromium-104.0.5112.102")
  # set(DEFAULT_CEF_SDK_VERSION "105.3.39+g2ec21f9+chromium-105.0.5195.127")
  # set(DEFAULT_CEF_SDK_VERSION "106.1.1+g5891c70+chromium-106.0.5249.119")
  # set(DEFAULT_CEF_SDK_VERSION "107.1.12+g65b79a6+chromium-107.0.5304.122")
  # set(DEFAULT_CEF_SDK_VERSION "108.4.13+ga98cd4c+chromium-108.0.5359.125")
  # set(DEFAULT_CEF_SDK_VERSION "109.1.18+gf1c41e4+chromium-109.0.5414.120")
  # set(DEFAULT_CEF_SDK_VERSION "110.0.32+g291f1df+chromium-110.0.5481.180")
  # set(DEFAULT_CEF_SDK_VERSION "111.2.7+gebf5d6a+chromium-111.0.5563.148")
  # set(DEFAULT_CEF_SDK_VERSION "112.3.0+gb09c4ca+chromium-112.0.5615.165")
  # set(DEFAULT_CEF_SDK_VERSION "113.3.1+g525fa10+chromium-113.0.5672.128")
  # set(DEFAULT_CEF_SDK_VERSION "114.2.13+g6792e13+chromium-114.0.5735.200")
  # set(DEFAULT_CEF_SDK_VERSION "115.3.15+g21130e0+chromium-115.0.5790.173")
  # set(DEFAULT_CEF_SDK_VERSION "116.0.27+gd8c85ac+chromium-116.0.5845.190")
  # set(DEFAULT_CEF_SDK_VERSION "117.2.5+gda4c36a+chromium-117.0.5938.152")
  # set(DEFAULT_CEF_SDK_VERSION "118.7.1+g99817d2+chromium-118.0.5993.119")
  # set(DEFAULT_CEF_SDK_VERSION "119.4.7+g55e15c8+chromium-119.0.6045.199")
  # set(DEFAULT_CEF_SDK_VERSION "120.1.6+gf08b1fd+chromium-120.0.6099.71")
  # set(DEFAULT_CEF_SDK_VERSION "121.3.15+g4d3b0b4+chromium-121.0.6167.184")
  # set(DEFAULT_CEF_SDK_VERSION "122.1.13+gde5b724+chromium-122.0.6261.130")
  # set(DEFAULT_CEF_SDK_VERSION "123.0.13+gfc703fb+chromium-123.0.6312.124")
  # set(DEFAULT_CEF_SDK_VERSION "124.3.9+g9bd638f+chromium-124.0.6367.207")
  # set(DEFAULT_CEF_SDK_VERSION "125.0.22+gc410c95+chromium-125.0.6422.142")
  # set(DEFAULT_CEF_SDK_VERSION "126.2.18+g3647d39+chromium-126.0.6478.183")
  # set(DEFAULT_CEF_SDK_VERSION "127.3.5+g114ea2a+chromium-127.0.6533.120")
  # set(DEFAULT_CEF_SDK_VERSION "128.4.9+g9840ad9+chromium-128.0.6613.120")   # BAD:debugbreak - debug mode
  # set(DEFAULT_CEF_SDK_VERSION "129.0.12+gf09539f+chromium-129.0.6668.101")
  # set(DEFAULT_CEF_SDK_VERSION "130.1.16+g5a7e5ed+chromium-130.0.6723.117")
  # set(DEFAULT_CEF_SDK_VERSION "131.4.1+g437feba+chromium-131.0.6778.265")
  # set(DEFAULT_CEF_SDK_VERSION "132.3.2+g4997b2f+chromium-132.0.6834.161")
  # set(DEFAULT_CEF_SDK_VERSION "133.4.8+g99a2ab1+chromium-133.0.6943.142")
  # set(DEFAULT_CEF_SDK_VERSION "134.3.11+g7c94248+chromium-134.0.6998.178")
  # set(DEFAULT_CEF_SDK_VERSION "135.0.22+g442c600+chromium-135.0.7049.115")
  # set(DEFAULT_CEF_SDK_VERSION "135.0.22+g442c600+chromium-135.0.7049.115")
  # set(DEFAULT_CEF_SDK_VERSION "136.1.6+g1ac1b14+chromium-136.0.7103.114")
  # set(DEFAULT_CEF_SDK_VERSION "137.0.19+g8a1c4ce+chromium-137.0.7151.121")
  # set(DEFAULT_CEF_SDK_VERSION "138.0.49+g5ae25f5+chromium-138.0.7204.261")
  # set(DEFAULT_CEF_SDK_VERSION "139.0.40+g465474a+chromium-139.0.7258.139")
  # set(DEFAULT_CEF_SDK_VERSION "140.1.20+g49f5d09+chromium-140.0.7339.214")
  # set(DEFAULT_CEF_SDK_VERSION "141.0.11+g7e73ac4+chromium-141.0.7390.123")
  set(DEFAULT_CEF_SDK_VERSION "142.0.15+g6dfdb28+chromium-142.0.7444.176")

  # set(DEFAULT_CEF_SDK_VERSION "128.4.9+g9840ad9+chromium-128.0.6613.120")   # BAD   # debugbreak - debug mode
  message(STATUS
    "CEF_SDK_VERSION is empty, use default version ${DEFAULT_CEF_SDK_VERSION}\n"
    "You can change the version by adding -DCEF_SDK_VERSION=xxx to commandline for generation")
  set(CEF_SDK_VERSION ${DEFAULT_CEF_SDK_VERSION} CACHE STRING "Cef binary SDK version" FORCE)
endif()

# setup CEF
include(SetupCef)

# ##############################################################
set(CMAKE_SUPPRESS_REGENERATION TRUE)

# ##############################################################
set(CefViewCore_INCLUDE_PATH
  "${CEF_INCLUDE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_subdirectory(src)

if(OS_MACOS)
  set(CefViewCore_HELPER_TARGETS "${CEFVIEW_WING_NAME};${CEFVIEW_WING_NAME}_gpu;${CEFVIEW_WING_NAME}_plugin;${CEFVIEW_WING_NAME}_renderer")
else()
  set(CefViewCore_HELPER_TARGETS "${CEFVIEW_WING_NAME}")
endif()

get_directory_property(IS_CURRENT_IN_SUBDIRECTORY PARENT_DIRECTORY)

if(IS_CURRENT_IN_SUBDIRECTORY)
  set(CEFVIEW_WING_NAME ${CEFVIEW_WING_NAME} PARENT_SCOPE)
  set(CefViewCore_EXPORT_INCLUDE_PATH ${CefViewCore_INCLUDE_PATH} PARENT_SCOPE)
  set(CefViewCore_HELPER_APP_TARGETS ${CefViewCore_HELPER_TARGETS} PARENT_SCOPE)
  set(CefViewCore_CEF_BINARY_DIR ${CEF_BINARY_DIR} PARENT_SCOPE)
  set(CefViewCore_CEF_RESOURCE_DIR ${CEF_RESOURCE_DIR} PARENT_SCOPE)
  set(CefViewCore_CEF_INCLUDE_DIR ${CEF_ROOT}/include PARENT_SCOPE)
endif()

install(
  DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib"
  DESTINATION "$<CONFIG>"
)
install(
  DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/bin"
  DESTINATION "$<CONFIG>"
  USE_SOURCE_PERMISSIONS
)
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  DESTINATION "include/CefViewCore"
)
install(
  DIRECTORY "${CEF_SDK_DIR}/include"
  DESTINATION "include/cef"
)
