#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

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
  logScope();

  CEF_REQUIRE_UI_THREAD();

  // [Javascript Context]
  // from now on, you can send Javascript to the frame, but they will not be executed immediately,
  // the script will be executed right after the initialization of V8 context for this frame,
  // that means the scripts you write here will be scheduled.
  frame->ExecuteJavaScript("console.info('[JSRuntime]:frame context is ready')", frame->GetURL(), 0);

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadStart(browser, frame, transition_type);
}

void
CefViewBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
  logScope();

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
