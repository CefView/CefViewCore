#
# The link for downloading the CEF binary sdk 
#
if(OS_WINDOWS)
  set(CEF_SDK_URL
    # Change this value to swith between different CEF versions.
    "https://cef-builds.spotifycdn.com/cef_binary_89.0.12%2Bg2b76680%2Bchromium-89.0.4389.90_windows64.tar.bz2"
    )
elseif(OS_MACOS)
  set(CEF_SDK_URL
    # Change this value to swith between different CEF versions.
    "https://cef-builds.spotifycdn.com/cef_binary_89.0.12%2Bg2b76680%2Bchromium-89.0.4389.90_macosx64.tar.bz2"
    )
elseif (OS_LINUX)
  set(CEF_SDK_URL
    # Change this value to swith between different CEF versions.
    "https://cef-builds.spotifycdn.com/cef_binary_89.0.12%2Bg2b76680%2Bchromium-89.0.4389.90_linux64.tar.bz2"
    )
else()
  message(FATAL_ERROR "Unsupported plaftorm")
endif()


#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Usually, there is NO NEED to modify the following config
#
# Download CEF binary package
#
set (CEF_SDK_PACKAGE "${CMAKE_CURRENT_SOURCE_DIR}/dep/cef_package.tar.bz2")
if(NOT EXISTS "${CEF_SDK_PACKAGE}")
  file(DOWNLOAD 
    "${CEF_SDK_URL}"        # URL 
    "${CEF_SDK_PACKAGE}"    # Local Path
    SHOW_PROGRESS 
    TLS_VERIFY ON)
endif()

#
# Extact the CEF SDK dir
#
file(GLOB CEF_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dep/cef_binary_*")

#
# Extract CEF binary package
#
if (NOT EXISTS ${CEF_SDK_DIR})
  message(STATUS "CEF SDK dir does not exist, extracting new one....")

  # Extract
  file(ARCHIVE_EXTRACT
    INPUT "${CEF_SDK_PACKAGE}"
    DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/dep")

  # Capture the dir name
  file(GLOB CEF_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dep/cef_binary_*")
endif()

# output
message(STATUS "CEF SDK dir: ${CEF_SDK_DIR}")
