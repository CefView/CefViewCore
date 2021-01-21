#pragma region project_headers
#include "CefViewDefaultRenderDelegate.h"
#include "CefViewClient.h"
#pragma endregion project_headers

// Must match the value in client_handler.cc.
const char kFocusedNodeChangedMessage[] = "ClientRenderer.FocusedNodeChanged";

namespace CefViewDefaultRenderDelegate {
void
CreateBrowserDelegate(CefViewRenderApp::RenderDelegateSet& delegates, const std::string& bridge_name)
{
  delegates.insert(new RenderDelegate(bridge_name));
}

RenderDelegate::RenderDelegate(const std::string& bridge_name)
  : bridge_object_name_(bridge_name)
{}

void
RenderDelegate::OnRenderThreadCreated(CefRefPtr<CefViewRenderApp> app, CefRefPtr<CefListValue> extra_info)
{}

void
RenderDelegate::OnWebKitInitialized(CefRefPtr<CefViewRenderApp> app)
{
  CefMessageRouterConfig config;
  config.js_query_function = CEFVIEW_QUERY_NAME;
  config.js_cancel_function = CEFVIEW_QUERY_CANCEL_NAME;
  render_message_router_ = CefMessageRouterRendererSide::Create(config);
}

void
RenderDelegate::OnContextCreated(CefRefPtr<CefViewRenderApp> app,
                                 CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context)
{
  render_message_router_->OnContextCreated(browser, frame, context);

  int64 frameId = frame->GetIdentifier();
  auto it = frame_id_to_client_map_.find(frameId);
  if (it == frame_id_to_client_map_.end()) {
    // create and insert the CefViewClient Object into this frame.window object
    CefRefPtr<CefV8Value> objWindow = context->GetGlobal();
    CefRefPtr<CefViewClient> objClient = new CefViewClient(browser, frame);
    if (bridge_object_name_.empty()) {
      bridge_object_name_ = CEFVIEW_OBJECT_NAME;
    }
    objWindow->SetValue(bridge_object_name_, objClient->GetObject(), V8_PROPERTY_ATTRIBUTE_READONLY);
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

  int64 frameId = frame->GetIdentifier();
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
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(kFocusedNodeChangedMessage);
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
  if (message->GetName() == TRIGGEREVENT_NOTIFY_MESSAGE) {
    CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();
    if (messageArguments && (messageArguments->GetSize() >= 2)) {
      int idx = 0;
      if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx)) {
        CefString eventName = messageArguments->GetString(idx++);

        if (CefValueType::VTYPE_DICTIONARY == messageArguments->GetType(idx)) {
          CefRefPtr<CefDictionaryValue> dict = messageArguments->GetDictionary(idx++);

          ExecuteEventListener(browser, frame, eventName, dict);
          return true;
        }
      }
    }
  }

  return false;
}

void
RenderDelegate::ExecuteEventListener(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     const CefString& name,
                                     CefRefPtr<CefDictionaryValue> dict)
{
  if (browser && frame) {
    int64 frameId = frame->GetIdentifier();
    auto it = frame_id_to_client_map_.find(frameId);
    if (it != frame_id_to_client_map_.end()) {
      const CefRefPtr<CefViewClient>& objClient = it->second;
      objClient->ExecuteEventListener(name, dict);
    }
  }
}

} // namespace CefViewDefaultRenderDelegate
