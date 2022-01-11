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
  /// <param name="url"></param>
  virtual void processUrlRequest(const std::string& url) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="query"></param>
  virtual void processQueryRequest(CefRefPtr<CefBrowser>& browser,
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

  /// <summary>
  ///
  /// </summary>
  virtual void browserIsDestroying() = 0;
};

#endif
