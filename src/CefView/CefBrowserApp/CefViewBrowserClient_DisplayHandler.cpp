#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefDisplayHandler>
CefViewBrowserClient::GetDisplayHandler()
{
  return this;
}

void
CefViewBrowserClient::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->addressChanged(browser, frame->GetIdentifier(), url);
}

void
CefViewBrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->titleChanged(browser, title);
}

void
CefViewBrowserClient::OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate) {
    delegate->faviconURLChanged(browser, icon_urls);
  }
}

void
CefViewBrowserClient::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->fullscreenModeChanged(browser, fullscreen);
}

bool
CefViewBrowserClient::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->tooltipMessage(browser, text);

  return false;
}

void
CefViewBrowserClient::OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->statusMessage(browser, value);
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

bool
CefViewBrowserClient::OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size)
{
  CEF_REQUIRE_UI_THREAD();
  return false;
}

void
CefViewBrowserClient::OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadingProgressChanged(browser, progress);
}

bool
CefViewBrowserClient::OnCursorChange(CefRefPtr<CefBrowser> browser,
                                     CefCursorHandle cursor,
                                     cef_cursor_type_t type,
                                     const CefCursorInfo& custom_cursor_info)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->cursorChanged(browser, cursor, type, custom_cursor_info);

  return false;
}
