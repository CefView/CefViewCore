//
//  CefViewBrowserHandler.h
//  CefView
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserHandler_h
#define CefViewBrowserHandler_h
#pragma once

#pragma region std_headers
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_version.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#pragma endregion cef_headers

#include <CefViewBrowserApp.h>
#include <CefViewBrowserClientDelegate.h>
#include <CefViewQueryHandler.h>

class CefViewBrowserClient
  : public CefClient
  , public CefContextMenuHandler
  , public CefDialogHandler
  , public CefDisplayHandler
  , public CefDownloadHandler
  , public CefDragHandler
  , public CefFocusHandler
  , public CefJSDialogHandler
  , public CefKeyboardHandler
  , public CefLifeSpanHandler
  , public CefLoadHandler
  , public CefRequestHandler
  , public CefResourceRequestHandler
  , public CefRenderHandler
{
  IMPLEMENT_REFCOUNTING(CefViewBrowserClient);

private:
  bool is_closing_;
  bool close_by_native_;
  bool initial_navigation_;

  std::unordered_map<int, CefRefPtr<CefBrowser>> browser_map_;

  CefRefPtr<CefViewBrowserApp> app_;
  CefViewBrowserClientDelegateInterface::WeakPtr client_delegate_;

  // message router
  CefMessageRouterConfig message_router_config_;
  CefRefPtr<CefViewQueryHandler> cefquery_handler_;
  CefRefPtr<CefMessageRouterBrowserSide> message_router_;

  // resource manager
  CefRefPtr<CefResourceManager> resource_manager_;

public:
  /// <summary>
  ///
  /// </summary>
  enum
  {
    MAIN_FRAME = (int64_t)0,
    ALL_FRAMES = (int64_t)-1,
  };

  /// <summary>
  ///
  /// </summary>
  /// <param name="app_"></param>
  /// <param name="delegate"></param>
  CefViewBrowserClient(CefRefPtr<CefViewBrowserApp> app, CefViewBrowserClientDelegateInterface::RefPtr delegate);

  /// <summary>
  ///
  /// </summary>
  ~CefViewBrowserClient();

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  int GetBrowserCount() { return static_cast<int>(browser_map_.size()); }

  /// <summary>
  ///
  /// </summary>
  void CloseAllBrowsers();

  /// <summary>
  ///
  /// </summary>
  /// <param name="dir_path"></param>
  /// <param name="url"></param>
  /// <param name="priority"></param>
  void AddLocalDirectoryResourceProvider(const std::string& dir_path, const std::string& url, int priority = 0);

  /// <summary>
  ///
  /// </summary>
  /// <param name="archive_path"></param>
  /// <param name="url"></param>
  /// <param name="password"></param>
  /// <param name="priority"></param>
  void AddArchiveResourceProvider(const std::string& archive_path,
                                  const std::string& url,
                                  const std::string& password,
                                  int priority = 0);

  bool TriggerEvent(CefRefPtr<CefBrowser> browser, const int64_t frame_id, const CefRefPtr<CefProcessMessage> msg);

  /// <summary>
  ///
  /// </summary>
  /// <param name="query"></param>
  /// <param name="success"></param>
  /// <param name="response"></param>
  /// <param name="error"></param>
  /// <returns></returns>
  bool ResponseQuery(const int64_t query, bool success, const CefString& response, int error);

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="code"></param>
  /// <param name="url"></param>
  /// <param name="context"></param>
  /// <returns></returns>
  int64_t AsyncExecuteJSCode(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& code,
                             const CefString& url,
                             const CefString& context);

protected:
  bool DispatchRenderMessage(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefProcessMessage> message);

  bool OnRenderFocusedNodeChangedMessage(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefProcessMessage> message);

  bool OnRenderInvokeMethodMessage(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefProcessMessage> message);

  bool OnRenderReportJSResultMessage(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefProcessMessage> message);

  // CefClient methods:
#pragma region CefContextMenuHandler
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;
#pragma endregion

  // CefContextMenuHandler methods
#pragma region CefContextMenuHandler
  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) override;
  virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params,
                              CefRefPtr<CefMenuModel> model,
                              CefRefPtr<CefRunContextMenuCallback> callback) override;
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) override;
  virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;
#pragma endregion

  // CefDialogHandler
#pragma region CefDialogHandler
  virtual CefRefPtr<CefDialogHandler> GetDialogHandler() override;
  virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                            FileDialogMode mode,
                            const CefString& title,
                            const CefString& default_file_path,
                            const std::vector<CefString>& accept_filters,
                            CefRefPtr<CefFileDialogCallback> callback) override;
#pragma endregion

  // CefDisplayHandler methods
