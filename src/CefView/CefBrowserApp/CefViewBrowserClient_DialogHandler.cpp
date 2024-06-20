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

CefRefPtr<CefDialogHandler>
CefViewBrowserClient::GetDialogHandler()
{
  return this;
}

bool
CefViewBrowserClient::OnFileDialog(CefRefPtr<CefBrowser> browser,
                                   FileDialogMode mode,
                                   const CefString& title,
                                   const CefString& default_file_path,
                                   const std::vector<CefString>& accept_filters,
#if CEF_VERSION_MAJOR < 102
                                   int selected_accept_filter,
#endif
                                   CefRefPtr<CefFileDialogCallback> callback)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onFileDialog(browser,
                                  mode,
                                  title,
                                  default_file_path,
                                  accept_filters,
#if CEF_VERSION_MAJOR < 102
                                  selected_accept_filter,
#endif
                                  callback);

  return false;
}
