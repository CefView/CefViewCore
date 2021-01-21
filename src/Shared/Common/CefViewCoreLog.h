//
//  CefViewCoreLog.h
//  CefViewCore
//
//  Created by Sheen Tian on 2020/6/19.
//

#ifndef CefViewCoreLog_h
#define CefViewCoreLog_h
#pragma once

void
log_debug(const char* fmt, ...);
void
log_info(const char* fmt, ...);
void
log_error(const char* fmt, ...);
void
log_fatal(const char* fmt, ...);

#if (defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG))
// DEBUG BUILD
#define logD(format, ...) log_debug(format, ##__VA_ARGS__)
#define logI(format, ...) log_info(format, ##__VA_ARGS__)
#define logE(format, ...) log_error(format, ##__VA_ARGS__)
#define logF(format, ...) log_fatal(format, ##__VA_ARGS__)
#define FLog(message, ...) log_debug(("%s [Line %d] " message), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
// RELEASE BUILD
#define logD(format, ...)
#define logI(format, ...) log_info(format, ##__VA_ARGS__)
#define logE(format, ...) log_error(format, ##__VA_ARGS__)
#define logF(format, ...) log_fatal(format, ##__VA_ARGS__)
#define FLog(message, ...)
#endif

#endif /* CefViewCoreLog_hpp */
