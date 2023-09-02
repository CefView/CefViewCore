#pragma region project_headers
#include "Common/CefViewCoreLog.h"
#include "CefViewDefaultRenderDelegate.h"
#include "CefViewClient.h"
#include "CefViewCoreProtocol.h"
#pragma endregion project_headers

namespace CefViewDefaultRenderDelegate {
void
CreateBrowserDelegate(CefViewRenderApp::RenderDelegateSet& delegates, const std::string& bridge_name)
{
  delegates.insert(new RenderDelegate(bridge_name));
}

RenderDelegate::RenderDelegate(const std::string& bridge_name)
  : bridge_object_name_(bridge_name)
{
}

void
RenderDelegate::OnWebKitInitialized(CefRefPtr<CefViewRenderApp> app)
{
  CefMessageRouterConfig config;
  config.js_query_function = kCefViewQueryFuntionName;
  config.js_cancel_function = kCefViewQueryCancelFunctionName;
  render_message_router_ = CefMessageRouterRendererSide::Create(config);
}

void
RenderDelegate::OnContextCreated(CefRefPtr<CefViewRenderApp> app,
                                 CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context)
{
  // [Javascript Context]
  // V8 context for this frame has been initialized already,
  // but the script of the page hasn't been executed now
  render_message_router_->OnContextCreated(browser, frame, context);

  // log this event
  frame->ExecuteJavaScript("console.info('[JSRuntime]:frame context created')", frame->GetURL(), 0);

  // binding bridge object and functions
  auto frameId = frame->GetIdentifier();
  auto it = frame_id_to_client_map_.find(frameId);
  if (it == frame_id_to_client_map_.end()) {
    // create and insert the bridge Object into this frame.window object
    CefRefPtr<CefV8Value> objWindow = context->GetGlobal();
    CefRefPtr<CefViewClient> objClient = new CefViewClient(browser, frame, objWindow, bridge_object_name_);
    if (!objClient) {
      log_error("Failed to create the client object");
      return;
    }
    frame_id_to_client_map_[frameId] = objClient;
  }
}

void
RenderDelegate::OnContextReleased(CefRefPtr<CefViewRenderApp> app,
                                  CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context)
{
  render_message_router_->OnContextReleased(browser, frame, context);

  auto frameId = frame->GetIdentifier();
  auto it = frame_id_to_client_map_.find(frameId);
  if (it != frame_id_to_client_map_.end()) {
    frame_id_to_client_map_.erase(it);
  }
}

void
RenderDelegate::OnFocusedNodeChanged(CefRefPtr<CefViewRenderApp> app,
                                     CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefDOMNode> node)
{
  bool is_editable = (node.get() && node->IsEditable());
  if (is_editable != last_node_is_editable_) {
    // Notify the browser of the change in focused element type.
    last_node_is_editable_ = is_editable;
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(kCefViewClientRenderFocusedNodeChangedMessage);
    message->GetArgumentList()->SetBool(0, is_editable);
    frame->SendProcessMessage(PID_BROWSER, message);
  }
}

bool
RenderDelegate::OnProcessMessageReceived(CefRefPtr<CefViewRenderApp> app,
                                         CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefProcessId source_process,
                                         CefRefPtr<CefProcessMessage> message)
{
  if (render_message_router_->OnProcessMessageReceived(browser, frame, source_process, message)) {
    return true;
  }

  if (OnTriggerEventNotifyMessage(browser, frame, source_process, message)) {
    return true;
  }

  return false;
}

bool
RenderDelegate::OnTriggerEventNotifyMessage(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefProcessId source_process,
                                            CefRefPtr<CefProcessMessage> message)
{
  if (message->GetName() == kCefViewClientBrowserTriggerEventMessage) {
    CefRefPtr<CefListValue> args = message->GetArgumentList()->Copy();
    //** arguments(CefValueList)
    //** +------------+
    //** | event name |
    //** | event arg1 |
    //** | event arg2 |
    //** | event arg3 |
    //** | event arg4 |
    //** |    ...     |
    //** |    ...     |
    //** |    ...     |
    //** |    ...     |
    //** +------------+
    if (!args || args->GetSize() <= 0) {
      log_error("Invalid message arguments, event name is required");
      return true;
    }

    if (CefValueType::VTYPE_STRING != args->GetType(0)) {
      log_error("Invalid message arguments, invalid type for event name");
      return true;
    }

    auto name = args->GetString(0);
    args->Remove(0);
    ExecuteEventListener(browser, frame, name, args);

    return true;
  }

  return false;
}

void
RenderDelegate::ExecuteEventListener(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     const CefString& name,
                                     CefRefPtr<CefListValue> args)
{
  if (browser && frame) {
    auto frameId = frame->GetIdentifier();
    auto it = frame_id_to_client_map_.find(frameId);
    if (it != frame_id_to_client_map_.end()) {
      const CefRefPtr<CefViewClient>& objClient = it->second;
      objClient->ExecuteEventListener(name, args);
    }
  }
}

} // namespace CefViewDefaultRenderDelegate
