#ifndef CefRenderApp_h
#define CefRenderApp_h
#pragma once

#pragma region stl_headers
#include <string>
#include <unordered_map>
#pragma endregion

#pragma region cef_headers
#include <include/wrapper/cef_message_router.h>
#pragma endregion

#include <CefViewCoreGlobal.h>

#include "CefViewAppBase.h"
#include "../Bridge/CefViewBridgeObject.h"

/// <summary>
///
/// </summary>
class CefViewRenderApp
  : public CefViewAppBase
  , public CefRenderProcessHandler
{
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(CefViewRenderApp);

private:
  std::string bridge_object_name_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefMessageRouterRendererSide> message_router_;

  /// <summary>
  ///
  /// </summary>
  typedef std::unordered_map<CefFrameId, CefRefPtr<CefViewBridgeObject>> FrameID2BridgeObjMap;
  FrameID2BridgeObjMap frame_id_to_bridge_obj_map_;

  /// <summary>
  ///
  /// </summary>
  bool last_node_is_editable_;

public:
  /// <summary>
  ///
  /// </summary>
  CefViewRenderApp(const std::string& scheme_name, const std::string& bridge_name);

  /// <summary>
  ///
  /// </summary>
  ~CefViewRenderApp();

private:
#pragma region CefApp
  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

#pragma endregion

#pragma region CefRenderProcessHandler

  /// <summary>
  ///
  /// </summary>
  virtual void OnWebKitInitialized() override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="extra_info"></param>
  virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="context"></param>
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="context"></param>
  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="context"></param>
  /// <param name="exception"></param>
  /// <param name="stackTrace"></param>
  virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context,
                                   CefRefPtr<CefV8Exception> exception,
                                   CefRefPtr<CefV8StackTrace> stackTrace) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="node"></param>
  virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefDOMNode> node) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="source_process"></param>
  /// <param name="message"></param>
  /// <returns></returns>
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;

#pragma endregion

private:
  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="source_process"></param>
  /// <param name="message"></param>
  /// <returns></returns>
  bool OnTriggerEventNotifyMessage(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefProcessId source_process,
                                   CefRefPtr<CefProcessMessage> message);

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="name"></param>
  /// <param name="list"></param>
  void ExecuteEventListener(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            const CefString& name,
                            CefRefPtr<CefListValue> args);
};

#endif
