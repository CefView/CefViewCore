file(GLOB_RECURSE CefViewCore_SHARED_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Shared/*.h"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/Shared" 
  PREFIX Shared
  FILES ${CefViewCore_SHARED_SRC_FILES}
)

### CefViewCore 
################################################################################################
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

if (OS_WINDOWS)
  SET_EXECUTABLE_TARGET_PROPERTIES(CefViewCore)
  
  ADD_LOGICAL_TARGET(libcef_lib "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

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

  target_link_libraries(CefViewCore PUBLIC
    ${CefViewCore_LIBS}
  )
endif()

if (OS_MACOS)
  file(GLOB_RECURSE CefViewCore_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h"
  )
  
  set_property(TARGET CefViewCore 
    APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc"
  )

  set_target_properties(CefViewCore
    PROPERTIES
      FRAMEWORK TRUE
      PUBLIC_HEADER                                               "${CefViewCore_PUBLIC_HEADERS}"
      CLANG_ENABLE_OBJC_ARC                                       "YES"
      XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD                 "gnu++11"   # -std=gnu++11
      XCODE_ATTRIBUTE_CLANG_LINK_OBJC_RUNTIME                     "NO"        # -fno-objc-link-runtime
      XCODE_ATTRIBUTE_COPY_PHASE_STRIP                            "NO"
      XCODE_ATTRIBUTE_DEAD_CODE_STRIPPING[variant=Release]        "YES"       # -Wl,-dead_strip
      XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD                     "c99"       # -std=c99
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER                   "com.cefview.cefview"
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

  target_link_libraries(CefViewCore PUBLIC
    ${CefViewCore_LIBS}
  )
endif()

### CefViewWing
################################################################################################
# soruce code for all platforms
file(GLOB_RECURSE CefViewWing_SRC_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/CefRenderApp/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/CefRenderApp/*.cpp"
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing"
  PREFIX Source 
  FILES ${CefViewWing_SRC_FILES}
)

if (OS_WINDOWS)
  file(GLOB_RECURSE CefViewWing_PLATFORM_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win/*.cpp"
  )
  source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/win"
    PREFIX Source
    FILES ${CefViewWing_PLATFORM_SRC_FILES}
  )

  # Create Helper executable target.
  add_executable(CefViewWing WIN32
    ${CefViewCore_SHARED_SRC_FILES}
    ${CefViewWing_SRC_FILES}
    ${CefViewWing_PLATFORM_SRC_FILES}
  )

  target_include_directories(CefViewWing PRIVATE
    ${CefViewCore_INCLUDE_PATH}
  )

  add_dependencies(CefViewWing 
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

  target_link_libraries(CefViewWing 
    ${_helper_libs}
  )

  # Add the Helper as a dependency of the main executable target.
  add_dependencies(CefViewCore CefViewWing)

  # copy cef binary files
  add_custom_command(TARGET CefViewWing POST_BUILD
    COMMAND mt.exe 
    -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\CefWing\\win\\CefViewWing.manifest\" 
    -inputresource:\"$<TARGET_FILE:CefViewWing>\" 
    -outputresource:\"$<TARGET_FILE:CefViewWing>\"
    
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CEF_RESOURCE_DIR}" "$<TARGET_FILE_DIR:CefViewWing>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:CefViewWing>/resources/icudtl.dat" "$<TARGET_FILE_DIR:CefViewWing>"
    COMMAND ${CMAKE_COMMAND} -E remove "$<TARGET_FILE_DIR:CefViewWing>/resources/icudtl.dat"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CEF_BINARY_DIR}" "$<TARGET_FILE_DIR:CefViewWing>"
  )
endif()  # OS_WINDOWS

if (OS_MACOS)
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

  # Create the multiple Helper app bundle targets.
  if(NOT CEF_HELPER_APP_SUFFIXES)
    message("-- CEF_HELPER_APP_SUFFIXES is empty, use default instead")
    set(CEF_HELPER_APP_SUFFIXES "::")
  endif()

  # Create Helper executable target.
  add_executable(CefViewWing MACOSX_BUNDLE 
    ${CefViewCore_SHARED_SRC_FILES}
    ${CefViewWing_SRC_FILES}
    ${CefViewWing_PLATFORM_SRC_FILES}
  )

  target_include_directories(CefViewWing PRIVATE 
    ${CefViewCore_INCLUDE_PATH}
  )

  SET_EXECUTABLE_TARGET_PROPERTIES(CefViewWing)
  add_dependencies(CefViewWing 
    libcef_dll_wrapper
  )

  set(CefViewWing_LIBS
    libcef_dll_wrapper 
    ${CEF_STANDARD_LIBS}
  )
  
  if(USE_SANDBOX)
    list(APPEND CefViewWing_LIBS cef_sandbox_lib)
  endif()

  target_link_libraries(CefViewWing 
    ${CefViewWing_LIBS}
  )

  set_target_properties(CefViewWing
    PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST                  "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/info.plist"
      CLANG_ENABLE_OBJC_ARC                     "YES"
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.cefview.cefviewwing"
      XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME   "YES"
      XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS    "${CMAKE_CURRENT_SOURCE_DIR}/CefWing/mac/CefViewWing.entitlements"
  )

  # Add the Helper as a dependency of the main executable target.
  add_dependencies(CefViewCore CefViewWing)

  # copy cef binary files to the output folder
  add_custom_command(TARGET CefViewWing POST_BUILD
    #copy the cef framework to resource directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework" "$<TARGET_BUNDLE_DIR:CefViewWing>/../Chromium Embedded Framework.framework"
    VERBATIM
  )
endif() # OS_MACOS