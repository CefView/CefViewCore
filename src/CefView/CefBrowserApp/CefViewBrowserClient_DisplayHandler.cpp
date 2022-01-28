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

void
CefViewBrowserClient::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
  CEF_REQUIRE_UI_THREAD();
}

void
CefViewBrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
  CEF_REQUIRE_UI_THREAD();
}

bool
CefViewBrowserClient::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                       cef_log_severity_t level,
                                       const CefString& message,
                                       const CefString& source,
                                       int line)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->consoleMessage(browser, message.ToString(), level);

#if (defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG))
  return false;
#else
  return true;
#endif
}
