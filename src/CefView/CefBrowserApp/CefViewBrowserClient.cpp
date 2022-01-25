#include <CefViewBrowserClient.h>

#pragma region std_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#include <include/cef_parser.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion cef_headers

#include <CefViewCoreProtocol.h>
#include <Common/CefViewCoreLog.h>

CefViewBrowserClient::CefViewBrowserClient(CefViewBrowserClientDelegateInterface::RefPtr delegate)
  : is_closing_(false)
  , initial_navigation_(false)
  , client_delegate_(delegate)
  , cefquery_handler_(new CefViewQueryHandler(delegate))
  , message_router_(nullptr)
  , resource_manager_(new CefResourceManager())
{
  // Create the browser-side router for query handling.
  message_router_config_.js_query_function = CEFVIEW_QUERY_NAME;
  message_router_config_.js_cancel_function = CEFVIEW_QUERY_CANCEL_NAME;

  // Register handlers with the router.
  message_router_ = CefMessageRouterBrowserSide::Create(message_router_config_);
  message_router_->AddHandler(cefquery_handler_.get(), false);
}

CefViewBrowserClient::~CefViewBrowserClient()
{
  log_debug("CefViewBrowserClient::~CefViewBrowserClient()");

  message_router_->RemoveHandler(cefquery_handler_.get());
  message_router_ = nullptr;
  cefquery_handler_ = nullptr;
}

bool
CefViewBrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefContextMenuParams> params,
                                          CefRefPtr<CefMenuModel> model)
{
  CEF_REQUIRE_UI_THREAD();

  model->Clear();
}

bool
CefViewBrowserClient::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefContextMenuParams> params,
                                           int command_id,
                                           EventFlags event_flags)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

void
CefViewBrowserClient::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
  CEF_REQUIRE_UI_THREAD();
}

void
CefViewBrowserClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
  CEF_REQUIRE_UI_THREAD();
}

bool
CefViewBrowserClient::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                       cef_log_severity_t level,
                                       const CefString& message,
                                       const CefString& source,
                                       int line)
{
  CEF_REQUIRE_UI_THREAD();

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->consoleMessage(browser, message.ToString(), level);

#if (defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG))
  return false;
#else
  return true;
#endif
}

bool
CefViewBrowserClient::OnDragEnter(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser,
                                                CefRefPtr<CefFrame> frame,
                                                const std::vector<CefDraggableRegion>& regions)
{
  CEF_REQUIRE_UI_THREAD();
  NotifyDragRegion(browser, regions);
  return;
}

bool
CefViewBrowserClient::OnJSDialog(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                           const CefString& message_text,
                                           bool is_reload,
                                           CefRefPtr<CefJSDialogCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

void
CefViewBrowserClient::OnResetDialogState(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();
}

void
CefViewBrowserClient::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
{
  CEF_REQUIRE_UI_THREAD();

  NotifyTakeFocus(browser, next);
}

bool
CefViewBrowserClient::OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source)
{
  CEF_REQUIRE_UI_THREAD();

  if (initial_navigation_) {
    return true;
  }

  return false;
}

bool
CefViewBrowserClient::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                    const CefKeyEvent& event,
                                    CefEventHandle os_event,
                                    bool* is_keyboard_shortcut)
{
  CEF_REQUIRE_UI_THREAD();

  return false;
}

bool
CefViewBrowserClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // If the browser is closing, return immediately to release the new created browser
  if (is_closing_)
    return;

  browser_map_[browser->GetIdentifier()] = browser;
}

bool
CefViewBrowserClient::DoClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void
CefViewBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnBeforeClose(browser);

  browser_map_.erase(browser->GetIdentifier());
}

void
CefViewBrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                           bool isLoading,
                                           bool canGoBack,
                                           bool canGoForward)
{
  CEF_REQUIRE_UI_THREAD();

  if (!isLoading && initial_navigation_) {
    initial_navigation_ = false;
  }

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadingStateChanged(browser, isLoading, canGoBack, canGoForward);
}

void
CefViewBrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  TransitionType transition_type)
{
  CEF_REQUIRE_UI_THREAD();
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadStart(browser);
}

void
CefViewBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
  CEF_REQUIRE_UI_THREAD();
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadEnd(browser, httpStatusCode);
}

