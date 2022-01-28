#include <CefViewBrowserClient.h>

#pragma region std_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion std_headers

#pragma region cef_headers
#include <include/cef_app.h>
#pragma endregion cef_headers

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefAccessibilityHandler>
CefViewBrowserClient::GetAccessibilityHandler()
{
  return nullptr;
}
bool
CefViewBrowserClient::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
  return false;
}

void
CefViewBrowserClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{}

bool
CefViewBrowserClient::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
  return false;
}

bool
CefViewBrowserClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
  return false;
}

void
CefViewBrowserClient::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{}

void
CefViewBrowserClient::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{}

void
CefViewBrowserClient::OnPaint(CefRefPtr<CefBrowser> browser,
                              PaintElementType type,
                              const RectList& dirtyRects,
                              const void* buffer,
                              int width,
                              int height)
{}

void
CefViewBrowserClient::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                         PaintElementType type,
                                         const RectList& dirtyRects,
                                         void* shared_handle)
{}

bool
CefViewBrowserClient::StartDragging(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefDragData> drag_data,
                                    CefRenderHandler::DragOperationsMask allowed_ops,
                                    int x,
                                    int y)
{
  return false;
}

void
CefViewBrowserClient::UpdateDragCursor(CefRefPtr<CefBrowser> browser, DragOperation operation)
{}

void
CefViewBrowserClient::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{}

void
CefViewBrowserClient::OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                                   const CefRange& selected_range,
                                                   const RectList& character_bounds)
{}

void
CefViewBrowserClient::OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                             const CefString& selected_text,
                                             const CefRange& selected_range)
{}

void
CefViewBrowserClient::OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, TextInputMode input_mode)
{}
