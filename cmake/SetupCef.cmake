# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Generally, there is NO NEED to modify the following config
#
# Download CEF binary package
# https://cef-builds.spotifycdn.com/index.html
#

# #################################################################################
# Stage 1. Download CEF binary package
if(NOT CEF_SDK_VERSION)
  message(FATAL_ERROR "CEF_SDK_VERSION is missing!")
endif()

if(OS_WINDOWS)
  set(CEF_SDK_PLATFORM "windows")
elseif(OS_LINUX)
  set(CEF_SDK_PLATFORM "linux")
elseif(OS_MACOS)
  set(CEF_SDK_PLATFORM "macos")
else()
  message(FATAL_ERROR "Unsupported Operating System")
endif()

if(PROJECT_ARCH STREQUAL "x86_64")
  set(CEF_SDK_ARCH "64")
elseif(PROJECT_ARCH STREQUAL "x86")
  set(CEF_SDK_ARCH "32")
elseif(PROJECT_ARCH STREQUAL "arm64")
  set(CEF_SDK_ARCH "arm64")
elseif(PROJECT_ARCH STREQUAL "arm")
  set(CEF_SDK_ARCH "arm")
else()
  message(FATAL_ERROR "Unsupported Processor Architecture")
endif()

# set cef sdk package name
set(CEF_SDK_WORKSPACE "${CMAKE_CURRENT_SOURCE_DIR}/dep")

if(OS_MACOS AND PROJECT_ARCH STREQUAL "x86_64")
  # macosx64
  set(CEF_SDK_PACKAGE_NAME "cef_binary_${CEF_SDK_VERSION}_${CEF_SDK_PLATFORM}x${CEF_SDK_ARCH}")
else()
  set(CEF_SDK_PACKAGE_NAME "cef_binary_${CEF_SDK_VERSION}_${CEF_SDK_PLATFORM}${CEF_SDK_ARCH}")
endif()

if(NOT CUSTOM_CEF_SDK_DIR)
  # Scan extracted sdk dir
  set(CEF_SDK_EXTRACTED_DIR "${CEF_SDK_WORKSPACE}/${CEF_SDK_PACKAGE_NAME}")
  file(GLOB CEF_SDK_DIR "${CEF_SDK_EXTRACTED_DIR}")
else()
  message(STATUS "CUSTOM_CEF_SDK_DIR set: ${CUSTOM_CEF_SDK_DIR}")
  set(CEF_SDK_DIR "${CUSTOM_CEF_SDK_DIR}")
endif()

# output
message(STATUS "CEF SDK dir: ${CEF_SDK_DIR}")

# #################################################################################
# Stage 2. Extract CEF binary package
if(NOT EXISTS ${CEF_SDK_DIR})
  set(CEF_LOCAL_PACKAGE_PATH "${CEF_SDK_WORKSPACE}/${CEF_SDK_PACKAGE_NAME}.tar.bz2")

  # if no local cef sdk package file then download it
  if(NOT EXISTS "${CEF_LOCAL_PACKAGE_PATH}")
    set(CEF_SDK_DOWNLOAD_URL "https://cef-builds.spotifycdn.com/${CEF_SDK_PACKAGE_NAME}.tar.bz2")
    message(STATUS "Downloading CEF binary SDK from ${CEF_SDK_DOWNLOAD_URL}")
    file(DOWNLOAD
      "${CEF_SDK_DOWNLOAD_URL}" # URL
      "${CEF_LOCAL_PACKAGE_PATH}" # Local Path
      SHOW_PROGRESS
      TLS_VERIFY ON
      STATUS DOWNLOAD_RESULT
    )
    list(GET DOWNLOAD_RESULT 0 DOWNLOAD_RESULT_CODE)
    list(GET DOWNLOAD_RESULT 1 DOWNLOAD_RESULT_MESSAGE)

    if(NOT DOWNLOAD_RESULT_CODE EQUAL 0)
      file(REMOVE "${CEF_LOCAL_PACKAGE_PATH}")
      message(FATAL_ERROR "Failed to download CEF binary SDK, ERROR:[${DOWNLOAD_RESULT_CODE}]${DOWNLOAD_RESULT_MESSAGE}")
    endif()
  endif()

  message(STATUS "Extracting CEF binary SDK from ${CEF_LOCAL_PACKAGE_PATH}")

  # Extract
  file(ARCHIVE_EXTRACT
    INPUT "${CEF_LOCAL_PACKAGE_PATH}"
    DESTINATION "${CEF_SDK_WORKSPACE}"
    VERBOSE
  )

  # Capture the dir name
  file(GLOB CEF_SDK_DIR "${CEF_SDK_EXTRACTED_DIR}")
