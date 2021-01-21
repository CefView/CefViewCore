//
//  CefOtherApp.hpp
//  CefViewWing
//
//  Created by Sheen Tian on 2020/6/17.
//

#ifndef CefOtherApp_h
#define CefOtherApp_h
#pragma once

#pragma region project_heasers
#include "CefViewAppBase.h"
#pragma endregion project_heasers

class CefViewOtherApp : public CefViewAppBase
{
public:
  CefViewOtherApp();

private:
  IMPLEMENT_REFCOUNTING(CefViewOtherApp);
};

#endif
