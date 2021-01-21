#include <CefViewBrowserHandler.h>

#pragma region std_headers
#include <sstream>
#include <string>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_parser.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#include <CefViewCoreProtocol.h>
#include <Common/CefViewCoreLog.h>

CefViewBrowserHandler::CefViewBrowserHandler(CefViewBrowserDelegateWeakPtr delegate)
  : browser_delegate_(delegate)
  , browser_count_(0)
  , initial_navigation_(false)
  , is_closing_(false)
  , resource_manager_(new CefResourceManager())
  , message_router_(nullptr)
  , cefquery_handler_(new CefViewQueryHandler(delegate))
{}

CefViewBrowserHandler::~CefViewBrowserHandler() {}

bool
CefViewBrowserHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                CefProcessId source_process,
                                                CefRefPtr<CefProcessMessage> message)
{
  CEF_REQUIRE_UI_THREAD();
  if (message_router_->OnProcessMessageReceived(browser, frame, source_process, message))
    return true;

  if (DispatchNotifyRequest(browser, source_process, message))
    return true;

  return false;
}

void
CefViewBrowserHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefContextMenuParams> params,
                                           CefRefPtr<CefMenuModel> model)
{
  CEF_REQUIRE_UI_THREAD();

  model->Clear();
}

bool
CefViewBrowserHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefContextMenuParams> params,
                                            int command_id,
                                            EventFlags event_flags)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

void
CefViewBrowserHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
  CEF_REQUIRE_UI_THREAD();
}

void
CefViewBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
  CEF_REQUIRE_UI_THREAD();
}

bool
CefViewBrowserHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                        cef_log_severity_t level,
                                        const CefString& message,
                                        const CefString& source,
                                        int line)
{
  CEF_REQUIRE_UI_THREAD();

  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->consoleMessage(message.ToString(), level);

#if (defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG))
  return false;
#else
  return true;
#endif
}

bool
CefViewBrowserHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefDragData> dragData,
                                   CefDragHandler::DragOperationsMask mask)
{
  CEF_REQUIRE_UI_THREAD();

  // Forbid dragging of URLs and files.
  if ((mask & DRAG_OPERATION_LINK) && !dragData->IsFragment()) {
    return true;
  }

  return true;
}

void
CefViewBrowserHandler::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const std::vector<CefDraggableRegion>& regions)
{
  CEF_REQUIRE_UI_THREAD();
  NotifyDragRegion(regions);
  return;
}

bool
CefViewBrowserHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                  const CefString& origin_url,
                                  JSDialogType dialog_type,
                                  const CefString& message_text,
                                  const CefString& default_prompt_text,
                                  CefRefPtr<CefJSDialogCallback> callback,
                                  bool& suppress_message)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

bool
CefViewBrowserHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                            const CefString& message_text,
                                            bool is_reload,
                                            CefRefPtr<CefJSDialogCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

void
CefViewBrowserHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();
}

void
CefViewBrowserHandler::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
{
  CEF_REQUIRE_UI_THREAD();

  NotifyTakeFocus(next);
}

bool
CefViewBrowserHandler::OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source)
{
  CEF_REQUIRE_UI_THREAD();

  if (initial_navigation_) {
    return true;
  }

  return false;
}

bool
CefViewBrowserHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                     const CefKeyEvent& event,
                                     CefEventHandle os_event,
                                     bool* is_keyboard_shortcut)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

bool
CefViewBrowserHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
                                     bool* no_javascript_access)
{
  CEF_REQUIRE_UI_THREAD();

  // If the browser is closing, block the popup
  if (is_closing_)
    return true;

  // Redirect all popup page into the source frame forcefully
  frame->LoadURL(target_url);
  // Don't allow new window or tab
  return true;
}

void
CefViewBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // If the browser is closing, return immediately to release the new created browser
  if (is_closing_)
    return;

  if (!message_router_) {
    // Create the browser-side router for query handling.
    CefMessageRouterConfig config;
    config.js_query_function = CEFVIEW_QUERY_NAME;
    config.js_cancel_function = CEFVIEW_QUERY_CANCEL_NAME;

    // Register handlers with the router.
    message_router_ = CefMessageRouterBrowserSide::Create(config);
    message_router_->AddHandler(cefquery_handler_.get(), false);
  }

  if (!main_browser_) {
    // If this is the main browser then keep it
    {
      std::unique_lock<std::mutex> lock(mtx_);
      // We need to keep the main child window, but not popup windows
      main_browser_ = browser;
    }

    // notify the parent window/view/widget
    auto browserDelegate = browser_delegate_.lock();
    if (browserDelegate)
      browserDelegate->setCefBrowserWindowHandle(main_browser_->GetHost()->GetWindowHandle());
  } else if (browser->IsPopup()) {
    // Add to the list of popup browsers.
    popup_browser_list_.push_back(browser);

    // Give focus to the popup browser. Perform asynchronously because the
    // parent window may attempt to keep focus after launching the popup.
    CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true)));
  }

  // Increase the browser count
  {
    std::unique_lock<std::mutex> lock(mtx_);
    browser_count_++;
  }
}

