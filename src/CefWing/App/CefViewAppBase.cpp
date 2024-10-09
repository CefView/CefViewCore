#include "CefViewAppBase.h"

#include <Common/CefViewCoreLog.h>

#include <CefViewCoreProtocol.h>

// These flags must match the Chromium values.
const char kProcessType[] = "type";
const char kZygoteProcess[] = "zygote";
const char kRendererProcess[] = "renderer";

CefViewAppBase::CefViewAppBase(const std::string& scheme_name)
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

  const std::string& process_type = command_line->GetSwitchValue(kProcessType);
  logI("process type parameter is: %s", process_type.c_str());
  if (process_type == kZygoteProcess) {
    // for linux only
    return ZygoteProcess;
  } else if (process_type == kRendererProcess) {
    return RendererProcess;
  }

  return OtherProcess;
}

std::string
CefViewAppBase::GetBridgeObjectName(CefRefPtr<CefCommandLine> command_line)
{
  if (!command_line->HasSwitch(kCefViewBridgeObjectNameKey))
    return "";

  const std::string& name = command_line->GetSwitchValue(kCefViewBridgeObjectNameKey);
  logI("bridge object name: %s", name.c_str());
  return name;
}

std::string
CefViewAppBase::GetBuiltinSchemeName(CefRefPtr<CefCommandLine> command_line)
{
  if (!command_line->HasSwitch(kCefViewBuiltinSchemeNameKey))
    return "";

  const std::string& name = command_line->GetSwitchValue(kCefViewBuiltinSchemeNameKey);
  logI("built-in scheme name: %s", name.c_str());
  return name;
}

void
CefViewAppBase::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
  if (registrar) {
    registrar->AddCustomScheme(builtin_scheme_name_.empty() ? kCefViewDefaultBuiltinSchemaName : builtin_scheme_name_,
                               0);
  }
}
