#include <CefViewBrowserApp.h>

#pragma region cef_headers
#include <include/cef_origin_whitelist.h>
#pragma endregion

#pragma region stl_headers
#include <string>
#include <set>
#pragma endregion

#include <Common/CefViewCoreLog.h>

#include <CefViewCoreProtocol.h>

#include "CefViewSchemeHandler/CefViewSchemeHandlerFactory.h"

CefViewBrowserApp::CefViewBrowserApp(const std::string& scheme_name,
                                     const std::string& bridge_name,
                                     CefViewBrowserAppDelegateInterface::RefPtr delegate)
  : builtin_scheme_name_(scheme_name.empty() ? kCefViewDefaultBuiltinSchemaName : scheme_name)
  , bridge_object_name_(bridge_name.empty() ? kCefViewDefaultBridgeObjectName : bridge_name)
  , app_delegate_(delegate)
{
}

CefViewBrowserApp::~CefViewBrowserApp()
{
  log_debug("CefViewBrowserApp::~CefViewBrowserApp");
}

void
CefViewBrowserApp::CheckInClient(void* ctx, const CefViewBrowserClientDelegateInterface::RefPtr& handler)
{
  client_handler_map_[ctx] = handler;
}

void
CefViewBrowserApp::CheckOutClient(void* ctx)
{
  client_handler_map_.erase(ctx);
}

CefViewBrowserClientDelegateInterface::RefPtr
CefViewBrowserApp::GetClientHandler(void* ctx)
{
  if (client_handler_map_.count(ctx)) {
    return client_handler_map_[ctx].lock();
  }
  return nullptr;
}

void
CefViewBrowserApp::AddLocalFolderResource(const std::string& path, const std::string& url, int priority /*= 0*/)
{
  folderResourceMappingList_.emplace_back(FolderResourceMapping{ path, url, priority });
}

const std::list<FolderResourceMapping>&
CefViewBrowserApp::FolderResourceMappingList()
{
  return folderResourceMappingList_;
}

void
CefViewBrowserApp::AddArchiveResource(const std::string& path,
                                      const std::string& url,
                                      const std::string& password /*= ""*/,
                                      int priority /*= 0*/)
{
  archiveResourceMappingList_.emplace_back(ArchiveResourceMapping{ path, url, password, priority });
}

const std::list<ArchiveResourceMapping>&
CefViewBrowserApp::ArchiveResourceMappingList()
{
  return archiveResourceMappingList_;
}

bool
CefViewBrowserApp::AddGlobalCookie(const std::string& name,
                                   const std::string& value,
                                   const std::string& domain,
                                   const std::string& url)
{
  CefCookie cookie;
  CefString(&cookie.name).FromString(name);
  CefString(&cookie.value).FromString(value);
  CefString(&cookie.domain).FromString(domain);
  return CefCookieManager::GetGlobalManager(nullptr)->SetCookie(CefString(url), cookie, nullptr);
}

bool
CefViewBrowserApp::DeleteAllCookies()
{
  return CefCookieManager::GetGlobalManager(nullptr)->DeleteCookies(CefString(), CefString(), nullptr);
}

bool
CefViewBrowserApp::AddCrossOriginWhitelistEntry(const std::string& sourceOrigin,
                                                const std::string& targetSchema,
                                                const std::string& targetDomain,
                                                bool allowTargetSubdomains)
{
  CefString source(sourceOrigin);
  CefString schema(targetSchema);
  CefString domain(targetDomain);
  return CefAddCrossOriginWhitelistEntry(source, schema, domain, allowTargetSubdomains);
}

bool
CefViewBrowserApp::RemoveCrossOriginWhitelistEntry(const std::string& sourceOrigin,
                                                   const std::string& targetSchema,
                                                   const std::string& targetDomain,
                                                   bool allowTargetSubdomains)
{
  CefString source(sourceOrigin);
  CefString schema(targetSchema);
  CefString domain(targetDomain);
  return CefRemoveCrossOriginWhitelistEntry(source, schema, domain, allowTargetSubdomains);
}

bool
CefViewBrowserApp::ClearCrossOriginWhitelistEntry()
{
  return CefClearCrossOriginWhitelist();
}

bool
CefViewBrowserApp::IsSafeToExit()
{
  return client_handler_map_.empty();
}

//////////////////////////////////////////////////////////////////////////
void
CefViewBrowserApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
  auto delegate = app_delegate_.lock();
  if (delegate)
    delegate->onBeforeCommandLineProcessing(process_type, command_line);
}

void
CefViewBrowserApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
  if (registrar) {
    // register custom scheme
    registrar->AddCustomScheme(builtin_scheme_name_, 0);
  }
}

CefRefPtr<CefResourceBundleHandler>
CefViewBrowserApp::GetResourceBundleHandler()
{
  return nullptr;
}

CefRefPtr<CefBrowserProcessHandler>
CefViewBrowserApp::GetBrowserProcessHandler()
{
  return this;
}

CefRefPtr<CefRenderProcessHandler>
CefViewBrowserApp::GetRenderProcessHandler()
{
  return nullptr;
}

//////////////////////////////////////////////////////////////////////////
void
CefViewBrowserApp::OnContextInitialized()
{
  CEF_REQUIRE_UI_THREAD();

  // register custom scheme and handler
  CefRegisterSchemeHandlerFactory(builtin_scheme_name_, "", new CefViewSchemeHandlerFactory(this));
}

void
CefViewBrowserApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
  auto delegate = app_delegate_.lock();
  if (delegate)
    delegate->onBeforeChildProcessLaunch(command_line);

  if (bridge_object_name_.empty())
    bridge_object_name_ = kCefViewDefaultBridgeObjectName;
  command_line->AppendSwitchWithValue(kCefViewBridgeObjectNameKey, bridge_object_name_);

  if (builtin_scheme_name_.empty())
    builtin_scheme_name_ = kCefViewBuiltinSchemeNameKey;
  command_line->AppendSwitchWithValue(kCefViewBuiltinSchemeNameKey, bridge_object_name_);
}

void
CefViewBrowserApp::OnScheduleMessagePumpWork(int64_t delay_ms)
{
  auto delegate = app_delegate_.lock();

  if (delegate)
    delegate->onScheduleMessageLoopWork(delay_ms);
}

CefRefPtr<CefClient>
CefViewBrowserApp::GetDefaultClient()
{
  return nullptr;
}
