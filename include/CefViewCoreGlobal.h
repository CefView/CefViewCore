//
//  CefViewCoreGlobal.h
//  CefViewCore
//
//  Created by Sheen Tian on 2023/5/30.
//

#ifndef CefViewCoreGlobal_h
#define CefViewCoreGlobal_h
#pragma once

#pragma region stl_headers
#include <string>
#include <type_traits>
#pragma endregion

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_version.h>
#include <include/cef_parser.h>
#include <include/wrapper/cef_helpers.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#pragma endregion

#if CEF_VERSION_MAJOR < 122
using CefFrameId = int64_t;
#else
using CefFrameId = CefString;

template<>
struct std::hash<CefFrameId>
{
  std::size_t operator()(const CefFrameId& k) const
  {
    using std::hash;
    using std::string;
    return std::hash<std::string>()(k.ToString());
  }
};
#endif // CEF_VERSION_MAJOR < 122

#endif // CefViewCoreGlobal_h
