//
//  CefViewSchemeHandlerFactory.h
//  CefView
//
//  Created by Sheen Tian on 2024/10/9.
//

#ifndef CefViewSchemeHandlerFactory_h
#define CefViewSchemeHandlerFactory_h

#pragma once

#include <CefViewCoreGlobal.h>
#include <CefViewBrowserApp.h>

class CefViewSchemeHandlerFactory : public CefSchemeHandlerFactory
{
  IMPLEMENT_REFCOUNTING(CefViewSchemeHandlerFactory);

private:
  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefViewBrowserApp> app_;

public:
  /// <summary>
  ///
  /// </summary>
  /// <param name="delegate"></param>
  CefViewSchemeHandlerFactory(CefRefPtr<CefViewBrowserApp> app);

  /// <summary>
  ///
  /// </summary>
  ~CefViewSchemeHandlerFactory();

  /// <summary>
  ///
  /// </summary>
  /// <param name="browser"></param>
  /// <param name="frame"></param>
  /// <param name="scheme_name"></param>
  /// <param name="request"></param>
  /// <returns></returns>
  virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                               CefRefPtr<CefFrame> frame,
                                               const CefString& scheme_name,
                                               CefRefPtr<CefRequest> request) override;
};

#endif
