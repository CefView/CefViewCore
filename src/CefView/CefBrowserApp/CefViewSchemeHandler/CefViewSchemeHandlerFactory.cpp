#include "CefViewSchemeHandlerFactory.h"

#include <CefViewBrowserClient.h>
#include <CefViewBrowserClientDelegate.h>

#include "CefViewSchemeHandler.h"

CefViewSchemeHandlerFactory::CefViewSchemeHandlerFactory(CefRefPtr<CefViewBrowserApp> app)
  : app_(app)
{
}

CefViewSchemeHandlerFactory::~CefViewSchemeHandlerFactory() {}

CefRefPtr<CefResourceHandler>
CefViewSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    const CefString& scheme_name,
                                    CefRefPtr<CefRequest> request)
{
  if (!app_) {
    return nullptr;
  }

  auto client = browser->GetHost()->GetClient();
  if (!client) {
    return nullptr;
  }

  auto handler = app_->GetClientHandler(client.get());
  if (!handler) {
    return nullptr;
  }
  return new CefViewSchemeHandler(browser, frame, handler);
}
