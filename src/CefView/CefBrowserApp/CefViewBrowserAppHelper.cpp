#include <CefViewBrowserApp.h>

#pragma region std_headers
#include <string>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#pragma endregion cef_headers

#include "SchemeHandlers/CefViewDefaultSchemeHandler.h"

void
CefViewBrowserApp::RegisterCustomSchemesHandlerFactories()
{
  CefViewDefaultSchemeHandler::RegisterSchemeHandlerFactory();
}

void
CefViewBrowserApp::RegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
  CefViewDefaultSchemeHandler::RegisterScheme(registrar);
}
