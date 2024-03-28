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

CefRefPtr<CefRequestHandler>
CefViewBrowserClient::GetRequestHandler()
{
  return this;
}

bool
CefViewBrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefRequest> request,
                                     bool user_gesture,
                                     bool is_redirect)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnBeforeBrowse(browser, frame);
  return false;
}

bool
CefViewBrowserClient::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& target_url,
                                       CefRequestHandler::WindowOpenDisposition target_disposition,
                                       bool user_gesture)
{
  CEF_REQUIRE_UI_THREAD();

  return false; // return true to cancel this navigation.
}

#if CEF_VERSION_MAJOR > 91 && CEF_VERSION_MAJOR < 109
bool
CefViewBrowserClient::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                     const CefString& origin_url,
                                     int64 new_size,
                                     CefRefPtr<CefCallback> callback)
{
  CEF_REQUIRE_IO_THREAD();
  static const int maxSize = 10 * 1024 * 1024;
  if (new_size <= maxSize)
    callback->Continue();
  else
    callback->Cancel();

  return true;
}
#endif

void
CefViewBrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnRenderProcessTerminated(browser);

  if (browser) {
    CefString url = browser->GetMainFrame()->GetURL();
    if (!url.empty()) {
      browser->GetMainFrame()->LoadURL(url);
    }
  }
}
