#include <CefViewBrowserApp.h>

#pragma region std_headers
#include <string>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#include "Common/CefViewCoreLog.h"

#include <CefViewCoreProtocol.h>

CefViewBrowserApp::CefViewBrowserApp(const std::string& bridge_name,
                                     CefViewBrowserAppDelegateInterface::WeakPtr delegate)
  : bridge_object_name_(bridge_name)
  , app_delegate_(delegate)
{}

CefViewBrowserApp::~CefViewBrowserApp()
{
  log_debug("CefViewBrowserApp::~CefViewBrowserApp");
}

//////////////////////////////////////////////////////////////////////////
void
CefViewBrowserApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
  // set switches
  if (process_type.empty()) {
    // command_line->AppendSwitch("no-proxy-server");
    command_line->AppendSwitch("use-mock-keychain");
    command_line->AppendSwitch("disable-extensions");
    command_line->AppendSwitch("disable-web-security");
    command_line->AppendSwitch("disable-pdf-extension");
    command_line->AppendSwitch("disable-spell-checking");
    command_line->AppendSwitch("disable-remote-core-animation");
    command_line->AppendSwitch("disable-site-isolation-trials");
    command_line->AppendSwitch("enable-aggressive-domstorage-flushing");

    // set switches with value
    command_line->AppendSwitchWithValue("renderer-process-limit", "1");
    command_line->AppendSwitchWithValue("disable-features", "BlinkGenPropertyTrees,TranslateUI,site-per-process");
  }
}

void
CefViewBrowserApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
  RegisterCustomSchemes(registrar);
}

CefRefPtr<CefBrowserProcessHandler>
CefViewBrowserApp::GetBrowserProcessHandler()
{
  return this;
}

//////////////////////////////////////////////////////////////////////////
void
CefViewBrowserApp::OnContextInitialized()
{
  CEF_REQUIRE_UI_THREAD();

  // Register cookieable schemes with the global cookie manager.
  CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
  DCHECK(manager.get());
  typedef std::vector<CefString> CookiableSchemeSet;
  CookiableSchemeSet cookieable_schemes_;
  manager->SetSupportedSchemes(cookieable_schemes_, true, nullptr);

  // TO-DO (sheen) disable the custom scheme for now
  // RegisterCustomSchemesHandlerFactories();
}

void
CefViewBrowserApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
  if (bridge_object_name_.empty())
    bridge_object_name_ = CEFVIEW_OBJECT_NAME;

  command_line->AppendSwitchWithValue(CEFVIEW_BRIDGE_OBJ_NAME_KEY, bridge_object_name_);
}

void
CefViewBrowserApp::OnScheduleMessagePumpWork(int64 delay_ms)
{
  auto delegate = app_delegate_.lock();

  if (delegate)
    delegate->OnScheduleMessageLoopWork(delay_ms);
}
