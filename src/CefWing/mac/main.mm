//
//  main.m
//  CefViewWing
//
//  Created by Sheen Tian on 2020/6/10.
//

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/wrapper/cef_library_loader.h>
#if defined(CEF_USE_SANDBOX)
#include <include/cef_sandbox_mac.h>
#endif
#pragma endregion

#include <Foundation/Foundation.h>
#include <libgen.h>
#include <mach-o/dyld.h>

#include <Common/CefViewCoreLog.h>

#include "../App/CefViewAppBase.h"
#include "../App/CefViewOtherApp.h"
#include "../App/CefViewRenderApp.h"

const char kFrameworkPath[] = "Chromium Embedded Framework.framework/Chromium Embedded Framework";
const char kPathFromHelperExe[] = "../../..";

std::string GetFrameworkPath() {
  uint32_t exec_path_size = 0;
  int rv = _NSGetExecutablePath(NULL, &exec_path_size);
  if (rv != -1) {
    return std::string();
  }

  std::unique_ptr<char[]> exec_path(new char[exec_path_size]);
  rv = _NSGetExecutablePath(exec_path.get(), &exec_path_size);
  if (rv != 0) {
    return std::string();
  }

  // Get the directory path of the executable.
  const char *parent_dir = dirname(exec_path.get());
  if (!parent_dir) {
    return std::string();
  }

  // Append the relative path to the framework.
  std::stringstream ss;
  ss << parent_dir << "/" << kPathFromHelperExe << "/" << kFrameworkPath;
  return ss.str();
}

bool LoadCefLibrary() {
  std::string cef_lib_path = GetFrameworkPath();
  return (1 == cef_load_library(cef_lib_path.c_str()));
}

int CefViewWingMain(int argc, char *argv[]) {
  logI("CefViewWing is launching....");

#if defined(CEF_USE_SANDBOX)
  // Initialize the macOS sandbox for this helper process.
  CefScopedSandboxContext sandbox_context;
  logI("Initializing the sandbox context");
  if (!sandbox_context.Initialize(argc, argv)) {
    logE("Failed to initialize the cef sandbox context");
    return 1;
  }
#endif

  logI("Loading the cef library");
  if (!LoadCefLibrary()) {
    logI("Failed to load cef library");
    return 1;
  }

  CefMainArgs main_args(argc, argv);

  CefRefPtr<CefApp> app;
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);

  // get parameter from command line
  auto process_type = CefViewAppBase::GetProcessType(command_line);
  auto builtin_scheme_name = CefViewAppBase::GetBuiltinSchemeName(command_line);
  auto bridge_object_name = CefViewAppBase::GetBridgeObjectName(command_line);

  if (process_type == CefViewAppBase::RendererProcess) {
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

int main(int argc, const char *argv[]) {
  // main
  return CefViewWingMain(argc, const_cast<char **>(argv));
}
