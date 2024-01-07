#ifndef CefViewCoreProtocol_h
#define CefViewCoreProtocol_h
#pragma once

/// <summary>
///
/// </summary>
#define kCefViewSchema "CefView"

/// <summary>
///
/// </summary>
#define kCefViewQueryFuntionName "CefViewQuery"

/// <summary>
///
/// </summary>
#define kCefViewQueryCancelFunctionName "CefViewQueryCancel"

/// <summary>
///
/// </summary>
#define kCefViewBridgeObjectNameKey "bridge-obj-name"

/// <summary>
///
/// </summary>
#define kCefViewWindowsJobNameKey "windows-job-name"

/// <summary>
///
/// </summary>
#define kCefViewDefaultBridgeObjectName "CefViewClient"

/// <summary>
/// CEFVIEWClient.invokeMethod("method_name", ...)
/// </summary>
#define kCefViewInvokeMethodFunctionName "invokeMethod"

/// <summary>
/// CEFVIEWClient.addEventListener(type, listener)
/// </summary>
#define kCefViewAddEventListenerFunctionName "addEventListener"

/// <summary>
/// CEFVIEWClient.removeEventListener(type, listener)
/// </summary>
#define kCefViewRemoveEventListenerFunctionName "removeEventListener"

/// <summary>
/// this message is sent from render process to browser process
/// and is processed in the Qt UI thread
///
/// </summary>
#define kCefViewClientRenderFocusedNodeChangedMessage "CefViewClientRender.FocusedNodeChanged"

/// <summary>
/// this message is sent from render process to browser process
/// and is processed in the Qt UI thread
///
/// </summary>
#define kCefViewClientRenderReportJSResultMessage "CefViewClientRender.ReportJSResult"

/// <summary>
/// this message is sent from render process to browser process
/// and is processed in the Qt UI thread
///
/// format
///   msg.name
///   msg.arg[0]: function name
///   msg.arg[1~...]: function parameters
/// </summary>
#define kCefViewClientRenderInvokeMethodMessage "CefViewClientRender.InvokeMethod"

/// <summary>
/// this message is sent from browser process to render process
/// and is processed in the CefRenderer_Main thread
///
/// format:
///   msg.name:
///   msg.arg[0]: function name
///   msg.arg[1~...]: function parameters
/// </summary>
#define kCefViewClientBrowserTriggerEventMessage "CefViewClientBrowser.TriggerEvent"

/// <summary>
/// window.__cefview_report_js_result__(context, result)
/// context must be double
/// </summary>
#define kCefViewReportJSResultFunctionName "__cefview_report_js_result__"

/// <summary>
///
/// </summary>
#define kCefViewResourceDirectoryName "resources"

/// <summary>
///
/// </summary>
#define kCefViewLocalesDirectoryName "locales"

/// <summary>
///
/// </summary>
#if defined(OS_WINDOWS)
#define kCefViewDefaultUserAgent "CefView/1.0 (Windows; en-us)"
#elif defined(OS_MACOS)
#define kCefViewDefaultUserAgent "CefView/1.0 (macOS; en-us)"
#elif defined(OS_LINUX)
#define kCefViewDefaultUserAgent "CefView/1.0 (Linux; en-us)"
#else
#define kCefViewDefaultUserAgent "CefView/1.0 (Unknown; en-us)"
#endif

#endif
