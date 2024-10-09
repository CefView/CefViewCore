#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefDialogHandler>
CefViewBrowserClient::GetDialogHandler()
{
  return this;
}

#if CEF_VERSION_MAJOR < 102
bool
CefViewBrowserClient::OnFileDialog(CefRefPtr<CefBrowser> browser,
                                   FileDialogMode mode,
                                   const CefString& title,
                                   const CefString& default_file_path,
                                   const std::vector<CefString>& accept_filters,
                                   int selected_accept_filter,
                                   CefRefPtr<CefFileDialogCallback> callback)
#elif CEF_VERSION_MAJOR < 126
bool
CefViewBrowserClient::OnFileDialog(CefRefPtr<CefBrowser> browser,
                                   FileDialogMode mode,
                                   const CefString& title,
                                   const CefString& default_file_path,
                                   const std::vector<CefString>& accept_filters,
                                   CefRefPtr<CefFileDialogCallback> callback)
#else
bool
CefViewBrowserClient::OnFileDialog(CefRefPtr<CefBrowser> browser,
                                   FileDialogMode mode,
                                   const CefString& title,
                                   const CefString& default_file_path,
                                   const std::vector<CefString>& accept_filters,
                                   const std::vector<CefString>& accept_extensions,
                                   const std::vector<CefString>& accept_descriptions,
                                   CefRefPtr<CefFileDialogCallback> callback)
#endif
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
