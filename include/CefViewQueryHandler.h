//
//  CefViewQueryHandler.h
//  CefView
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewQueryHandler_h
#define CefViewQueryHandler_h
#pragma once

#pragma region std_headers
#include <map>
#include <mutex>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_base.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_stream_resource_handler.h>

#pragma endregion cef_headers

#include <CefViewBrowserClientDelegate.h>

/// <summary>
///
/// </summary>
class CefViewQueryHandler
  : public CefBaseRefCounted
  , public CefMessageRouterBrowserSide::Handler
{
  IMPLEMENT_REFCOUNTING(CefViewQueryHandler);

public:
  CefViewQueryHandler(CefViewBrowserClientDelegateInterface::WeakPtr delegate);

  ~CefViewQueryHandler();

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="query_id"></param>
  /// <param name="request"></param>
  /// <param name="persistent"></param>
  /// <param name="callback"></param>
  /// <returns></returns>
  virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64_t query_id,
                       const CefString& request,
                       bool persistent,
                       CefRefPtr<Callback> callback) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="query_id"></param>
  virtual void OnQueryCanceled(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64_t query_id) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="query"></param>
  /// <param name="success"></param>
  /// <param name="response"></param>
  /// <param name="error"></param>
  /// <returns></returns>
  bool Response(int64_t query, bool success, const CefString& response, int error);

private:
  /// <summary>
  ///
  /// </summary>
  CefViewBrowserClientDelegateInterface::WeakPtr handler_delegate_;

  /// <summary>
  ///
  /// </summary>
  std::map<int64_t, CefRefPtr<Callback>> mapCallback_;

  /// <summary>
  ///
  /// </summary>
  std::mutex mtxCallbackMap_;
};
#endif
