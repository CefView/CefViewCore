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
#include <Common/CefViewDebug.h>

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
  logD("CefViewBrowserClient::OnLoadingStateChange, browser=%s, isLoading=%d, canGoBack=%d, canGoForward=%d",
                      toString(browser).c_str(), isLoading, canGoBack, canGoForward);

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
  logD("CefViewBrowserClient::OnLoadStart, browser=%s, frame=%s, transition_type=%d",
                    toString(browser).c_str(), toString(frame).c_str(), (int)transition_type);

  CEF_REQUIRE_UI_THREAD();
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadStart(browser, frame, (int)transition_type);
}

void
CefViewBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
  logD("CefViewBrowserClient::OnLoadEnd, browser=%s, frame=%s, httpStatusCode=%d",
                    toString(browser).c_str(), toString(frame).c_str(), httpStatusCode);

  CEF_REQUIRE_UI_THREAD();
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadEnd(browser, frame, httpStatusCode);
}

void
CefViewBrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  ErrorCode errorCode,
                                  const CefString& errorText,
                                  const CefString& failedUrl)
{
  logD("CefViewBrowserClient::OnLoadError, browser=%s, frame=%s, errorCode=%d, errorText=%s, failedUrl=%s",
                    toString(browser).c_str(), toString(frame).c_str(), errorCode, errorText.c_str(), failedUrl.c_str());

  CEF_REQUIRE_UI_THREAD();
  if (errorCode == ERR_ABORTED)
    return;

  // If the browser is closing, block the popup
  if (is_closing_)
    return;

  auto msg = errorText.ToString();
  auto url = failedUrl.ToString();

  //handled的值能不能传递进来，取决于最外层的信号槽的连接方式。
  bool handled = false;
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadError(browser, frame, errorCode, msg, url, handled);

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
