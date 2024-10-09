#include "CefViewQueryHandler.h"

CefViewQueryHandler::CefViewQueryHandler(CefViewBrowserClientDelegateInterface::WeakPtr delegate)
  : handler_delegate_(delegate)
{
}

CefViewQueryHandler::~CefViewQueryHandler() {}

bool
CefViewQueryHandler::Response(int64_t query, bool success, const CefString& response, int error)
{
  CefRefPtr<Callback> cb;
  mtxCallbackMap_.lock();
  auto it = mapCallback_.find(query);
  if (it != mapCallback_.end()) {
    cb = it->second;
    mapCallback_.erase(it);
  }
  mtxCallbackMap_.unlock();

  if (!cb)
    return false;

  if (success)
    cb->Success(response);
  else
    cb->Failure(error, response);

  return true;
}

bool
CefViewQueryHandler::OnQuery(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             int64_t query_id,
                             const CefString& request,
                             bool persistent,
                             CefRefPtr<Callback> callback)
{
  auto browserDelegate = handler_delegate_.lock();
  if (browserDelegate) {
    mtxCallbackMap_.lock();
    mapCallback_[query_id] = callback;
    mtxCallbackMap_.unlock();

    browserDelegate->processQueryRequest(browser, frame->GetIdentifier(), request.ToString(), query_id);

    return true;
  }

  return false;
}

void
CefViewQueryHandler::OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id)
{
  mtxCallbackMap_.lock();
  auto it = mapCallback_.find(query_id);
  if (it != mapCallback_.end())
    mapCallback_.erase(it);

  mtxCallbackMap_.unlock();
}
