#pragma once

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


class CefViewFaviconDownloadCallback : public CefDownloadImageCallback
{
public:
  explicit CefViewFaviconDownloadCallback(std::shared_ptr<CefViewBrowserClientDelegateInterface> client_handler)
      : client_handler_(client_handler)
    {

    }

  void OnDownloadImageFinished(const CefString& image_url,
                               int http_status_code,
                               CefRefPtr<CefImage> image) override
  {
    if (image)
      client_handler_->faviconChanged(image);
  }

 private:
  std::shared_ptr<CefViewBrowserClientDelegateInterface> client_handler_;

  IMPLEMENT_REFCOUNTING(CefViewFaviconDownloadCallback);
  DISALLOW_COPY_AND_ASSIGN(CefViewFaviconDownloadCallback);
};

