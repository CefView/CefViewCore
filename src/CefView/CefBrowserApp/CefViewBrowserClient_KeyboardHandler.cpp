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

CefRefPtr<CefKeyboardHandler>
CefViewBrowserClient::GetKeyboardHandler()
{
  return this;
}

bool
CefViewBrowserClient::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                    const CefKeyEvent& event,
                                    CefEventHandle os_event,
                                    bool* is_keyboard_shortcut)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onPreKeyEvent(browser, event, os_event, is_keyboard_shortcut);

  return false;
}

bool
CefViewBrowserClient::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onKeyEvent(browser, event, os_event);

  return false;
}
