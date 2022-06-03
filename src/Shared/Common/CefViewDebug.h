//
//  CefViewDebug.h
//  CefViewCore
//
//  Created by leashi on 2022/5/10.
//

#ifndef CefViewDebug_h
#define CefViewDebug_h

#pragma once

#include <include/cef_browser.h>
#include <include/cef_frame.h>


std::string toString(CefRefPtr<CefBrowser> browser);
std::string toString(CefRefPtr<CefFrame> frame);


#endif /* CefViewDebug_h */
