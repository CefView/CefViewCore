#include <CefViewBrowserClient.h>

#pragma region std_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion cef_headers

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefJSDialogHandler>
CefViewBrowserClient::GetJSDialogHandler()
{
  return this;
}

bool
CefViewBrowserClient::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                 const CefString& origin_url,
                                 JSDialogType dialog_type,
                                 const CefString& message_text,
                                 const CefString& default_prompt_text,
                                 CefRefPtr<CefJSDialogCallback> callback,
                                 bool& suppress_message)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

bool
CefViewBrowserClient::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                           const CefString& message_text,
                                           bool is_reload,
                                           CefRefPtr<CefJSDialogCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

void
CefViewBrowserClient::OnResetDialogState(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();
}
