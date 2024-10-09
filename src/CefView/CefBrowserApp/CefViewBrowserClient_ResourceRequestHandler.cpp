#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefResourceRequestHandler>
CefViewBrowserClient::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefRefPtr<CefRequest> request,
                                                bool is_navigation,
                                                bool is_download,
                                                const CefString& request_initiator,
                                                bool& disable_default_handling)
{
  return this;
}

CefResourceRequestHandler::ReturnValue
CefViewBrowserClient::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
#if CEF_VERSION_MAJOR > 91
                                           CefRefPtr<CefCallback> callback
#else
                                           CefRefPtr<CefRequestCallback> callback
#endif
)
{
  auto url = request->GetURL().ToString();
  // logD("require resource: %s", url.c_str());
  return resource_manager_->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler>
CefViewBrowserClient::GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefRequest> request)
{
  return resource_manager_->GetResourceHandler(browser, frame, request);
}

void
CefViewBrowserClient::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefRequest> request,
                                          bool& allow_os_execution)
{
}
