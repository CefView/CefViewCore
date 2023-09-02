//
//  CefViewCoreLog.cpp
//  CefViewCore
//
//  Created by leashi on 2022/5/10.
//

#include "CefViewDebug.h"

#include <string>

std::string toString(CefRefPtr<CefBrowser> browser)
{
    std::string msg;

    msg += "( ";
    msg += "CefBrowser:";
    msg += ", ptr=" + std::to_string((int64_t)browser.get());
#if CEF_VERSION_MAJOR > 91
    msg += ", IsValid=" + std::to_string(browser->IsValid());
#endif
    msg += ", GetHost=" + std::to_string((int64_t)browser->GetHost().get());
    msg += ", CanGoBack=" + std::to_string(browser->CanGoBack());
    msg += ", CanGoForward=" + std::to_string(browser->CanGoForward());
    msg += ", IsLoading=" + std::to_string(browser->IsLoading());
    msg += ", GetIdentifier=" + std::to_string(browser->GetIdentifier());
    msg += ", IsPopup=" + std::to_string(browser->IsPopup());
    msg += ", HasDocument=" + std::to_string(browser->HasDocument());
    msg += ", GetMainFrame=" + std::to_string((int64_t)browser->GetMainFrame().get());
    msg += ", GetFocusedFrame=" + std::to_string((int64_t)browser->GetFocusedFrame().get());
    msg += ", GetFrameCount=" + std::to_string(browser->GetFrameCount());
    msg += " )";

    return msg;
}

std::string toString(CefRefPtr<CefFrame> frame)
{
    std::string msg;

    msg += "( ";
    msg += "CefFrame:";
    msg += ", ptr=" + std::to_string((int64_t)frame.get());
    msg += ", IsValid=" + std::to_string(frame->IsValid());
    msg += ", IsMain=" + std::to_string(frame->IsMain());
    msg += ", IsFocused=" + std::to_string(frame->IsFocused());
    msg += ", GetName=" + frame->GetName().ToString();
    msg += ", GetIdentifier=" + std::to_string(frame->GetIdentifier());
    msg += ", GetParent=" + std::to_string((int64_t)frame->GetParent().get());
    msg += ", GetURL=" + frame->GetURL().ToString();
    msg += ", GetBrowser=" + std::to_string((int64_t)frame->GetBrowser().get());
    //msg += ", GetV8Context=" + std::to_string((int64_t)frame->GetV8Context().get());
    msg += " )";

    return msg;
}
