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
  /// <param name="browser"></param>
  /// <param name="frameId"></param>
  /// <param name="targetUrl"></param>
  /// <param name="targetFrameName"></param>
  /// <param name="targetDisposition"></param>
  /// <param name="windowInfo"></param>
  /// <param name="settings"></param>
  /// <param name="DisableJavascriptAccess"></param>
  /// <returns></returns>
  virtual bool onBeforPopup(CefRefPtr<CefBrowser>& browser,
                            int64_t frameId,
                            const std::string& targetUrl,
                            const std::string& targetFrameName,
                            CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
                            CefWindowInfo& windowInfo,
                            CefBrowserSettings& settings,
                            bool& DisableJavascriptAccess) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  virtual void onAfterCreate(CefRefPtr<CefBrowser>& browser) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual bool doClose(CefRefPtr<CefBrowser> browser) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) = 0;

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
  /// <param name="browser"></param>
  /// <param name="frameId"></param>
  /// <param name="url"></param>
  virtual void addressChanged(CefRefPtr<CefBrowser>& browser, int64_t frameId, const std::string& url) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="title"></param>
  virtual void titleChanged(CefRefPtr<CefBrowser>& browser, const std::string& title) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="text"></param>
  /// <returns></returns>
  virtual bool tooltipMessage(CefRefPtr<CefBrowser>& browser, const std::string& text) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="fullscreen"></param>
  virtual void fullscreenModeChanged(CefRefPtr<CefBrowser>& browser, bool fullscreen) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="value"></param>
  virtual void statusMessage(CefRefPtr<CefBrowser>& browser, const std::string& value) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="progress"></param>
  virtual void loadingProgressChanged(CefRefPtr<CefBrowser>& browser, double progress) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="message"></param>
  /// <param name="level"></param>
  virtual void consoleMessage(CefRefPtr<CefBrowser>& browser, const std::string& message, int level) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="cursor"></param>
  /// <param name="type"></param>
  /// <param name="custom_cursor_info"></param>
  virtual bool cursorChanged(CefRefPtr<CefBrowser> browser,
                             CefCursorHandle cursor,
                             cef_cursor_type_t type,
                             const CefCursorInfo& custom_cursor_info) = 0;

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
                                   int64_t frameId,
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
                                  int64_t frameId,
                                  const std::string& method,
                                  const CefRefPtr<CefListValue>& arguments) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frameId"></param>
  /// <param name="result"></param>
  virtual void reportJSResult(CefRefPtr<CefBrowser>& browser,
                              int64_t frameId,
                              int64_t contextId,
                              const CefRefPtr<CefValue>& result) = 0;

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
  virtual void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                  CefRenderHandler::PaintElementType type,
                                  const CefRenderHandler::RectList& dirtyRects,
                                  void* shared_handle) = 0;
  virtual bool StartDragging(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefDragData> drag_data,
                             CefRenderHandler::DragOperationsMask allowed_ops,
                             int x,
                             int y) = 0;
  virtual void UpdateDragCursor(CefRefPtr<CefBrowser> browser, CefRenderHandler::DragOperation operation) = 0;
  virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y) = 0;
  virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                            const CefRange& selected_range,
                                            const CefRenderHandler::RectList& character_bounds) = 0;
  virtual void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                      const CefString& selected_text,
                                      const CefRange& selected_range) = 0;
  virtual void OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser,
                                          CefRenderHandler::TextInputMode input_mode) = 0;
};

#endif
