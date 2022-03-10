﻿//
//  CefViewBrowserAppDelegate.h
//  CefView
//
//  Created by Sheen Tian on 2020/6/11.
//

#ifndef CefViewBrowserAppDelegate_h
#define CefViewBrowserAppDelegate_h
#pragma once

#include <memory>
#include <string>

#include <include/cef_app.h>

/// <summary>
///
/// </summary>
class CefViewBrowserAppDelegateInterface
{
public:
  /// <summary>
  ///
  /// </summary>
  typedef std::shared_ptr<CefViewBrowserAppDelegateInterface> RefPtr;

  /// <summary>
  ///
  /// </summary>
  typedef std::weak_ptr<CefViewBrowserAppDelegateInterface> WeakPtr;

  /// <summary>
  ///
  /// </summary>
  virtual ~CefViewBrowserAppDelegateInterface(){};

  /// <summary>
  ///
  /// </summary>
  /// <param name="process_type"></param>
  /// <param name="command_line"></param>
  virtual void onBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="command_line"></param>
  virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="delay_ms"></param>
  virtual void onScheduleMessageLoopWork(int64_t delay_ms) = 0;
};

#endif
