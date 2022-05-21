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
  /// <param name="browser"></param>
  /// <param name="frameId"></param>
  /// <param name="focusOnEditableNode"></param>
  virtual void focusedEditableNodeChanged(CefRefPtr<CefBrowser>& browser,
                                          int64_t frameId,
                                          bool focusOnEditableNode) = 0;

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

  virtual void onBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) = 0;
  virtual bool onRunContextMenu(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefContextMenuParams> params,
                                CefRefPtr<CefMenuModel> model,
                                CefRefPtr<CefRunContextMenuCallback> callback) = 0;
  virtual bool onContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    CefContextMenuHandler::EventFlags event_flags) = 0;
  virtual void onContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) = 0;

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
  virtual bool onBeforePopup(CefRefPtr<CefBrowser>& browser,
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
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="transition_type"></param>
  virtual void loadStart(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefFrame>& frame, int transition_type) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="httpStatusCode"></param>
  virtual void loadEnd(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefFrame>& frame, int httpStatusCode) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="errorCode"></param>
  /// <param name="errorMsg"></param>
  /// <param name="failedUrl"></param>
  /// <param name="handled"></param>
  virtual void loadError(CefRefPtr<CefBrowser>& browser,
                         CefRefPtr<CefFrame>& frame,
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

  virtual bool onPreKeyEvent(CefRefPtr<CefBrowser> browser,
                             const CefKeyEvent& event,
                             CefEventHandle os_event,
                             bool* is_keyboard_shortcut) = 0;

  virtual bool onKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event) = 0;

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

  // Off screen rendering
  virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) { return false; };
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect){};
  virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
  {
    return false;
  };
  virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) { return false; };
  virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show){};
  virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect){};
  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                       CefRenderHandler::PaintElementType type,
                       const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height){};
  virtual void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                  CefRenderHandler::PaintElementType type,
                                  const CefRenderHandler::RectList& dirtyRects,
                                  void* shared_handle){};
  virtual bool StartDragging(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefDragData> drag_data,
                             CefRenderHandler::DragOperationsMask allowed_ops,
                             int x,
                             int y)
  {
    return false;
  };
  virtual void UpdateDragCursor(CefRefPtr<CefBrowser> browser, CefRenderHandler::DragOperation operation){};
  virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y){};
  virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                            const CefRange& selected_range,
                                            const CefRenderHandler::RectList& character_bounds){};
  virtual void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                      const CefString& selected_text,
                                      const CefRange& selected_range){};
  virtual void OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, CefRenderHandler::TextInputMode input_mode){};
};

#endif
