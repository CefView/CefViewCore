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

bool
CefViewBrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    const CefString& target_url,
                                    const CefString& target_frame_name,
                                    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                                    bool user_gesture,
                                    const CefPopupFeatures& popupFeatures,
                                    CefWindowInfo& windowInfo,
                                    CefRefPtr<CefClient>& client,
                                    CefBrowserSettings& settings,
                                    CefRefPtr<CefDictionaryValue>& extra_info,
                                    bool* no_javascript_access)
{
  CEF_REQUIRE_UI_THREAD();

  // If the browser is closing, block the popup
  if (is_closing_)
    return true;

  // Redirect all popup page into the source frame forcefully
  frame->LoadURL(target_url);
  // Don't allow new window or tab
  return true;
}

void
CefViewBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // If the browser is closing, return immediately to release the new created browser
  if (is_closing_)
    return;

  browser_map_[browser->GetIdentifier()] = browser;
}

bool
CefViewBrowserClient::DoClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void
CefViewBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnBeforeClose(browser);

  browser_map_.erase(browser->GetIdentifier());
}