bool
CefViewBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed description of this
  // process.
  if (main_browser_ && main_browser_->IsSame(browser))
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void
CefViewBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnBeforeClose(browser);

  if (main_browser_ && main_browser_->IsSame(browser)) {
    // if the main browser is closing, we need to close all the pop up browsers.
    if (!popup_browser_list_.empty()) {
      for (auto& popup : popup_browser_list_) {
        if (popup) {
          popup->StopLoad();
          popup->GetHost()->CloseBrowser(true);
        }
      }
    }

    main_browser_->StopLoad();
    main_browser_ = nullptr;
  } else if (browser->IsPopup()) {
    // Remove from the browser popup list.
    for (auto it = popup_browser_list_.begin(); it != popup_browser_list_.end(); ++it) {
      if ((*it)->IsSame(browser)) {
        popup_browser_list_.erase(it);
        break;
      }
    }
  }

  // Decrease the browser count
  {
    std::unique_lock<std::mutex> lock(mtx_);
    if (--browser_count_ == 0) {
      message_router_->RemoveHandler(cefquery_handler_.get());
      cefquery_handler_ = nullptr;
      message_router_ = nullptr;
      auto browserDelegate = browser_delegate_.lock();
      if (browserDelegate)
        browserDelegate->browserIsDestroying();
    }
  }
}

void
CefViewBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                            bool isLoading,
                                            bool canGoBack,
                                            bool canGoForward)
{
  CEF_REQUIRE_UI_THREAD();

  if (!isLoading && initial_navigation_) {
    initial_navigation_ = false;
  }

  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->loadingStateChanged(isLoading, canGoBack, canGoForward);
}

void
CefViewBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   TransitionType transition_type)
{
  CEF_REQUIRE_UI_THREAD();
  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->loadStart();
}

void
CefViewBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
  CEF_REQUIRE_UI_THREAD();
  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->loadEnd(httpStatusCode);
}

void
CefViewBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   ErrorCode errorCode,
                                   const CefString& errorText,
                                   const CefString& failedUrl)
{
  CEF_REQUIRE_UI_THREAD();
  if (errorCode == ERR_ABORTED)
    return;

  auto msg = errorText.ToString();
  auto url = failedUrl.ToString();

  bool handled = false;
  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->loadError(errorCode, msg, url, handled);

  if (handled)
    return;

  std::ostringstream oss;
  oss << "<html><body bgcolor=\"white\">"
      << "<h2>Failed to load URL: " << url << " </h2>"
      << "<h2>Error: " << msg << "(" << errorCode << ")</h2>"
      << "</body></html>";

  std::string data = oss.str();
  data = CefURIEncode(CefBase64Encode(data.c_str(), data.size()), false).ToString();
  data = "data:text/html;base64," + data;
  frame->LoadURL(data);
}

bool
CefViewBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefRequest> request,
                                      bool user_gesture,
                                      bool is_redirect)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnBeforeBrowse(browser, frame);
  return false;
}

bool
CefViewBrowserHandler::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        const CefString& target_url,
                                        CefRequestHandler::WindowOpenDisposition target_disposition,
                                        bool user_gesture)
{
  CEF_REQUIRE_UI_THREAD();

  return false; // return true to cancel this navigation.
}

CefRefPtr<CefResourceRequestHandler>
CefViewBrowserHandler::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 CefRefPtr<CefRequest> request,
                                                 bool is_navigation,
                                                 bool is_download,
                                                 const CefString& request_initiator,
                                                 bool& disable_default_handling)
{
  CEF_REQUIRE_IO_THREAD();
  return this;
}

bool
CefViewBrowserHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                      const CefString& origin_url,
                                      int64 new_size,
                                      CefRefPtr<CefRequestCallback> callback)
{
  CEF_REQUIRE_IO_THREAD();
  static const int maxSize = 10 * 1024 * 1024;
  callback->Continue(new_size <= maxSize);
  return true;
}

void
CefViewBrowserHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnRenderProcessTerminated(browser);

  if (main_browser_) {
    CefString url = main_browser_->GetMainFrame()->GetURL();
    if (!url.empty()) {
      main_browser_->GetMainFrame()->LoadURL(url);
    }
  }
}

CefResourceRequestHandler::ReturnValue
CefViewBrowserHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefRequest> request,
                                            CefRefPtr<CefRequestCallback> callback)
{
  auto url = request->GetURL().ToString();
  logD("require resouce: %s", url.c_str());
  return resource_manager_->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler>
CefViewBrowserHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefRequest> request)
{
  return resource_manager_->GetResourceHandler(browser, frame, request);
}

