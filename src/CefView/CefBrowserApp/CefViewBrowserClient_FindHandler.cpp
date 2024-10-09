#include <CefViewBrowserClient.h>

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefFindHandler>
CefViewBrowserClient::GetFindHandler()
{
  return this;
}

void
CefViewBrowserClient::OnFindResult(CefRefPtr<CefBrowser> browser,
                                   int identifier,
                                   int count,
                                   const CefRect& selectionRect,
                                   int activeMatchOrdinal,
                                   bool finalUpdate)
{
  CEF_REQUIRE_UI_THREAD();
}
