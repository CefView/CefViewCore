﻿//
//  CefViewBrowserApp.h
//  CefViewCore
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserApp_h
#define CefViewBrowserApp_h
#pragma once

#pragma region stl_headers
#include <unordered_map>
#include <list>
#pragma endregion

#include <CefViewCoreGlobal.h>
#include <CefViewBrowserAppDelegate.h>
#include <CefViewBrowserClientDelegate.h>

struct FolderResourceMapping
{
  CefString path;
  CefString url;
  int priority;
};

struct ArchiveResourceMapping
{
  CefString path;
  CefString url;
  CefString password;
  int priority;
};

class CefViewBrowserApp
  : public CefApp
  , public CefBrowserProcessHandler
{
  IMPLEMENT_REFCOUNTING(CefViewBrowserApp);

private:
  // The name of the bridge object
  CefString builtin_scheme_name_;
  CefString bridge_object_name_;
  std::unordered_map<void*, CefViewBrowserClientDelegateInterface::WeakPtr> client_handler_map_;

  // The app delegate
  CefViewBrowserAppDelegateInterface::WeakPtr app_delegate_;

  std::list<FolderResourceMapping> folderResourceMappingList_;

  std::list<ArchiveResourceMapping> archiveResourceMappingList_;

public:
  CefViewBrowserApp(const CefString& scheme_name,
                    const CefString& bridge_name,
                    CefViewBrowserAppDelegateInterface::RefPtr delegate);

  ~CefViewBrowserApp();

  void CheckInClient(void* ctx, const CefViewBrowserClientDelegateInterface::RefPtr& handler);

  void CheckOutClient(void* ctx);

  CefViewBrowserClientDelegateInterface::RefPtr GetClientHandler(void* ctx);

  void AddLocalFolderResource(const CefString& path, const CefString& url, int priority = 0);
  const std::list<FolderResourceMapping>& FolderResourceMappingList();

  void AddArchiveResource(const CefString& path,
                          const CefString& url,
                          const CefString& password = "",
                          int priority = 0);
  const std::list<ArchiveResourceMapping>& ArchiveResourceMappingList();

  bool AddGlobalCookie(const CefString& name,
                       const CefString& value,
                       const CefString& domain,
                       const CefString& url);

  bool DeleteAllCookies();

  bool AddCrossOriginWhitelistEntry(const CefString& sourceOrigin,
                                    const CefString& targetSchema,
                                    const CefString& targetDomain,
                                    bool allowTargetSubdomains);

  bool RemoveCrossOriginWhitelistEntry(const CefString& sourceOrigin,
                                       const CefString& targetSchema,
                                       const CefString& targetDomain,
                                       bool allowTargetSubdomains);

  bool ClearCrossOriginWhitelistEntry();

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
