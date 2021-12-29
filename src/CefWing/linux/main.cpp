//
//  main.cpp
//  CeViewfWing
//
//  Created by Sheen Tian on 2020/6/10.
//

#pragma region cef_headers
#include <include/cef_app.h>
#if defined(CEF_USE_SANDBOX)
#include <include/cef_sandbox_win.h>
#endif
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
  logI("CefViewWindg is launching....");

  void* sandbox_info = NULL;
#if defined(CEF_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  CefMainArgs main_args(argc, argv);

  CefRefPtr<CefApp> app;
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);
  auto process_type = CefViewAppBase::GetProcessType(command_line);
  if (process_type == CefViewAppBase::RendererProcess) {
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
