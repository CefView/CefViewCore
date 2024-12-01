#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

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
  if (delegate) {
    delegate->onBeforeDownload(browser, download_item, suggested_name, callback);
  } else {
    // no delegate, just allow this download and then cancel it in OnDownloadUpdated
    if (callback) {
      callback->Continue("", false);
    }
  }

  /// Return true and execute |callback| either asynchronously or in this method to continue or cancel the download.
  /// Return false to proceed with default handling (cancel with Alloy style, download shelf with Chrome style)
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
  if (delegate) {
    delegate->onDownloadUpdated(browser, download_item, callback);
  } else {
    // no delegate, just cancel this downloading
    if (callback) {
      callback->Cancel();
    }
  }
}