void
CefViewBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
                                           bool& allow_os_execution)
{}

CefRefPtr<CefBrowser>
CefViewBrowserHandler::GetBrowser() const
{
  std::unique_lock<std::mutex> lock(mtx_);
  return main_browser_;
}

void
CefViewBrowserHandler::AddLocalDirectoryResourceProvider(const std::string& dir_path,
                                                         const std::string& url,
                                                         int priority /* = 0*/)
{
  if (dir_path.empty() || url.empty())
    return;

  std::string identifier;
  resource_manager_->AddDirectoryProvider(url, dir_path, priority, identifier);
}

void
CefViewBrowserHandler::AddArchiveResourceProvider(const std::string& archive_path,
                                                  const std::string& url,
                                                  const std::string& password,
                                                  int priority /*= 0*/)
{
  if (archive_path.empty() || url.empty())
    return;

  std::string identifier;
  resource_manager_->AddArchiveProvider(url, archive_path, password, 0, identifier);
}

void
CefViewBrowserHandler::CloseAllBrowsers(bool force_close)
{
  // If all browsers had been closed, then return
  std::unique_lock<std::mutex> lock(mtx_);
  if (!browser_count_) {
    return;
  }

  // Flip the closing flag
  is_closing_ = true;

  // Close all popup browsers if any
  if (!popup_browser_list_.empty()) {
    for (auto it = popup_browser_list_.begin(); it != popup_browser_list_.end(); ++it) {
      if (!(*it))
        continue;
      auto host = (*it)->GetHost();
      if (!host)
        continue;
      host->CloseBrowser(force_close);
    }
  }

  if (main_browser_) {
    // Request that the main browser close.
    auto host = main_browser_->GetHost();
    if (host)
      main_browser_->GetHost()->CloseBrowser(force_close);
  }
}

bool
CefViewBrowserHandler::TriggerEvent(const int64_t frame_id, const CefRefPtr<CefProcessMessage> msg)
{
  if (msg->GetName().empty())
    return false;

  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser) {
    std::vector<int64> frameIds;
    if (MAIN_FRAME == frame_id) {
      frameIds.push_back(browser->GetMainFrame()->GetIdentifier());
    } else if (ALL_FRAMES == frame_id) {
      browser->GetFrameIdentifiers(frameIds);
    } else {
      frameIds.push_back(frame_id);
    }

    for (auto id : frameIds) {
      auto frame = browser->GetFrame(id);
      frame->SendProcessMessage(PID_RENDERER, msg);
      return true;
    }
  }

  return false;
}

bool
CefViewBrowserHandler::ResponseQuery(const int64_t query, bool success, const CefString& response, int error)
{
  if (cefquery_handler_)
    return cefquery_handler_->Response(query, success, response, error);

  return false;
}

bool
CefViewBrowserHandler::DispatchNotifyRequest(CefRefPtr<CefBrowser> browser,
                                             CefProcessId source_process,
                                             CefRefPtr<CefProcessMessage> message)
{
  // validate the browser delegate
  auto browserDelegate = browser_delegate_.lock();
  if (!browserDelegate || message->GetName() != INVOKEMETHOD_NOTIFY_MESSAGE)
    return false;

  // validate the argument list
  CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();
  if (!messageArguments || (messageArguments->GetSize() < 2))
    return false;

  int browserId = browser->GetIdentifier();
  messageArguments = messageArguments->Copy();

  // validate the frame id
  if (CefValueType::VTYPE_INT != messageArguments->GetType(0))
    return false;
  int frameId = messageArguments->GetInt(0);
  messageArguments->Remove(0);

  // validate the function name
  if (CefValueType::VTYPE_STRING != messageArguments->GetType(0))
    return false;
  CefString functionName = messageArguments->GetString(0);
  if (functionName != CEFVIEW_INVOKEMETHOD)
    return false;
  messageArguments->Remove(0);

  // validate the method name
  std::string method;
  if (CefValueType::VTYPE_STRING != messageArguments->GetType(0))
    return false;
  method = messageArguments->GetString(0).ToString();
  if (method.empty())
    return false;
  messageArguments->Remove(0);
  browserDelegate->invokeMethodNotify(browserId, frameId, method, messageArguments);
  return true;
}

void
CefViewBrowserHandler::NotifyTakeFocus(bool next)
{
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the main thread.
    CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&CefViewBrowserHandler::NotifyTakeFocus, this, next)));
    return;
  }

  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->takeFocus(next);
}

void
CefViewBrowserHandler::NotifyDragRegion(const std::vector<CefDraggableRegion> regions)
{
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the main thread.
    CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&CefViewBrowserHandler::NotifyDragRegion, this, regions)));
    return;
  }

  auto browserDelegate = browser_delegate_.lock();
  if (browserDelegate)
    browserDelegate->draggableRegionChanged(regions);
}
