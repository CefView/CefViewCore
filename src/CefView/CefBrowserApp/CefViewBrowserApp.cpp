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

#if defined(OS_LINUX) && defined(_LIBCPP_VERBOSE_ABORT)
#pragma message "**** detected std::__libcpp_verbose_abort, override it"
// Provide own definition for `std::__libcpp_verbose_abort` to avoid dependency
// on the version provided by libc++.
void std::__libcpp_verbose_abort(char const* format, ...) {
  va_list list;
  va_start(list, format);
  std::vfprintf(stderr, format, list);
  va_end(list);

  std::abort();
}
#endif

CefViewBrowserApp::CefViewBrowserApp(const CefString& scheme_name,
                                     const CefString& bridge_name,
                                     CefViewBrowserAppDelegateInterface::RefPtr delegate)
  : builtin_scheme_name_(scheme_name.empty() ? kCefViewDefaultBuiltinSchemaName : scheme_name)
  , bridge_object_name_(bridge_name.empty() ? kCefViewDefaultBridgeObjectName : bridge_name)
  , app_delegate_(delegate)
{
}

CefViewBrowserApp::~CefViewBrowserApp()
{
  logD("CefViewBrowserApp::~CefViewBrowserApp");
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
CefViewBrowserApp::AddLocalFolderResource(const CefString& path, const CefString& url, int priority /*= 0*/)
{
  folderResourceMappingList_.emplace_back(FolderResourceMapping{ path, url, priority });
}

const std::list<FolderResourceMapping>&
CefViewBrowserApp::FolderResourceMappingList()
{
  return folderResourceMappingList_;
}

void
CefViewBrowserApp::AddArchiveResource(const CefString& path,
                                      const CefString& url,
                                      const CefString& password /*= ""*/,
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
CefViewBrowserApp::AddGlobalCookie(const CefString& name,
                                   const CefString& value,
                                   const CefString& domain,
                                   const CefString& url)
{
  CefCookie cookie;
  CefString(&cookie.name) = name;
  CefString(&cookie.value) = value;
  CefString(&cookie.domain) = domain;
  return CefCookieManager::GetGlobalManager(nullptr)->SetCookie(CefString(url), cookie, nullptr);
}

bool
CefViewBrowserApp::DeleteAllCookies()
{
  return CefCookieManager::GetGlobalManager(nullptr)->DeleteCookies(CefString(), CefString(), nullptr);
}

bool
CefViewBrowserApp::AddCrossOriginWhitelistEntry(const CefString& sourceOrigin,
                                                const CefString& targetSchema,
                                                const CefString& targetDomain,
                                                bool allowTargetSubdomains)
{
  return CefAddCrossOriginWhitelistEntry(sourceOrigin, targetSchema, targetDomain, allowTargetSubdomains);
}

bool
CefViewBrowserApp::RemoveCrossOriginWhitelistEntry(const CefString& sourceOrigin,
                                                   const CefString& targetSchema,
                                                   const CefString& targetDomain,
                                                   bool allowTargetSubdomains)
{
  return CefRemoveCrossOriginWhitelistEntry(sourceOrigin, targetSchema, targetDomain, allowTargetSubdomains);
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
    int options = 0                                 //
                  | CEF_SCHEME_OPTION_STANDARD      //
                  | CEF_SCHEME_OPTION_SECURE        //
                  | CEF_SCHEME_OPTION_CORS_ENABLED  //
                  | CEF_SCHEME_OPTION_FETCH_ENABLED //
                  | 0;
    if (!registrar->AddCustomScheme(builtin_scheme_name_, options)) {
      logE("faield to add built-in scheme: %s", builtin_scheme_name_.c_str());
    }
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
