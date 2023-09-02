#include "CefViewDefaultSchemeHandler.h"

#pragma region std_headers
#include <algorithm>
#include <string>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_browser.h>
#include <include/cef_callback.h>
#include <include/cef_frame.h>
#include <include/cef_request.h>
#include <include/cef_resource_handler.h>
#include <include/cef_response.h>
#include <include/cef_scheme.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#include <CefViewCoreProtocol.h>

namespace CefViewDefaultSchemeHandler {
//////////////////////////////////////////////////////////////////////////
// handler
SchemeHandler::SchemeHandler(CefViewBrowserClientDelegateInterface::RefPtr delegate)
  : browser_delegate_(delegate)
  , offset_(0)
{}

bool
SchemeHandler::Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback)
{
  handle_request = true;

  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate) {
    CefString cefStrUrl = request->GetURL();
    browserDelegate->processUrlRequest(cefStrUrl.ToString());
  }

  data_ = "ok";
  mime_type_ = "text/html";

  return true;
}

void
SchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64_t& response_length, CefString& redirectUrl)
{
  CEF_REQUIRE_IO_THREAD();
  DCHECK(!data_.empty());
  response->SetMimeType(mime_type_);
  response->SetStatus(200);
  // Set the resulting response length
  response_length = data_.length();
}

bool
SchemeHandler::Skip(int64_t bytes_to_skip, int64_t& bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback)
{
  bytes_skipped = 0;

  offset_ += bytes_to_skip;
  if (offset_ <= (int)data_.length()) {
    bytes_skipped = bytes_to_skip;
  } else {
    offset_ = (int)data_.length();
    bytes_skipped = data_.length() - offset_;
  }

  return true;
}

bool
SchemeHandler::Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback)
{
  bytes_read = 0;
  if (offset_ < (int)data_.length()) {
    // Copy the next block of data into the buffer.
    int transfer_size = std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
    memcpy(data_out, data_.c_str() + offset_, transfer_size);
    offset_ += transfer_size;
    bytes_read = transfer_size;
  }

  return bytes_read > 0;
}

void
SchemeHandler::Cancel()
{}

//////////////////////////////////////////////////////////////////////////
// handler factory
//

CefRefPtr<CefResourceHandler>
SchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& scheme_name,
                             CefRefPtr<CefRequest> request)
{
  CefViewBrowserClientDelegateInterface::RefPtr browserDelegate;
  return new SchemeHandler(browserDelegate);
}

//////////////////////////////////////////////////////////////////////////
bool
RegisterScheme(CefRawPtr<CefSchemeRegistrar> registrar)
{
  // register default scheme_name
  return registrar->AddCustomScheme(kCefViewSchema, 0);
}

bool
RegisterSchemeHandlerFactory()
{
  return CefRegisterSchemeHandlerFactory(kCefViewSchema, "", new SchemeHandlerFactory());
}
} // namespace CefViewDefaultSchemeHandler
