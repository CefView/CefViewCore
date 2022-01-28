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

void
CefViewBrowserClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefContextMenuParams> params,
                                          CefRefPtr<CefMenuModel> model)
{
  CEF_REQUIRE_UI_THREAD();

  model->Clear();
}

bool
CefViewBrowserClient::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefContextMenuParams> params,
                                           int command_id,
                                           EventFlags event_flags)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}
