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
                                           CefRefPtr<CefCallback> callback)
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
{}

void
CefViewBrowserClient::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefRequest> request,
                                             CefRefPtr<CefResponse> response,
                                             URLRequestStatus status,
                                             int64 received_content_length)
{
    auto delegate = client_delegate_.lock();
    if (delegate)
    {
      delegate->onResourceLoadComplete(browser, frame, request, response, status, received_content_length);
    }
}


CefRefPtr<CefResponseFilter>
CefViewBrowserClient::GetResourceResponseFilter(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefRefPtr<CefRequest> request,
                                                CefRefPtr<CefResponse> response)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
  {
    return delegate->onGetResourceResponseFilter(browser, frame, request, response);
  }

  return nullptr;
}
