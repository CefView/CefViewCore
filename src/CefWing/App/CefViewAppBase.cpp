#include "CefViewAppBase.h"

#include <Common/CefViewCoreLog.h>

#include <CefViewCoreProtocol.h>

// These flags must match the Chromium values.
const char kProcessType[] = "type";
const char kZygoteProcess[] = "zygote";
const char kRendererProcess[] = "renderer";

CefViewAppBase::CefViewAppBase(const CefString& scheme_name)
  : builtin_scheme_name_(scheme_name)
{
}

// static
CefViewAppBase::ProcessType
CefViewAppBase::GetProcessType(CefRefPtr<CefCommandLine> command_line)
{
  // The command-line flag won't be specified for the browser process.
  if (!command_line->HasSwitch(kProcessType))
    return UnkownProcess;

  auto process_type = command_line->GetSwitchValue(kProcessType);
  logI("process type parameter is: %s", process_type.c_str());
  if (process_type == kZygoteProcess) {
    // for linux only
    return ZygoteProcess;
  } else if (process_type == kRendererProcess) {
    return RendererProcess;
  }

  return OtherProcess;
}

CefString
CefViewAppBase::GetBridgeObjectName(CefRefPtr<CefCommandLine> command_line)
{
  if (!command_line->HasSwitch(kCefViewBridgeObjectNameKey))
    return "";

  auto name = command_line->GetSwitchValue(kCefViewBridgeObjectNameKey);
  logI("bridge object name: %s", name.c_str());
  return name;
}

CefString
CefViewAppBase::GetBuiltinSchemeName(CefRefPtr<CefCommandLine> command_line)
{
  if (!command_line->HasSwitch(kCefViewBuiltinSchemeNameKey))
    return "";

  auto name = command_line->GetSwitchValue(kCefViewBuiltinSchemeNameKey);
  logI("built-in scheme name: %s", name.c_str());
  return name;
}

void
CefViewAppBase::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
  if (registrar) {
    int options = 0                                 //
                  | CEF_SCHEME_OPTION_STANDARD      //
                  | CEF_SCHEME_OPTION_SECURE        //
                  | CEF_SCHEME_OPTION_CORS_ENABLED  //
                  | CEF_SCHEME_OPTION_FETCH_ENABLED //
                  | 0;
    auto scheme = builtin_scheme_name_.empty() ? kCefViewDefaultBuiltinSchemaName : builtin_scheme_name_;
    if (!registrar->AddCustomScheme(scheme, options)) {
      logE("faield to add built-in scheme: %s", scheme.c_str());
    }
  }
}
