#include "CefViewRenderApp.h"

#pragma region stl_headers
#include <string>
#pragma endregion

#pragma region cef_headers
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>
#pragma endregion

#include <Common/CefViewCoreLog.h>

#include <CefViewCoreProtocol.h>

CefViewRenderApp::CefViewRenderApp(const std::string& scheme_name, const std::string& bridge_name)
  : CefViewAppBase(scheme_name)
  , bridge_object_name_(bridge_name)
  , last_node_is_editable_(false)
{
}

CefViewRenderApp::~CefViewRenderApp() {}

//////////////////////////////////////////////////////////////////////////
CefRefPtr<CefRenderProcessHandler>
CefViewRenderApp::GetRenderProcessHandler()
{
  return this;
}

void
CefViewRenderApp::OnWebKitInitialized()
{
  CEF_REQUIRE_RENDERER_THREAD();

  CefMessageRouterConfig config;
  config.js_query_function = kCefViewQueryFuntionName;
  config.js_cancel_function = kCefViewQueryCancelFunctionName;
  message_router_ = CefMessageRouterRendererSide::Create(config);
}

void
CefViewRenderApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
{
  CEF_REQUIRE_RENDERER_THREAD();
}

void
CefViewRenderApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_RENDERER_THREAD();
}

CefRefPtr<CefLoadHandler>
CefViewRenderApp::GetLoadHandler()
{
  CEF_REQUIRE_RENDERER_THREAD();

  return nullptr;
}

void
CefViewRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context)
{
  CEF_REQUIRE_RENDERER_THREAD();

  // [Javascript Context]
  // V8 context for this frame has been initialized already,
  // but the script of the page hasn't been executed now
  message_router_->OnContextCreated(browser, frame, context);

  // log this event
  frame->ExecuteJavaScript("console.info('[JSRuntime]:frame context created')", frame->GetURL(), 0);

  // binding bridge object and functions
  auto frameId = frame->GetIdentifier();
  auto it = frame_id_to_bridge_obj_map_.find(frameId);
  if (it == frame_id_to_bridge_obj_map_.end()) {
    // create and insert the bridge Object into this frame.window object
    CefRefPtr<CefV8Value> objWindow = context->GetGlobal();
    CefRefPtr<CefViewBridgeObject> objClient = new CefViewBridgeObject(browser, frame, objWindow, bridge_object_name_);
    if (!objClient) {
      log_error("Failed to create the client object");
      return;
    }
    frame_id_to_bridge_obj_map_[frameId] = objClient;
  }
}

void
CefViewRenderApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefV8Context> context)
{
  CEF_REQUIRE_RENDERER_THREAD();

  message_router_->OnContextReleased(browser, frame, context);

  auto frameId = frame->GetIdentifier();
  auto it = frame_id_to_bridge_obj_map_.find(frameId);
  if (it != frame_id_to_bridge_obj_map_.end()) {
    frame_id_to_bridge_obj_map_.erase(it);
  }
}

void
CefViewRenderApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefV8Context> context,
                                      CefRefPtr<CefV8Exception> exception,
                                      CefRefPtr<CefV8StackTrace> stackTrace)
{
  CEF_REQUIRE_RENDERER_THREAD();
}

void
CefViewRenderApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefDOMNode> node)
{
  CEF_REQUIRE_RENDERER_THREAD();

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
CefViewRenderApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefProcessId source_process,
                                           CefRefPtr<CefProcessMessage> message)
{
  CEF_REQUIRE_RENDERER_THREAD();

  bool handled = false;

  if (message_router_->OnProcessMessageReceived(browser, frame, source_process, message)) {
    handled = true;
  }

  if (OnTriggerEventNotifyMessage(browser, frame, source_process, message)) {
    handled = true;
  }

  return handled;
}

bool
CefViewRenderApp::OnTriggerEventNotifyMessage(CefRefPtr<CefBrowser> browser,
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
CefViewRenderApp::ExecuteEventListener(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& name,
                                       CefRefPtr<CefListValue> args)
{
  if (browser && frame) {
    auto frameId = frame->GetIdentifier();
    auto it = frame_id_to_bridge_obj_map_.find(frameId);
    if (it != frame_id_to_bridge_obj_map_.end()) {
      const CefRefPtr<CefViewBridgeObject>& objClient = it->second;
      objClient->ExecuteEventListener(name, args);
    }
  }
}
