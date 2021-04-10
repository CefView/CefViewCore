#ifndef CefRenderApp_h
#define CefRenderApp_h
#pragma once

#pragma region std_headers
#include <set>
#pragma endregion std_headers

#pragma region project_heasers
#include "CefViewAppBase.h"
#pragma endregion project_heasers

/// <summary>
///
/// </summary>
class CefViewRenderApp
  : public CefViewAppBase
  , public CefRenderProcessHandler
{
public:
  /// <summary>
  ///
  /// </summary>
  CefViewRenderApp(const std::string& bridge_name);

  /// <summary>
  ///
  /// </summary>
  ~CefViewRenderApp();

  /// <summary>
  ///
  /// </summary>
  class RenderDelegate : public virtual CefBaseRefCounted
  {
  public:
    /// <summary>
    ///
    /// </summary>
    /// <param name="app"></param>
    virtual void OnWebKitInitialized(CefRefPtr<CefViewRenderApp> app) {}

    /// <summary>
    ///
    /// </summary>
    /// <param name="app"></param>
    /// <param name="browser"></param>
    virtual void OnBrowserCreated(CefRefPtr<CefViewRenderApp> app,
                                  CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDictionaryValue> extra_info)
    {}

    /// <summary>
    ///
    /// </summary>
    /// <param name="app"></param>
    /// <param name="browser"></param>
    virtual void OnBrowserDestroyed(CefRefPtr<CefViewRenderApp> app, CefRefPtr<CefBrowser> browser) {}

    /// <summary>
    ///
    /// </summary>
    /// <param name="app"></param>
    /// <returns></returns>
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<CefViewRenderApp> app) { return nullptr; }

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
                                  CefRefPtr<CefV8Context> context)
    {}

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
                                   CefRefPtr<CefV8Context> context)
    {}

    /// <summary>
    ///
    /// </summary>
    /// <param name="app"></param>
    /// <param name="browser"></param>
    /// <param name="frame"></param>
    /// <param name="context"></param>
    /// <param name="exception"></param>
    /// <param name="stackTrace"></param>
    virtual void OnUncaughtException(CefRefPtr<CefViewRenderApp> app,
                                     CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefV8Context> context,
                                     CefRefPtr<CefV8Exception> exception,
                                     CefRefPtr<CefV8StackTrace> stackTrace)
    {}

    /// <summary>
    ///
    /// </summary>
    /// <param name="app"></param>
    /// <param name="browser"></param>
    /// <param name="frame"></param>
    /// <param name="node"></param>
    virtual void OnFocusedNodeChanged(CefRefPtr<CefViewRenderApp> app,
                                      CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefDOMNode> node)
    {}

    // Called when a process message is received. Return true if the message was
    // handled and should not be passed on to other handlers. RenderDelegates
    // should check for unique message names to avoid interfering with each
    // other.
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
                                          CefRefPtr<CefProcessMessage> message)
    {
      return false;
    }
  };

  /// <summary>
  ///
  /// </summary>
  typedef std::set<CefRefPtr<RenderDelegate>> RenderDelegateSet;

private:

#pragma region CefApp
  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

#pragma endregion CefApp

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

#pragma endregion CefRenderProcessHandler

private:
  /// <summary>
  ///
  /// </summary>
  std::string bridge_object_name_;

  // Set of supported RenderDelegates. Only used in the renderer process.
  /// <summary>
  ///
  /// </summary>
  RenderDelegateSet render_delegates_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(CefViewRenderApp);
};

#endif