#pragma region CefDisplayHandler
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) override;
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;
  virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) override;
  virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;
  virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) override;
  virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) override;
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                cef_log_severity_t level,
                                const CefString& message,
                                const CefString& source,
                                int line) override;
  virtual bool OnAutoResize(CefRefPtr<CefBrowser> browser, const CefSize& new_size) override;
  virtual void OnLoadingProgressChange(CefRefPtr<CefBrowser> browser, double progress) override;
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                              CefCursorHandle cursor,
                              cef_cursor_type_t type,
                              const CefCursorInfo& custom_cursor_info) override;
#pragma endregion

  // CefDownloadHandler
#pragma region CefDownloadHandler
  virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
  void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefDownloadItem> download_item,
                        const CefString& suggested_name,
                        CefRefPtr<CefBeforeDownloadCallback> callback) override;

  void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefDownloadItem> download_item,
                         CefRefPtr<CefDownloadItemCallback> callback) override;
#pragma endregion

  // CefDragHandler methods
#pragma region CefDragHandler
  virtual CefRefPtr<CefDragHandler> GetDragHandler() override;
  virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDragData> dragData,
                           CefDragHandler::DragOperationsMask mask) override;

  virtual void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         const std::vector<CefDraggableRegion>& regions) override;
#pragma endregion

  // CefFocusHandler methods
#pragma region CefFocusHandler
  virtual CefRefPtr<CefFocusHandler> GetFocusHandler() override;
  void OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) override;
  bool OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source) override;
  void OnGotFocus(CefRefPtr<CefBrowser> browser) override;
#pragma endregion

  // CefJSDialogHandler methods
#pragma region CefJSDialogHandler
  virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
  virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                          const CefString& origin_url,
                          JSDialogType dialog_type,
                          const CefString& message_text,
                          const CefString& default_prompt_text,
                          CefRefPtr<CefJSDialogCallback> callback,
                          bool& suppress_message) override;

  virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                    const CefString& message_text,
                                    bool is_reload,
                                    CefRefPtr<CefJSDialogCallback> callback) override;
  virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) override;
#pragma endregion

  // CefKeyboardHandler methods
#pragma region CefKeyboardHandler
  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
  virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                             const CefKeyEvent& event,
                             CefEventHandle os_event,
                             bool* is_keyboard_shortcut) override;
  virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event) override;
#pragma endregion

  // CefLifeSpanHandler methods:
#pragma region CefLifeSpanHandler
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                             bool user_gesture,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             CefRefPtr<CefDictionaryValue>& extra_info,
                             bool* no_javascript_access) override;
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
#pragma endregion

  // CefLoadHandler methods
#pragma region CefLoadHandler
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward) override;
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) override;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) override;
#pragma endregion

  // CefRenderHandler
#pragma region CefRenderHandler
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override;
  virtual CefRefPtr<CefAccessibilityHandler> GetAccessibilityHandler() override;
  virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY) override;
  virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;
  virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
  virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                       PaintElementType type,
                       const RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height) override;
  virtual void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                  PaintElementType type,
                                  const RectList& dirtyRects,
                                  void* shared_handle) override;
  virtual bool StartDragging(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefDragData> drag_data,
                             CefRenderHandler::DragOperationsMask allowed_ops,
                             int x,
                             int y) override;
  virtual void UpdateDragCursor(CefRefPtr<CefBrowser> browser, DragOperation operation) override;
  virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y) override;
  virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                            const CefRange& selected_range,
                                            const RectList& character_bounds) override;
  virtual void OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                      const CefString& selected_text,
                                      const CefRange& selected_range) override;
  virtual void OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, TextInputMode input_mode) override;
#pragma endregion

  // CefRequestHandler methods
#pragma region CefRequestHandler
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override;
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              bool user_gesture,
                              bool is_redirect) override;

  virtual bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                const CefString& target_url,
                                CefRequestHandler::WindowOpenDisposition target_disposition,
                                bool user_gesture) override;
#if CEF_VERSION_MAJOR > 91 && CEF_VERSION_MAJOR < 109
  virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                              const CefString& origin_url,
                              int64 new_size,
                              CefRefPtr<CefCallback> callback) override;
#endif

  virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status) override;
#pragma endregion

  // CefResourceRequestHandler
#pragma region CefResourceRequestHandler
  virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
                                                                         CefRefPtr<CefFrame> frame,
                                                                         CefRefPtr<CefRequest> request,
                                                                         bool is_navigation,
                                                                         bool is_download,
                                                                         const CefString& request_initiator,
                                                                         bool& disable_default_handling) override;

  virtual ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
#if CEF_VERSION_MAJOR > 91
                                           CefRefPtr<CefCallback> callback
#else
                                           CefRefPtr<CefRequestCallback> callback
#endif
                                           ) override;

  virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                                           CefRefPtr<CefFrame> frame,
                                                           CefRefPtr<CefRequest> request) override;

  virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   bool& allow_os_execution) override;
#pragma endregion
};
#endif
