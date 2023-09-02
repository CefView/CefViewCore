#pragma once

#pragma region std_headers
#include <vector>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_base.h>
#include <include/cef_browser.h>
#include <include/cef_scheme.h>
#pragma endregion cef_headers

#include <CefViewCoreProtocol.h>
#include <CefViewBrowserClientDelegate.h>

namespace CefViewDefaultSchemeHandler {
bool
RegisterSchemeHandlerFactory();

bool
RegisterScheme(CefRawPtr<CefSchemeRegistrar> registrar);

class SchemeHandler : public CefResourceHandler
{
public:
  SchemeHandler(CefViewBrowserClientDelegateInterface::RefPtr delegate);

  virtual bool Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback) override;

  virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                  int64_t& response_length,
                                  CefString& redirectUrl) override;

  virtual bool Skip(int64_t bytes_to_skip, int64_t& bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) override;

  virtual bool Read(void* data_out,
                    int bytes_to_read,
                    int& bytes_read,
                    CefRefPtr<CefResourceReadCallback> callback) override;

  virtual void Cancel() override;

private:
  CefViewBrowserClientDelegateInterface::WeakPtr browser_delegate_;
  std::string data_;
  std::string mime_type_;
  int offset_;

private:
  IMPLEMENT_REFCOUNTING(SchemeHandler);
};

class SchemeHandlerFactory : public CefSchemeHandlerFactory
{

  // Return a new scheme handler instance to handle the request.
  virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                               CefRefPtr<CefFrame> frame,
                                               const CefString& scheme_name,
                                               CefRefPtr<CefRequest> request) override;

private:
  IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
};
} // namespace CefViewDefaultSchemeHandler
