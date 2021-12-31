//
//  main.cpp
//  CeViewfWing
//
//  Created by Sheen Tian on 2020/6/10.
//

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion cef_headers

#pragma region win_headers

#pragma endregion win_headers

#pragma region project_heasers
#include "../CefRenderApp/CefViewAppBase.h"
#include "../CefRenderApp/CefViewOtherApp.h"
#include "../CefRenderApp/CefViewRenderApp.h"
#include <Common/CefViewCoreLog.h>
#pragma endregion project_heasers

int
CefViewWingMain(int argc, char* argv[])
{
  logI("CefViewWing is launching....");
  CefMainArgs main_args(argc, argv);

  CefRefPtr<CefApp> app;
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);
  auto process_type = CefViewAppBase::GetProcessType(command_line);
  if (process_type == CefViewAppBase::RendererProcess || process_type == CefViewAppBase::ZygoteProcess) {
    auto bridge_name = CefViewAppBase::GetBridgeObjectName(command_line);
    app = new CefViewRenderApp(bridge_name);
  } else if (process_type == CefViewAppBase::OtherProcess) {
    app = new CefViewOtherApp();
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
