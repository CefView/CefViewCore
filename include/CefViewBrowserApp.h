//
//  CefViewBrowserApp.h
//  CefViewCore
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserApp_h
#define CefViewBrowserApp_h
#pragma once

#pragma region stl_headers
#include <string>
#include <unordered_map>
#pragma endregion

#include <CefViewCoreGlobal.h>
#include <CefViewBrowserAppDelegate.h>
#include <CefViewBrowserClientDelegate.h>

class CefViewBrowserApp
  : public CefApp
  , public CefBrowserProcessHandler
{
  IMPLEMENT_REFCOUNTING(CefViewBrowserApp);

private:
  // The name of the bridge object
  std::string builtin_scheme_name_;
  std::string bridge_object_name_;
  std::unordered_map<void*, CefViewBrowserClientDelegateInterface::WeakPtr> client_handler_map_;

  // The app delegate
  CefViewBrowserAppDelegateInterface::WeakPtr app_delegate_;

public:
  CefViewBrowserApp(const std::string& scheme_name,
                    const std::string& bridge_name,
                    CefViewBrowserAppDelegateInterface::RefPtr delegate);

  ~CefViewBrowserApp();

  void CheckInClient(void* ctx, const CefViewBrowserClientDelegateInterface::RefPtr& handler);

  void CheckOutClient(void* ctx);

  CefViewBrowserClientDelegateInterface::RefPtr GetClientHandler(void* ctx);

  bool IsSafeToExit();

private:
#pragma region CefApp
  //////////////////////////////////////////////////////////////////////////
  // CefApp methods:
  virtual void OnBeforeCommandLineProcessing(const CefString& process_type,
                                             CefRefPtr<CefCommandLine> command_line) override;

  virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

  virtual CefRefPtr<CefResourceBundleHandler> GetResourceBundleHandler() override;

  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

#pragma endregion

#pragma region CefBrowserProcessHandler
  //////////////////////////////////////////////////////////////////////////
  // CefBrowserProcessHandler methods:
  virtual void OnContextInitialized() override;

  virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;

  virtual void OnScheduleMessagePumpWork(int64_t delay_ms) override;

  CefRefPtr<CefClient> GetDefaultClient() override;

#pragma endregion
};

#endif
