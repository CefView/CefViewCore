//
//  CefViewCoreLog.h
//  CefViewCore
//
//  Created by Sheen Tian on 2020/6/19.
//

#ifndef CefViewCoreLog_h
#define CefViewCoreLog_h
#pragma once
#include <string>

void
log_debug(const char* fmt, ...);
void
log_info(const char* fmt, ...);
void
log_error(const char* fmt, ...);
void
log_fatal(const char* fmt, ...);

class ScopeLogger
{
public:
  ScopeLogger(const std::string& fn)
    : functionName_(fn)
  {
    // enter scope
    log_debug("+++ %s", functionName_.c_str());
  }

  ~ScopeLogger()
  {
    // leave scope
    log_debug("--- %s", functionName_.c_str());
  }

  std::string functionName_;
};

#if (defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG))
// DEBUG BUILD
#define logD(format, ...) log_debug(format, ##__VA_ARGS__)
#define logI(format, ...) log_info(format, ##__VA_ARGS__)
#define logE(format, ...) log_error(format, ##__VA_ARGS__)
#define logF(format, ...) log_fatal(format, ##__VA_ARGS__)
#define logScope() ScopeLogger __scope_logger__(__FUNCTION__);
#else
// RELEASE BUILD
#define logD(format, ...)
#define logI(format, ...) log_info(format, ##__VA_ARGS__)
#define logE(format, ...) log_error(format, ##__VA_ARGS__)
#define logF(format, ...) log_fatal(format, ##__VA_ARGS__)
#define logScope()
#endif

#endif /* CefViewCoreLog_hpp */
