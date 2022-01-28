#include <CefViewBrowserClient.h>

#pragma region std_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion cef_headers

#include <CefViewCoreProtocol.h>
#include <Common/CefViewCoreLog.h>

CefViewBrowserClient::CefViewBrowserClient(CefViewBrowserClientDelegateInterface::RefPtr delegate)
  : is_closing_(false)
  , initial_navigation_(true)
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
