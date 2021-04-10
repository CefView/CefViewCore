#pragma region std_headers
#include <string>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#pragma region project_headers
#include "CefViewRenderApp.h"
#include "RenderDelegates/CefViewDefaultRenderDelegate.h"
#pragma endregion project_headers

CefViewRenderApp::CefViewRenderApp(const std::string& bridge_name)
  : bridge_object_name_(bridge_name)
{
  CefViewDefaultRenderDelegate::CreateBrowserDelegate(render_delegates_, bridge_object_name_);
}

CefViewRenderApp::~CefViewRenderApp() {}

//////////////////////////////////////////////////////////////////////////
CefRefPtr<CefRenderProcessHandler>
CefViewRenderApp::GetRenderProcessHandler()
{
  return this;
}

void
CefViewRenderApp::OnWebKitInitialized()
{
  CEF_REQUIRE_RENDERER_THREAD();

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnWebKitInitialized(this);
}

void
CefViewRenderApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
{
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserCreated(this, browser, extra_info);
}

void
CefViewRenderApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler>
CefViewRenderApp::GetLoadHandler()
{
  CefRefPtr<CefLoadHandler> load_handler;
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !load_handler.get(); ++it)
    load_handler = (*it)->GetLoadHandler(this);

  return load_handler;
}

void
CefViewRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context)
{
  CEF_REQUIRE_RENDERER_THREAD();

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextCreated(this, browser, frame, context);
}

void
CefViewRenderApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefV8Context> context)
{
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextReleased(this, browser, frame, context);
}

void
CefViewRenderApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefV8Context> context,
                                      CefRefPtr<CefV8Exception> exception,
                                      CefRefPtr<CefV8StackTrace> stackTrace)
{
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
}

void
CefViewRenderApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefDOMNode> node)
{
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

bool
CefViewRenderApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefProcessId source_process,
                                           CefRefPtr<CefProcessMessage> message)
{
  CEF_REQUIRE_RENDERER_THREAD();
  DCHECK_EQ(source_process, PID_BROWSER);

  bool handled = false;

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !handled; ++it)
    handled = (*it)->OnProcessMessageReceived(this, browser, frame, source_process, message);

  return handled;
}
