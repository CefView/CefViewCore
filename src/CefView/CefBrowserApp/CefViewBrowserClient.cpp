#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

#include <CefViewCoreProtocol.h>

#include "CefViewQueryHandler/CefViewQueryHandler.h"

CefViewBrowserClient::CefViewBrowserClient(CefRefPtr<CefViewBrowserApp> app,
                                           CefViewBrowserClientDelegateInterface::RefPtr delegate)
  : app_(app)
  , client_delegate_(delegate)
  , is_closing_(false)
  , close_by_native_(false)
  , initial_navigation_(true)
  , message_router_(nullptr)
  , message_router_handler_(nullptr)
  , resource_manager_(new CefResourceManager())
{
  app_->CheckInClient(this, delegate);

  for (auto& folderMapping : app_->FolderResourceMappingList()) {
    AddLocalDirectoryResourceProvider(folderMapping.path, folderMapping.url, folderMapping.priority);
  }

  for (auto& archiveMapping : app_->ArchiveResourceMappingList()) {
    AddArchiveResourceProvider(
      archiveMapping.path, archiveMapping.url, archiveMapping.password, archiveMapping.priority);
  }

  // Create the browser-side router for query handling.
  message_router_config_.js_query_function = kCefViewQueryFuntionName;
  message_router_config_.js_cancel_function = kCefViewQueryCancelFunctionName;
}

CefViewBrowserClient::~CefViewBrowserClient()
{
  log_debug("CefViewBrowserClient::~CefViewBrowserClient()");

  app_->CheckOutClient(this);
}

void
CefViewBrowserClient::CloseAllBrowsers()
{
  close_by_native_ = true;
  auto browsers = browser_map_;
  for (auto& kv : browsers) {
    kv.second->StopLoad();
    kv.second->GetHost()->CloseBrowser(true);
  }
}

void
CefViewBrowserClient::AddLocalDirectoryResourceProvider(const CefString& dir_path,
                                                        const CefString& url,
                                                        int priority /* = 0*/)
{
  if (dir_path.empty() || url.empty())
    return;

  // convert to lower case
  auto lower_url = url.ToString();
  std::transform(
    lower_url.begin(), lower_url.end(), lower_url.begin(), [](unsigned char c) { return std::tolower(c); });

  std::string identifier;
  resource_manager_->AddDirectoryProvider(lower_url, dir_path, priority, identifier);
}

void
CefViewBrowserClient::AddArchiveResourceProvider(const CefString& archive_path,
                                                 const CefString& url,
                                                 const CefString& password,
                                                 int priority /*= 0*/)
{
  if (archive_path.empty() || url.empty())
    return;

  std::string identifier;
  resource_manager_->AddArchiveProvider(url, archive_path, password, 0, identifier);
}

bool
CefViewBrowserClient::TriggerEvent(CefRefPtr<CefBrowser> browser,
                                   const CefFrameId& frame_id,
                                   const CefRefPtr<CefProcessMessage> msg)
{
  if (msg->GetName().empty())
    return false;

  if (browser) {
    std::vector<CefFrameId> frameIds;
    if (frame_id == CEFVIEW_MAIN_FRAME) {
      frameIds.push_back(browser->GetMainFrame()->GetIdentifier());
    } else if (frame_id == CEFVIEW_ALL_FRAMES) {
      browser->GetFrameIdentifiers(frameIds);
    } else {
      frameIds.push_back(frame_id);
    }

    for (auto id : frameIds) {
      auto m = msg->Copy();
#if CEF_VERSION_MAJOR > 121
      auto frame = browser->GetFrameByIdentifier(id);
#else
      auto frame = browser->GetFrame(id);
#endif

      frame->SendProcessMessage(PID_RENDERER, m);
    }

    return true;
  }

  return false;
}

bool
CefViewBrowserClient::ResponseQuery(const int64_t query, bool success, const CefString& response, int error)
{
  if (message_router_handler_)
    return message_router_handler_->Response(query, success, response, error);

  return false;
}

int64_t
CefViewBrowserClient::AsyncExecuteJSCode(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         const CefString& code,
                                         const CefString& url,
                                         const CefString& context)
{
  /*
   * Javascript code:
   *
   * window.__cefview_report_js_result__(context, function() { ... })());
   */
  std::ostringstream codeWrapper;
  codeWrapper << "window." << kCefViewReportJSResultFunctionName //
              << "("                                             //
              << "\"" << context << "\", "                       //
              << "function(){" << code << " }()"                 //
              << ");";

  frame->ExecuteJavaScript(codeWrapper.str().c_str(), url, 0);

  return true;
}

bool
CefViewBrowserClient::DispatchRenderMessage(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefProcessMessage> message)
{
  auto msgName = message->GetName();
  if (msgName == kCefViewClientRenderFocusedNodeChangedMessage) {
    return OnRenderFocusedNodeChangedMessage(browser, frame, message);
  } else if (msgName == kCefViewClientRenderInvokeMethodMessage) {
    return OnRenderInvokeMethodMessage(browser, frame, message);
  } else if (msgName == kCefViewClientRenderReportJSResultMessage) {
    return OnRenderReportJSResultMessage(browser, frame, message);
  } else {
  }

  return false;
}

bool
CefViewBrowserClient::OnRenderFocusedNodeChangedMessage(CefRefPtr<CefBrowser> browser,
                                                        CefRefPtr<CefFrame> frame,
                                                        CefRefPtr<CefProcessMessage> message)
{
  auto delegate = client_delegate_.lock();
  if (!delegate)
    return false;

  // validate the argument list
  CefRefPtr<CefListValue> arguments = message->GetArgumentList();
  if (!arguments || (arguments->GetSize() < 1))
    return false;

  if (CefValueType::VTYPE_BOOL != arguments->GetType(0))
    return false;

  bool focusOnEditableField = message->GetArgumentList()->GetBool(0);
  delegate->focusedEditableNodeChanged(browser, frame, focusOnEditableField);

  return true;
}

bool
CefViewBrowserClient::OnRenderInvokeMethodMessage(CefRefPtr<CefBrowser> browser,
                                                  CefRefPtr<CefFrame> frame,
                                                  CefRefPtr<CefProcessMessage> message)
{
  // validate the argument list
  CefRefPtr<CefListValue> arguments = message->GetArgumentList();
  if (!arguments || (arguments->GetSize() < 1))
    return false;

  // validate the delegate
  auto delegate = client_delegate_.lock();
  if (!delegate)
    return false;

  // copy the arguments
  arguments = arguments->Copy();

  // validate the method name
  std::string method;
  if (CefValueType::VTYPE_STRING != arguments->GetType(0))
    return false;
  method = arguments->GetString(0).ToString();
  if (method.empty())
    return false;
  arguments->Remove(0);

  delegate->invokeMethodNotify(browser, frame, method, arguments);

  return true;
}

bool
CefViewBrowserClient::OnRenderReportJSResultMessage(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefProcessMessage> message)
{
  // validate the argument list
  CefRefPtr<CefListValue> arguments = message->GetArgumentList();
  if (!arguments || (arguments->GetSize() != 2))
    return false;

  // validate the delegate
  auto delegate = client_delegate_.lock();
  if (!delegate)
    return false;

  // get context
  std::string context;
  if (CefValueType::VTYPE_STRING != arguments->GetType(0))
    return false;
  context = arguments->GetString(0).ToString();
  if (context.empty())
    return false;

  // get script result
  auto result = arguments->GetValue(1);

  delegate->reportJSResult(browser, frame, context, result);

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

  if (DispatchRenderMessage(browser, frame, message))
    return true;

  return false;
}
