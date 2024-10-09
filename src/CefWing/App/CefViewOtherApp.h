//
//  CefOtherApp.hpp
//  CefViewWing
//
//  Created by Sheen Tian on 2020/6/17.
//

#ifndef CefOtherApp_h
#define CefOtherApp_h
#pragma once

#include "CefViewAppBase.h"

class CefViewOtherApp : public CefViewAppBase
{
  IMPLEMENT_REFCOUNTING(CefViewOtherApp);

public:
  CefViewOtherApp(const std::string& scheme_name);
};

#endif