endif()

# #################################################################################
# Stage 3. Config CEF
if(${CMAKE_VERSION} GREATER "3.11")
  cmake_policy(SET CMP0074 NEW)
endif()

if(${CMAKE_VERSION} GREATER "3.12")
  cmake_policy(SET CMP0077 NEW)
endif()

if(OS_WINDOWS)
  add_link_options(/DEBUG)

  if(USE_SANDBOX)
    # cef_sandbox.lib is MT already, must keep the same with it
    set(CEF_RUNTIME_LIBRARY_FLAG "/MT" CACHE STRING "Use static runtime")
    add_compile_options("/MT$<$<CONFIG:Debug>:d>")
  else()
    # either MT or MD is supported
    set(CEF_RUNTIME_LIBRARY_FLAG "/M$<IF:$<BOOL:${STATIC_CRT}>,T,D>" CACHE STRING "Use static runtime" FORCE)
    add_compile_options("/M$<IF:$<BOOL:${STATIC_CRT}>,T,D>$<$<CONFIG:Debug>:d>")
  endif()
else()
  add_compile_options(
    "-g"
    "$<$<CONFIG:DEBUG>:-O0>"
    "$<$<CONFIG:RELEASE>:-O3>"
  )
endif()

# Append CEF root dir to CMAKE_MODULE_PATH
set(CEF_ROOT "${CEF_SDK_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)

# Add libcef dll wrapper
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

if(USE_SANDBOX AND(OS_WINDOWS OR OS_MACOS))
  add_definitions(-DCEF_USE_SANDBOX)

  # message(STATUS "cef_sandbox_lib path:" "${CEF_SANDBOX_LIB_DEBUG}," "${CEF_SANDBOX_LIB_RELEASE}" )
  # Logical target used to link the cef_sandbox library.
  ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
endif()

PRINT_CEF_CONFIG()

# #################################################################################
# Stage 4. Read CEF version and generated CefVersion.h
# set need configure QCefView_global to false
message(STATUS "${CEF_ROOT}/include/cef_version.h")
file(READ "${CEF_ROOT}/include/cef_version.h" cef_sdk_ver_content)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/CefVersion.h")
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/include/CefVersion.h" cef_local_ver_content)
else()
  set(local_cef_version_content "")
endif()

set(Need_Config_CefVersion_File FALSE)

macro(compare_cef_version _verSeg _verReg _sdkContent _localContent)
  # read version seg value from CEF header file
  string(REGEX MATCH "#define ${_verSeg} ${_verReg}" _ "${_sdkContent}")
  set(${_verSeg} ${CMAKE_MATCH_1} CACHE STRING "CEF Version" FORCE)

  string(REGEX MATCH "#define ${_verSeg} ${_verReg}" _ "${_localContent}")
  set(LOCAL_${_verSeg} ${CMAKE_MATCH_1} CACHE STRING "CEF Version" FORCE)

  if(NOT "${${_verSeg}}" STREQUAL "${LOCAL_${_verSeg}}")
    set(Need_Config_CefVersion_File TRUE)
    message(STATUS "${_verSeg} Changed: ${LOCAL_${_verSeg}} -> ${${_verSeg}}")
  else()
    message(STATUS "${_verSeg} Same: ${LOCAL_${_verSeg}} = ${${_verSeg}}")
  endif()
endmacro(compare_cef_version)

compare_cef_version(CEF_VERSION "\"([a-z0-9\.\+\-]+)\"" "${cef_sdk_ver_content}" "${cef_local_ver_content}")
compare_cef_version(CEF_VERSION_MAJOR "([0-9]+)" "${cef_sdk_ver_content}" "${cef_local_ver_content}")
compare_cef_version(CEF_VERSION_MINOR "([0-9]+)" "${cef_sdk_ver_content}" "${cef_local_ver_content}")
compare_cef_version(CEF_VERSION_PATCH "([0-9]+)" "${cef_sdk_ver_content}" "${cef_local_ver_content}")
compare_cef_version(CEF_COMMIT_NUMBER "([0-9]+)" "${cef_sdk_ver_content}" "${cef_local_ver_content}")
compare_cef_version(CEF_COMMIT_HASH "\"([a-z0-9]+)\"" "${cef_sdk_ver_content}" "${cef_local_ver_content}")

if(${Need_Config_CefVersion_File})
  message(STATUS "Need to configure CefVersion.h file")
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CefVersion.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/CefVersion.h"
    @ONLY
    NEWLINE_STYLE UNIX
  )
else()
  message(STATUS "No need to configure CefVersion.h file")
endif()
