#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefContextMenuHandler>
CefViewBrowserClient::GetContextMenuHandler()
{
  return this;
}

void
CefViewBrowserClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefContextMenuParams> params,
                                          CefRefPtr<CefMenuModel> model)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onBeforeContextMenu(browser, frame, params, model);
}

bool
CefViewBrowserClient::RunContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model,
                                     CefRefPtr<CefRunContextMenuCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onRunContextMenu(browser, frame, params, model, callback);

  return false;
}

bool
CefViewBrowserClient::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefContextMenuParams> params,
                                           int command_id,
                                           EventFlags event_flags)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onContextMenuCommand(browser, frame, params, command_id, event_flags);

  return false;
}

void
CefViewBrowserClient::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onContextMenuDismissed(browser, frame);
}
