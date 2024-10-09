file(GLOB_RECURSE CefViewCore_SHARED_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared/*.h"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/Shared"
  PREFIX Shared
  FILES ${CefViewCore_SHARED_SRC_FILES}
)

# ## CefViewCore
# ###############################################################################################
# header files
file(GLOB_RECURSE CefViewCore_INCLUDE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  PREFIX Include
  FILES ${CefViewCore_INCLUDE_FILES}
)

# soruce code for all platforms
file(GLOB_RECURSE CefViewCore_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/CefView/CefBrowserApp/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefView/CefBrowserApp/*.cpp"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefView/CefBrowserApp"
  PREFIX Source
  FILES ${CefViewCore_SRC_FILES}
)

add_library(CefViewCore STATIC
  ${CefViewCore_SHARED_SRC_FILES}
  ${CefViewCore_INCLUDE_FILES}
  ${CefViewCore_SRC_FILES}
)

target_include_directories(CefViewCore PRIVATE
  ${CefViewCore_INCLUDE_PATH}
)

# ADD_LOGICAL_TARGET(libcef_lib
# "${CEF_LIB_DEBUG}"
# "${CEF_LIB_RELEASE}"
# )
add_library(libcef_lib ${CEF_LIBTYPE} IMPORTED GLOBAL)
set_target_properties(libcef_lib
  PROPERTIES
  IMPORTED_LOCATION "${CEF_LIB_RELEASE}"
  IMPORTED_LOCATION_DEBUG "${CEF_LIB_DEBUG}"
  IMPORTED_LOCATION_RELEASE "${CEF_LIB_RELEASE}"
)

if(OS_WINDOWS)
  SET_LIBRARY_TARGET_PROPERTIES(CefViewCore)

  add_dependencies(CefViewCore
    libcef_lib
    libcef_dll_wrapper
  )

  set(CefViewCore_LIBS
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  if(USE_SANDBOX)
    list(APPEND CefViewCore_LIBS cef_sandbox_lib)
  endif()

  target_link_libraries(CefViewCore
    PUBLIC
    ${CefViewCore_LIBS}
  )
endif() # OS_WINDOWS

if(OS_LINUX)
  SET_LIBRARY_TARGET_PROPERTIES(CefViewCore)

  add_dependencies(CefViewCore
    libcef_lib
    libcef_dll_wrapper
  )

  set(CefViewCore_LIBS
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  target_link_libraries(CefViewCore
    PUBLIC
    ${CefViewCore_LIBS}
  )
endif() # OS_LINUX

if(OS_MACOS)
  file(GLOB_RECURSE CefViewCore_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h"
  )

  target_compile_options(CefViewCore
    PRIVATE
    "-fobjc-arc"
  )

  SET_LIBRARY_TARGET_PROPERTIES(CefViewCore)

  set_target_properties(CefViewCore
    PROPERTIES
    FRAMEWORK TRUE
    PUBLIC_HEADER "${CefViewCore_PUBLIC_HEADERS}"
    CLANG_ENABLE_OBJC_ARC "YES"
    APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc"
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "gnu++11" # -std=gnu++11
    XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME "NO" # -fno-objc-link-runtime
    XCODE_ATTRIBUTE_COPY_PHASE_STRIP "NO"
    XCODE_ATTRIBUTE_DEAD_CODE_STRIPPING[variant=Release] "YES" # -Wl,-dead_strip
    XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD "c99" # -std=c99
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.cefview.cefview"
  )

  add_dependencies(CefViewCore
    libcef_dll_wrapper
  )

  set(CefViewCore_LIBS
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  if(USE_SANDBOX)
    list(APPEND CefViewCore_LIBS cef_sandbox_lib)
  endif()

  target_link_libraries(CefViewCore
    PUBLIC
    ${CefViewCore_LIBS}
  )
endif() # OS_MACOS

# ## CefViewWing
# ###############################################################################################
# soruce code for all platforms
file(GLOB_RECURSE CefViewWing_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/App/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/App/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/Bridge/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/Bridge/*.cpp"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing"
  PREFIX Source
  FILES ${CefViewWing_SRC_FILES}
)

if(OS_WINDOWS)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  if(EXISTS ${CEFVIEW_WING_ICON})
    # Copy icon file to binary dir
    configure_file(
      "${CEFVIEW_WING_ICON}"
      "${CMAKE_CURRENT_BINARY_DIR}/app.ico"
      COPYONLY
    )

    # Config resource file
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/resource.rc.in"
      "${CMAKE_CURRENT_BINARY_DIR}/resource.rc"
      COPYONLY
    )
    set(CefViewWing_WIN_RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
  else()
    set(CefViewWing_WIN_RESOURCE_FILE "")
  endif()

  # Create Helper executable target.
  add_executable(${CEFVIEW_WING_NAME} WIN32
    ${CefViewCore_SHARED_SRC_FILES}
    ${CefViewWing_SRC_FILES}
    ${CefViewWing_PLATFORM_SRC_FILES}
    ${CefViewWing_WIN_RESOURCE_FILE}
  )

  SET_EXECUTABLE_TARGET_PROPERTIES(${CEFVIEW_WING_NAME})
  target_link_options(${CEFVIEW_WING_NAME}
    PRIVATE
    "/MANIFEST"
  )

  target_include_directories(${CEFVIEW_WING_NAME}
    PRIVATE
    ${CefViewCore_INCLUDE_PATH}
  )

  add_dependencies(${CEFVIEW_WING_NAME}
    libcef_lib
    libcef_dll_wrapper
  )

  set(_helper_libs
    d3d11.lib
    glu32.lib
    imm32.lib
    opengl32.lib

    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
    ${CEF_SANDBOX_STANDARD_LIBS}
  )

  if(USE_SANDBOX)
    list(APPEND _helper_libs cef_sandbox_lib)
  endif()

  target_link_libraries(${CEFVIEW_WING_NAME}
    ${_helper_libs}
  )

  # Add the Helper as a dependency of the main executable target.
  add_dependencies(CefViewCore ${CEFVIEW_WING_NAME})

  add_custom_command(TARGET ${CEFVIEW_WING_NAME}
    POST_BUILD

    # embed the manifest file
    COMMAND mt.exe
    -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\CefWing\\win\\CefViewWing.manifest\"
    -outputresource:\"$<TARGET_FILE:${CEFVIEW_WING_NAME}>\"

    # copy cef binary files
    # .1 copy the cef resources to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_RESOURCE_DIR}"
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/resources"

    # .2 move the icudtl.dat to root of output dir
    COMMAND ${CMAKE_COMMAND} -E rename
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/resources/icudtl.dat"
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/icudtl.dat"

    # .3 copy all cef binary files to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}"
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>"
  )
endif() # OS_WINDOWS

if(OS_LINUX)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/linux/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/linux/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/linux"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  # Create Helper executable target.
  add_executable(${CEFVIEW_WING_NAME}
    ${CefViewCore_SHARED_SRC_FILES}
    ${CefViewWing_SRC_FILES}
    ${CefViewWing_PLATFORM_SRC_FILES}
  )

  SET_EXECUTABLE_TARGET_PROPERTIES(${CEFVIEW_WING_NAME})

  set_target_properties(${CEFVIEW_WING_NAME}
    PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE
  )

  target_include_directories(${CEFVIEW_WING_NAME} PRIVATE
    ${CefViewCore_INCLUDE_PATH}
  )

  add_dependencies(${CEFVIEW_WING_NAME}
    libcef_lib
    libcef_dll_wrapper
  )

  set(_helper_libs
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  target_link_libraries(${CEFVIEW_WING_NAME}
    ${_helper_libs}
  )

  # Add the Helper as a dependency of the main executable target.
  add_dependencies(CefViewCore ${CEFVIEW_WING_NAME})

  # copy cef binary files
  add_custom_command(TARGET ${CEFVIEW_WING_NAME}
    POST_BUILD

    # copy cef binary files
    # .1 copy the cef resources to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_RESOURCE_DIR}"
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/resources"

    # .2 move the icudtl.dat to root of output dir
    COMMAND ${CMAKE_COMMAND} -E rename
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/resources/icudtl.dat"
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/icudtl.dat"

    # .3 copy all cef binary files to output dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}"
    "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>"
  )

  SET_LINUX_SUID_PERMISSIONS(${CEFVIEW_WING_NAME} "$<TARGET_FILE_DIR:${CEFVIEW_WING_NAME}>/chrome-sandbox")
endif() # OS_LINUX

if(OS_MACOS)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.m"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  set(CefViewWing_LIBS
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )

  if(USE_SANDBOX)
    list(APPEND CefViewWing_LIBS cef_sandbox_lib)
  endif()

  # Create the multiple Helper app bundle targets.
  foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
    # Convert to a list and extract the suffix values.
    string(REPLACE ":" ";" _suffix_list ${_suffix_list})
    list(GET _suffix_list 0 _name_suffix)
    list(GET _suffix_list 1 _target_suffix)
    list(GET _suffix_list 2 _plist_suffix)

    # Define Helper target and output names.
    set(_helper_target "${CEFVIEW_WING_NAME}${_target_suffix}")
    set(_helper_output_name "${CEFVIEW_WING_NAME}${_name_suffix}")

    # Create Helper executable target.
    add_executable(${_helper_target} MACOSX_BUNDLE
      ${CefViewCore_SHARED_SRC_FILES}
      ${CefViewWing_SRC_FILES}
      ${CefViewWing_PLATFORM_SRC_FILES}
    )
    SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})

    set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/info.plist" _plist_contents)
    file(WRITE ${_helper_info_plist} ${_plist_contents})

    set_target_properties(${_helper_target}
      PROPERTIES
      CLANG_ENABLE_OBJC_ARC "YES"
      OUTPUT_NAME "${_helper_output_name}"
      MACOSX_BUNDLE_INFO_PLIST "${_helper_info_plist}"
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.cefview.${CEFVIEW_WING_NAME}${_plist_suffix}"
      XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
      XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/CefViewWing.entitlements"
    )

    target_include_directories(${_helper_target} PRIVATE
      ${CefViewCore_INCLUDE_PATH}
    )

    add_dependencies(${_helper_target}
      libcef_dll_wrapper
    )

    target_link_libraries(${_helper_target}
      ${CefViewWing_LIBS}
    )

    # Add the Helper as a dependency of the main executable target.
    add_dependencies(CefViewCore ${_helper_target})
  endforeach()

  # copy cef binary files to the output folder
  add_custom_command(TARGET ${CEFVIEW_WING_NAME}
    POST_BUILD

    # copy the cef framework to output directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
    "$<TARGET_BUNDLE_DIR:${CEFVIEW_WING_NAME}>/../Chromium Embedded Framework.framework"
    VERBATIM
  )
endif() # OS_MACOS
