# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Generally, there is NO NEED to modify the following config
#
# Download CEF binary package
#
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

# Scan extracted sdk dir
set(CEF_SDK_EXTRACTED_DIR "${CEF_SDK_WORKSPACE}/${CEF_SDK_PACKAGE_NAME}")
file(GLOB CEF_SDK_DIR "${CEF_SDK_EXTRACTED_DIR}")

# Extract CEF binary package
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

# output
message(STATUS "CEF SDK dir: ${CEF_SDK_DIR}")
