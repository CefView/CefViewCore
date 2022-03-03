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
#include "SchemeHandlers/CefViewDefaultSchemeHandler.h"

#include <CefViewCoreProtocol.h>

CefViewBrowserApp::CefViewBrowserApp(const std::string& bridge_name,
                                     CefViewBrowserAppDelegateInterface::RefPtr delegate)
  : bridge_object_name_(bridge_name)
  , app_delegate_(delegate)
{}

CefViewBrowserApp::~CefViewBrowserApp()
{
  log_debug("CefViewBrowserApp::~CefViewBrowserApp");
}

void
CefViewBrowserApp::RegisterCustomSchemesHandlerFactories()
{
  CefViewDefaultSchemeHandler::RegisterSchemeHandlerFactory();
}

void
CefViewBrowserApp::RegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
  CefViewDefaultSchemeHandler::RegisterScheme(registrar);
}

//////////////////////////////////////////////////////////////////////////
void
CefViewBrowserApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{}

void
CefViewBrowserApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{}

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
    delegate->onScheduleMessageLoopWork(delay_ms);
}
