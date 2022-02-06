#include <CefViewBrowserClient.h>

#pragma region std_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_parser.h>
#pragma endregion cef_headers

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefLoadHandler>
CefViewBrowserClient::GetLoadHandler()
{
  return this;
}

void
CefViewBrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                           bool isLoading,
                                           bool canGoBack,
                                           bool canGoForward)
{
  CEF_REQUIRE_UI_THREAD();

  if (!isLoading && initial_navigation_) {
    initial_navigation_ = false;
  }

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadingStateChanged(browser, isLoading, canGoBack, canGoForward);
}

void
CefViewBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  TransitionType transition_type)
{
  CEF_REQUIRE_UI_THREAD();
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadStart(browser);
}

void
CefViewBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
  CEF_REQUIRE_UI_THREAD();
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadEnd(browser, httpStatusCode);
}

void
CefViewBrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  ErrorCode errorCode,
                                  const CefString& errorText,
                                  const CefString& failedUrl)
{
  CEF_REQUIRE_UI_THREAD();
  if (errorCode == ERR_ABORTED)
    return;

  // If the browser is closing, block the popup
  if (is_closing_)
    return;

  auto msg = errorText.ToString();
  auto url = failedUrl.ToString();

  bool handled = false;
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadError(browser, errorCode, msg, url, handled);

  if (handled)
    return;

  std::ostringstream oss;
  oss << "<html><body bgcolor=\"white\">"
      << "<h2>Failed to load URL: " << url << " </h2>"
      << "<h2>Error: " << msg << "(" << errorCode << ")</h2>"
      << "</body></html>";

  std::string data = oss.str();
  data = CefURIEncode(CefBase64Encode(data.c_str(), data.size()), false).ToString();
  data = "data:text/html;base64," + data;
  frame->LoadURL(data);
}
