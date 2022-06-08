//
//  CefWingAppBase.cpp
//  CeViewfWing
//
//  Created by Sheen Tian on 2020/6/17.
//

#pragma region project_heasers
#include "CefViewAppBase.h"
#pragma endregion project_heasers

#pragma region mac_headers
#include <Common/CefViewCoreLog.h>
#pragma endregion mac_headers

#include <CefViewCoreProtocol.h>

// These flags must match the Chromium values.
const char kProcessType[] = "type";
const char kZygoteProcess[] = "zygote";
const char kRendererProcess[] = "renderer";

CefViewAppBase::CefViewAppBase() {}

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
