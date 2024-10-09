//
//  main.cpp
//  CeViewfWing
//
//  Created by Sheen Tian on 2020/6/10.
//

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_version.h>
#if defined(CEF_USE_SANDBOX)
#include <include/cef_sandbox_win.h>
#endif
#pragma endregion

#include <CefViewCoreProtocol.h>

#include <Common/CefViewCoreLog.h>

#include "../App/CefViewAppBase.h"
#include "../App/CefViewOtherApp.h"
#include "../App/CefViewRenderApp.h"

int
CefViewWingMain(HINSTANCE hInstance)
{
  logI("CefViewWing is launching....");
#if CEF_VERSION_MAJOR < 112
  CefEnableHighDPISupport();
#endif

  void* sandbox_info = NULL;
#if defined(CEF_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  CefMainArgs main_args(hInstance);

  CefRefPtr<CefApp> app;
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // get parameter from command line
  auto process_type = CefViewAppBase::GetProcessType(command_line);
  auto builtin_scheme_name = CefViewAppBase::GetBuiltinSchemeName(command_line);
  auto bridge_object_name = CefViewAppBase::GetBridgeObjectName(command_line);

  // create application
  if (process_type == CefViewAppBase::RendererProcess) {
    app = new CefViewRenderApp(builtin_scheme_name, bridge_object_name);
  } else if (process_type == CefViewAppBase::OtherProcess) {
    app = new CefViewOtherApp(builtin_scheme_name);
  } else {
    logI("Parse process unknown, exit");
    return 1;
  }

  // assign current process to window job
  if (command_line->HasSwitch(kCefViewWindowsJobNameKey)) {
    auto job_name = command_line->GetSwitchValue(kCefViewWindowsJobNameKey);
    HANDLE job_handle = ::OpenJobObjectA(JOB_OBJECT_ASSIGN_PROCESS, FALSE, job_name.ToString().c_str());
    if (job_handle) {
      if (!::AssignProcessToJobObject(job_handle, ::GetCurrentProcess())) {
        logE("Failed to assign current process to windows job object: %d", ::GetLastError());
      }
      ::CloseHandle(job_handle);
    } else {
      logE("Failed to open windows job object: %d", ::GetLastError());
    }
  }

  // Execute the secondary process.
  int rt = CefExecuteProcess(main_args, app, sandbox_info);
  logI("process returned with code: %d", rt);

  return rt;
}

// Program entry point function.
int APIENTRY
wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  return CefViewWingMain(hInstance);
}
