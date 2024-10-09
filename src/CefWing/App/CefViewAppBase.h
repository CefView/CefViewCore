//
//  CefWingAppBase.hpp
//  CefViewWing
//
//  Created by Sheen Tian on 2020/6/17.
//

#ifndef CefAppBase_h
#define CefAppBase_h

#pragma region stl_headers
#include <string>
#include <vector>
#pragma endregion

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion

class CefViewAppBase : public CefApp
{
  /// <summary>
  ///
  /// </summary>
  std::string builtin_scheme_name_;

public:
  CefViewAppBase(const std::string& scheme_name);

  enum ProcessType
  {
    UnkownProcess,
    ZygoteProcess,
    RendererProcess,
    OtherProcess,
  };

  /// <summary>
  /// Gets the current process type
  /// </summary>
  /// <param name="command_line">The command line</param>
  /// <returns>The process type</returns>
  static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

  /// <summary>
  /// Gets the bridge object name from command line
  /// </summary>
  /// <param name="command_line">The command line</param>
  /// <returns>The bridge object name</returns>
  static std::string GetBridgeObjectName(CefRefPtr<CefCommandLine> command_line);

  /// <summary>
  /// Gets the built-in scheme name
  /// </summary>
  /// <param name="command_line">The command line</param>
  /// <returns>The built-in scheme name</returns>
  static std::string GetBuiltinSchemeName(CefRefPtr<CefCommandLine> command_line);

private:
  /// <summary>
  ///
  /// </summary>
  /// <param name="registrar"></param>
  virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
};

#endif
