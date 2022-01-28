//
//  CefViewBrowserHandlerDelegate.h
//  CefView
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserHandlerDelegate_h
#define CefViewBrowserHandlerDelegate_h
#pragma once

#include <memory>
#include <string>

#include <include/cef_client.h>

/// <summary>
///
/// </summary>
class CefViewBrowserClientDelegateInterface
{
public:
  /// <summary>
  ///
  /// </summary>
  typedef std::shared_ptr<CefViewBrowserClientDelegateInterface> RefPtr;

  /// <summary>
  ///
  /// </summary>
  typedef std::weak_ptr<CefViewBrowserClientDelegateInterface> WeakPtr;

  /// <summary>
  ///
  /// </summary>
  virtual ~CefViewBrowserClientDelegateInterface(){};

  /// <summary>
  ///
  /// </summary>
  /// <param name="isLoading"></param>
  /// <param name="canGoBack"></param>
  /// <param name="canGoForward"></param>
  virtual void loadingStateChanged(CefRefPtr<CefBrowser>& browser,
                                   bool isLoading,
                                   bool canGoBack,
                                   bool canGoForward) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void loadStart(CefRefPtr<CefBrowser>& browser) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="httpStatusCode"></param>
  virtual void loadEnd(CefRefPtr<CefBrowser>& browser, int httpStatusCode) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="errorCode"></param>
  /// <param name="errorMsg"></param>
  /// <param name="failedUrl"></param>
  virtual void loadError(CefRefPtr<CefBrowser>& browser,
                         int errorCode,
                         const std::string& errorMsg,
                         const std::string& failedUrl,
                         bool& handled) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="rect"></param>
  /// <param name="draggable"></param>
  virtual void draggableRegionChanged(CefRefPtr<CefBrowser>& browser,
                                      const std::vector<CefDraggableRegion>& regions) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="message"></param>
  /// <param name="level"></param>
  virtual void consoleMessage(CefRefPtr<CefBrowser>& browser, const std::string& message, int level) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="next"></param>
  virtual void takeFocus(CefRefPtr<CefBrowser>& browser, bool next) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <returns></returns>
  virtual bool setFocus(CefRefPtr<CefBrowser>& browser) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  virtual void gotFocus(CefRefPtr<CefBrowser>& browser) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="url"></param>
  virtual void processUrlRequest(const std::string& url) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frameId"></param>
  /// <param name="query"></param>
  /// <param name="query_id"></param>
  virtual void processQueryRequest(CefRefPtr<CefBrowser>& browser,
                                   int frameId,
                                   const std::string& query,
                                   const int64_t query_id) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browserId"></param>
  /// <param name="frameId"></param>
  /// <param name="method"></param>
  /// <param name="arguments"></param>
  virtual void invokeMethodNotify(CefRefPtr<CefBrowser>& browser,
                                  int frameId,
                                  const std::string& method,
                                  const CefRefPtr<CefListValue>& arguments) = 0;

  // Off screen rendering
  virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
  virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY) = 0;
  virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) = 0;
  virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) = 0;
  virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) = 0;
  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                       CefRenderHandler::PaintElementType type,
                       const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height) = 0;
};

#endif
