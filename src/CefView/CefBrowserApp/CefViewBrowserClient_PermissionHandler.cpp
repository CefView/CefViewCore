#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

#if CEF_VERSION_MAJOR >= 106
CefRefPtr<CefPermissionHandler>
CefViewBrowserClient::GetPermissionHandler()
{
  return this;
}

bool
CefViewBrowserClient::OnRequestMediaAccessPermission(CefRefPtr<CefBrowser> browser,
                                                     CefRefPtr<CefFrame> frame,
                                                     const CefString& requesting_origin,
                                                     uint32_t requested_permissions,
                                                     CefRefPtr<CefMediaAccessCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate) {
    return delegate->onRequestMediaAccessPermission(browser, frame, requesting_origin, requested_permissions, callback);
  }

  return false;
}

bool
CefViewBrowserClient::OnShowPermissionPrompt(CefRefPtr<CefBrowser> browser,
                                             uint64_t prompt_id,
                                             const CefString& requesting_origin,
                                             uint32_t requested_permissions,
                                             CefRefPtr<CefPermissionPromptCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate) {
    return delegate->onShowPermissionPrompt(browser, prompt_id, requesting_origin, requested_permissions, callback);
  }

  return false;
}

void
CefViewBrowserClient::OnDismissPermissionPrompt(CefRefPtr<CefBrowser> browser,
                                                uint64_t prompt_id,
                                                cef_permission_request_result_t result)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onDismissPermissionPrompt(browser, prompt_id, result);
}
#endif
