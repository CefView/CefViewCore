#pragma once
#pragma region std_headers
#include <unordered_map>
#pragma endregion

#pragma region cef_headers
#include <include/wrapper/cef_message_router.h>
#pragma endregion cef_headers

#pragma region project_headers
#include "../CefViewRenderApp.h"
#include "CefViewClient.h"
#pragma endregion project_headers

namespace CefViewDefaultRenderDelegate {
/// <summary>
///
/// </summary>
void
CreateBrowserDelegate(CefViewRenderApp::RenderDelegateSet& delegates, const std::string& bridge_name);

/// <summary>
///
/// </summary>
class RenderDelegate : public CefViewRenderApp::RenderDelegate
{
  /// <summary>
  ///
  /// </summary>
  typedef std::unordered_map<int64_t, CefRefPtr<CefViewClient>> FrameID2CefClientMap;

public:
  /// <summary>
  ///
  /// </summary>
  RenderDelegate(const std::string& objName);

  /// <summary>
  ///
  /// </summary>
  /// <param name="app"></param>
  virtual void OnWebKitInitialized(CefRefPtr<CefViewRenderApp> app) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="app"></param>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="context"></param>
  virtual void OnContextCreated(CefRefPtr<CefViewRenderApp> app,
                                CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="app"></param>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="context"></param>
  virtual void OnContextReleased(CefRefPtr<CefViewRenderApp> app,
                                 CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) override;

  virtual void OnFocusedNodeChanged(CefRefPtr<CefViewRenderApp> app,
                                    CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefDOMNode> node) override;

  /// <summary>
  ///
  /// </summary>
  /// <param name="app"></param>
  /// <param name="browser"></param>
  /// <param name="source_process"></param>
  /// <param name="message"></param>
  /// <returns></returns>
  virtual bool OnProcessMessageReceived(CefRefPtr<CefViewRenderApp> app,
                                        CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;

protected:
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

private:
  std::string bridge_object_name_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefMessageRouterRendererSide> render_message_router_;

  /// <summary>
  ///
  /// </summary>
  FrameID2CefClientMap frame_id_to_client_map_;

  /// <summary>
  ///
  /// </summary>
  bool last_node_is_editable_;

private:
  IMPLEMENT_REFCOUNTING(RenderDelegate);
};

} // namespace CefViewDefaultRenderDelegate
