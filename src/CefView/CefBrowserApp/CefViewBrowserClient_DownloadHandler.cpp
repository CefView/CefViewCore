#include <CefViewBrowserClient.h>

#pragma region std_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion std_headers

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefDownloadHandler>
CefViewBrowserClient::GetDownloadHandler()
{
  return this;
}

#if CEF_VERSION_MAJOR < 125
void
CefViewBrowserClient::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefDownloadItem> download_item,
                                       const CefString& suggested_name,
                                       CefRefPtr<CefBeforeDownloadCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onBeforeDownload(browser, download_item, suggested_name, callback);
}
#else
bool
CefViewBrowserClient::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefDownloadItem> download_item,
                                       const CefString& suggested_name,
                                       CefRefPtr<CefBeforeDownloadCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onBeforeDownload(browser, download_item, suggested_name, callback);

  return true;
}
#endif

void
CefViewBrowserClient::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefDownloadItem> download_item,
                                        CefRefPtr<CefDownloadItemCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onDownloadUpdated(browser, download_item, callback);
}