void
CefViewBrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  ErrorCode errorCode,
                                  const CefString& errorText,
                                  const CefString& failedUrl)
{
  CEF_REQUIRE_UI_THREAD();
  if (errorCode == ERR_ABORTED)
    return;

  // If the browser is closing, block the popup
  if (is_closing_)
    return;

  auto msg = errorText.ToString();
  auto url = failedUrl.ToString();

  bool handled = false;
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->loadError(browser, errorCode, msg, url, handled);

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
CefViewBrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& target_url,
                                       CefRequestHandler::WindowOpenDisposition target_disposition,
                                       bool user_gesture)
{
  CEF_REQUIRE_UI_THREAD();

  return false; // return true to cancel this navigation.
}

CefRefPtr<CefResourceRequestHandler>
CefViewBrowserClient::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
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
CefViewBrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnRenderProcessTerminated(browser);

  if (browser) {
    CefString url = browser->GetMainFrame()->GetURL();
    if (!url.empty()) {
      browser->GetMainFrame()->LoadURL(url);
    }
  }
}

CefResourceRequestHandler::ReturnValue
CefViewBrowserClient::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
                                           CefRefPtr<CefRequestCallback> callback)
{
  auto url = request->GetURL().ToString();
  logD("require resource: %s", url.c_str());
  return resource_manager_->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler>
CefViewBrowserClient::GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefRequest> request)
{
  return resource_manager_->GetResourceHandler(browser, frame, request);
}

void
CefViewBrowserClient::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefRequest> request,
                                          bool& allow_os_execution)
{}

void
CefViewBrowserClient::AddLocalDirectoryResourceProvider(const std::string& dir_path,
                                                        const std::string& url,
                                                        int priority /* = 0*/)
{
  if (dir_path.empty() || url.empty())
    return;

  // convert to lower case
  auto lower_url = url;
  std::transform(
    lower_url.begin(), lower_url.end(), lower_url.begin(), [](unsigned char c) { return std::tolower(c); });

  std::string identifier;
  resource_manager_->AddDirectoryProvider(lower_url, dir_path, priority, identifier);
}

void
CefViewBrowserClient::AddArchiveResourceProvider(const std::string& archive_path,
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
CefViewBrowserClient::CloseAllBrowsers()
{
  for (auto& kv : browser_map_) {
    kv.second->StopLoad();
    kv.second->GetHost()->CloseBrowser(true);
  }
}

int
CefViewBrowserClient::GetBrowserCount()
{
  return static_cast<int>(browser_map_.size());
}

bool
CefViewBrowserClient::TriggerEvent(CefRefPtr<CefBrowser> browser,
                                   const int64_t frame_id,
                                   const CefRefPtr<CefProcessMessage> msg)
{
  if (msg->GetName().empty())
    return false;

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
CefViewBrowserClient::ResponseQuery(const int64_t query, bool success, const CefString& response, int error)
{
  if (cefquery_handler_)
    return cefquery_handler_->Response(query, success, response, error);

  return false;
}

bool
CefViewBrowserClient::DispatchNotifyRequest(CefRefPtr<CefBrowser> browser,
                                            CefProcessId source_process,
                                            CefRefPtr<CefProcessMessage> message)
{
  // validate the browser delegate
  auto delegate = client_delegate_.lock();
  if (!delegate || message->GetName() != INVOKEMETHOD_NOTIFY_MESSAGE)
    return false;

  // validate the argument list
  CefRefPtr<CefListValue> arguments = message->GetArgumentList();
  if (!arguments || (arguments->GetSize() < 2))
    return false;

  arguments = arguments->Copy();

  // validate the frame id
  if (CefValueType::VTYPE_INT != arguments->GetType(0))
    return false;
  int frameId = arguments->GetInt(0);
  arguments->Remove(0);

  // validate the method name
  std::string method;
  if (CefValueType::VTYPE_STRING != arguments->GetType(0))
    return false;
  method = arguments->GetString(0).ToString();
  if (method.empty())
    return false;
  arguments->Remove(0);
  delegate->invokeMethodNotify(browser, frameId, method, arguments);
  return true;
}

void
CefViewBrowserClient::NotifyTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
{
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the main thread.
    CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&CefViewBrowserClient::NotifyTakeFocus, this, browser, next)));
    return;
  }

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->takeFocus(browser, next);
}

void
CefViewBrowserClient::NotifyDragRegion(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion> regions)
{
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute this method on the main thread.
    CefPostTask(TID_UI,
                CefCreateClosureTask(base::Bind(&CefViewBrowserClient::NotifyDragRegion, this, browser, regions)));
    return;
  }

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->draggableRegionChanged(browser, regions);
}
