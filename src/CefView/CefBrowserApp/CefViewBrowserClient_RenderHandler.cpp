#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefRenderHandler>
CefViewBrowserClient::GetRenderHandler()
{
  return this;
}

CefRefPtr<CefAccessibilityHandler>
CefViewBrowserClient::GetAccessibilityHandler()
{
  return nullptr;
}
bool
CefViewBrowserClient::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->getRootScreenRect(browser, rect);

  return false;
}

void
CefViewBrowserClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->getViewRect(browser, rect);
}

bool
CefViewBrowserClient::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->getScreenPoint(browser, viewX, viewY, screenX, screenY);

  return false;
}

bool
CefViewBrowserClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->getScreenInfo(browser, screen_info);

  return false;
}

void
CefViewBrowserClient::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onPopupShow(browser, show);
}

void
CefViewBrowserClient::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onPopupSize(browser, rect);
}

void
CefViewBrowserClient::OnPaint(CefRefPtr<CefBrowser> browser,
                              PaintElementType type,
                              const RectList& dirtyRects,
                              const void* buffer,
                              int width,
                              int height)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onPaint(browser, type, dirtyRects, buffer, width, height);
}

#if CEF_VERSION_MAJOR < 124
void
CefViewBrowserClient::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                         PaintElementType type,
                                         const RectList& dirtyRects,
                                         void* shared_handle)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onAcceleratedPaint(browser, type, dirtyRects, shared_handle);
}
#else

void
CefViewBrowserClient::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                         PaintElementType type,
                                         const RectList& dirtyRects,
                                         const CefAcceleratedPaintInfo& info)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onAcceleratedPaint(browser, type, dirtyRects, info);
}
#endif

bool
CefViewBrowserClient::StartDragging(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefDragData> drag_data,
                                    CefRenderHandler::DragOperationsMask allowed_ops,
                                    int x,
                                    int y)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->startDragging(browser, drag_data, allowed_ops, x, y);

  return false;
}

void
CefViewBrowserClient::UpdateDragCursor(CefRefPtr<CefBrowser> browser, DragOperation operation)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->updateDragCursor(browser, operation);
}

void
CefViewBrowserClient::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onScrollOffsetChanged(browser, x, y);
}

void
CefViewBrowserClient::OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                                   const CefRange& selected_range,
                                                   const RectList& character_bounds)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onImeCompositionRangeChanged(browser, selected_range, character_bounds);
}

void
CefViewBrowserClient::OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                             const CefString& selected_text,
                                             const CefRange& selected_range)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onTextSelectionChanged(browser, selected_text, selected_range);
}

void
CefViewBrowserClient::OnVirtualKeyboardRequested(CefRefPtr<CefBrowser> browser, TextInputMode input_mode)
{
  auto delegate = client_delegate_.lock();
  if (delegate)
    return delegate->onVirtualKeyboardRequested(browser, input_mode);
}
