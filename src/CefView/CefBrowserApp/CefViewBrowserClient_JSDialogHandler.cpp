#include <CefViewBrowserClient.h>

#pragma region stl_headers
#include <sstream>
#include <string>
#include <algorithm>
#pragma endregion

#include <Common/CefViewCoreLog.h>

CefRefPtr<CefJSDialogHandler>
CefViewBrowserClient::GetJSDialogHandler()
{
  return this;
}

bool
CefViewBrowserClient::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                 const CefString& origin_url,
                                 JSDialogType dialog_type,
                                 const CefString& message_text,
                                 const CefString& default_prompt_text,
                                 CefRefPtr<CefJSDialogCallback> callback,
                                 bool& suppress_message)
{
  CEF_REQUIRE_UI_THREAD();

  /// Called to run a JavaScript dialog.
  ///
  /// If |origin_url| is non-empty it can be passed to the CefFormatUrlForSecurityDisplay
  /// function to retrieve a secureand user-friendly display string.
  ///
  /// The |default_prompt_text| value will be specified for prompt dialogs only.
  ///
  /// Set |suppress_message| to true and return false to suppress the message
  /// (suppressing messages is preferable to immediately executing the callback
  /// as this is used to detect presumably malicious behavior like spamming alert
  /// messages in onbeforeunload).
  ///
  /// Set |suppress_message| to false and return false to use the default
  /// implementation (the default implementation will show one modal dialog at a
  /// time and suppress any additional dialog requests until the displayed
  /// dialog is dismissed).
  ///
  /// Return true if the application will use a custom dialog or if the callback has
  /// been executed immediately. Custom dialogs may be either modal or modeless. If a
  /// custom dialog is used the application must execute |callback| once the custom
  /// dialog is dismissed.

  auto delegate = client_delegate_.lock();
  if (delegate) {
    return delegate->onJSDialog(
      browser, origin_url, dialog_type, message_text, default_prompt_text, callback, suppress_message);
  }

  return false;
}

bool
CefViewBrowserClient::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                           const CefString& message_text,
                                           bool is_reload,
                                           CefRefPtr<CefJSDialogCallback> callback)
{
  CEF_REQUIRE_UI_THREAD();

  /// Called to run a dialog asking the user if they want to leave a page.
  /// Return false to use the default dialog implementation. Return true if the
  /// application will use a custom dialog or if the callback has been executed
  /// immediately. Custom dialogs may be either modal or modeless. If a custom
  /// dialog is used the application must execute |callback| once the custom
  /// dialog is dismissed.

  auto delegate = client_delegate_.lock();
  if (delegate) {
    return delegate->onBeforeUnloadDialog(browser, message_text, is_reload, callback);
  }

  return false;
}

void
CefViewBrowserClient::OnResetDialogState(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  /// Called to cancel any pending dialogs and reset any saved dialog state.
  /// Will be called due to events like page navigation irregardless of whether
  /// any dialogs are currently pending.

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onResetDialogState(browser);
}

void
CefViewBrowserClient::OnDialogClosed(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  /// Called when the dialog is closed.

  auto delegate = client_delegate_.lock();
  if (delegate)
    delegate->onDialogClosed(browser);
}
