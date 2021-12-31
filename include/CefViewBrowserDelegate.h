//
//  CefViewBrowserDelegate.h
//  CefView
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserDelegate_h
#define CefViewBrowserDelegate_h
#pragma once

#include <memory>
#include <string>

class CefViewBrowserDelegateInterface
{
public:
  virtual ~CefViewBrowserDelegateInterface(){};

  /// <summary>
  ///
  /// </summary>
  /// <param name="win"></param>
  virtual void setBrowserWindowId(CefWindowHandle win) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="isLoading"></param>
  /// <param name="canGoBack"></param>
  /// <param name="canGoForward"></param>
  virtual void loadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void loadStart() = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="httpStatusCode"></param>
  virtual void loadEnd(int httpStatusCode) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="errorCode"></param>
  /// <param name="errorMsg"></param>
  /// <param name="failedUrl"></param>
  virtual void loadError(int errorCode, const std::string& errorMsg, const std::string& failedUrl, bool& handled) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="rect"></param>
  /// <param name="draggable"></param>
  virtual void draggableRegionChanged(const std::vector<CefDraggableRegion>& regions) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="message"></param>
  /// <param name="level"></param>
  virtual void consoleMessage(const std::string& message, int level) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="next"></param>
  virtual void takeFocus(bool next) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="url"></param>
  virtual void processUrlRequest(const std::string& url) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="query"></param>
  virtual void processQueryRequest(const std::string& query, const int64_t query_id) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browserId"></param>
  /// <param name="frameId"></param>
  /// <param name="method"></param>
  /// <param name="arguments"></param>
  virtual void invokeMethodNotify(int browserId,
                                  int frameId,
                                  const std::string& method,
                                  const CefRefPtr<CefListValue>& arguments) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void browserIsDestroying() = 0;
};

typedef std::shared_ptr<CefViewBrowserDelegateInterface> CefViewBrowserDelegatePtr;

typedef std::weak_ptr<CefViewBrowserDelegateInterface> CefViewBrowserDelegateWeakPtr;

#endif
