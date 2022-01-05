//
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
  /// <param name="delay_ms"></param>
  virtual void OnScheduleMessageLoopWork(int64_t delay_ms) = 0;
};

#endif
