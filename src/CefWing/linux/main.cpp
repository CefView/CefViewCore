//
//  main.cpp
//  CeViewfWing
//
//  Created by Sheen Tian on 2020/6/10.
//

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion

#include <Common/CefViewCoreLog.h>

#include "../App/CefViewAppBase.h"
#include "../App/CefViewOtherApp.h"
#include "../App/CefViewRenderApp.h"

int
CefViewWingMain(int argc, char* argv[])
{
  logI("CefViewWing is launching....");
  CefMainArgs main_args(argc, argv);

  CefRefPtr<CefApp> app;
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);

  // get parameter from command line
  auto process_type = CefViewAppBase::GetProcessType(command_line);
  auto builtin_scheme_name = CefViewAppBase::GetBuiltinSchemeName(command_line);
  auto bridge_object_name = CefViewAppBase::GetBridgeObjectName(command_line);

  if (process_type == CefViewAppBase::RendererProcess || process_type == CefViewAppBase::ZygoteProcess) {
    app = new CefViewRenderApp(builtin_scheme_name, bridge_object_name);
  } else if (process_type == CefViewAppBase::OtherProcess) {
    app = new CefViewOtherApp(builtin_scheme_name);
  } else {
    logI("Parse process unknown, exit");
    return 1;
  }

  // Execute the secondary process.
  int rt = CefExecuteProcess(main_args, app, nullptr);
  logI("process returned with code: %d", rt);

  return rt;
}

// Program entry point function.
int
main(int argc, const char* argv[])
{
  return CefViewWingMain(argc, const_cast<char**>(argv));
}
