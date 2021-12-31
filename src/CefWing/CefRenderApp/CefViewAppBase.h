//
//  CefWingAppBase.hpp
//  CefViewWing
//
//  Created by Sheen Tian on 2020/6/17.
//

#ifndef CefAppBase_h
#define CefAppBase_h

#pragma region std_headers
#include <string>
#include <vector>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion cef_headers

class CefViewAppBase : public CefApp
{
public:
  CefViewAppBase();

  enum ProcessType
  {
    UnkownProcess,
    ZygoteProcess,
    RendererProcess,
    OtherProcess,
  };

  // Determine the process type based on command-line arguments.
  static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

  // Determine the bridge object name.
  static std::string GetBridgeObjectName(CefRefPtr<CefCommandLine> command_line);

protected:
  // Schemes that will be registered with the global cookie manager.
  std::vector<CefString> cookieable_schemes_;
};

#endif
