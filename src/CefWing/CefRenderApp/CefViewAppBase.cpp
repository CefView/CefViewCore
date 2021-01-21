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
  return (process_type == kRendererProcess) ? RendererProcess : OtherProcess;
}

std::string
CefViewAppBase::GetBridgeObjectName(CefRefPtr<CefCommandLine> command_line)
{
  if (!command_line->HasSwitch(CEFVIEW_BRIDGE_OBJ_NAME_KEY))
    return "";

  const std::string& name = command_line->GetSwitchValue(CEFVIEW_BRIDGE_OBJ_NAME_KEY);
  return name;
}
