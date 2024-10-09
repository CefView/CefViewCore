#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefFocusHandler>
CefViewBrowserClient::GetFocusHandler()
{
  return this;
}

void
CefViewBrowserClient::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->takeFocus(browser, next);
}

bool
CefViewBrowserClient::OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->setFocus(browser);

  return true;
}

void
CefViewBrowserClient::OnGotFocus(CefRefPtr<CefBrowser> browser)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->gotFocus(browser);
}
