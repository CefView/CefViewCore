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

CefRefPtr<CefLifeSpanHandler>
CefViewBrowserClient::GetLifeSpanHandler()
{
  return this;
}

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

  bool result = false;

  auto delegate = client_delegate_.lock();
  if (delegate) {
    bool disableJSAccess = no_javascript_access ? *no_javascript_access : false;
    result = delegate->onBeforePopup(browser,
                                     frame->GetIdentifier(),
                                     target_url.ToString(),
                                     target_frame_name.ToString(),
                                     target_disposition,
                                     windowInfo,
                                     settings,
                                     disableJSAccess);
    if (no_javascript_access)
      *no_javascript_access = disableJSAccess;
  }

  return result;
}

void
CefViewBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // If the browser is closing, return immediately to release the new created browser
  if (is_closing_) {
    browser->GetHost()->CloseBrowser(true);
    return;
  }

  // Register handlers with the router.
  if (browser_map_.empty()) {
    message_router_ = CefMessageRouterBrowserSide::Create(message_router_config_);

    cefquery_handler_ = new CefViewQueryHandler(client_delegate_);
    message_router_->AddHandler(cefquery_handler_.get(), false);
  }

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onAfterCreate(browser);

  browser_map_[browser->GetIdentifier()] = browser;
}

bool
CefViewBrowserClient::DoClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  if (!browser->IsPopup()) {
    is_closing_ = true;
  }

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->doClose(browser);

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void
CefViewBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->OnBeforeClose(browser);

  message_router_->OnBeforeClose(browser);

  browser_map_.erase(browser->GetIdentifier());

  if (browser_map_.empty()) {
    message_router_->RemoveHandler(cefquery_handler_.get());
    message_router_ = nullptr;
    cefquery_handler_ = nullptr;
  }
}
