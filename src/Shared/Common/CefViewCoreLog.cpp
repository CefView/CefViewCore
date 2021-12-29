//
//  CefViewCoreLog.cpp
//  CefViewCore
//
//  Created by Sheen Tian on 2020/6/19.
//

#include "CefViewCoreLog.h"

#include <cstdarg>
#include <string>
#include <vector>

typedef enum log_level
{
  ll_debug = 1,
  ll_info,
  ll_error,
  ll_fatal,
} log_level;

#if defined(__APPLE__)
#include <os/log.h>
void
cefView_log(log_level level, const char* message)
{
  switch (level) {
    case ll_debug:
      os_log_debug(OS_LOG_DEFAULT, "[DEBUG]%s", message);
      break;
    case ll_info:
      os_log_info(OS_LOG_DEFAULT, "[INFO]%s", message);
      break;
    case ll_error:
      os_log_error(OS_LOG_DEFAULT, "[ERROR]%s", message);
      break;
    case ll_fatal:
      os_log_fault(OS_LOG_DEFAULT, "[FATAL]%s", message);
      break;
    default:
      break;
  }
}
#elif defined(_WIN32)
#include <Windows.h>
void
cefView_log(log_level level, const char* message)
{
  std::string msg;
  switch (level) {
    case ll_debug:
      msg = "[DEBUG]";
      break;
    case ll_info:
      msg = "[INFO]";
      break;
    case ll_error:
      msg = "[ERROR]";
      break;
    case ll_fatal:
      msg = "[FATAL]";
      break;
    default:
      msg = "[]";
      break;
  }
  msg += message;
  msg += "\r\n";
  OutputDebugStringA(msg.c_str());
}
#elif defined(__linux__)
#include <syslog.h>
void
cefView_log(log_level level, const char* message)
{
  switch (level) {
    case ll_debug:
      syslog(LOG_USER | LOG_DEBUG, "[DEBUG]%s", message);
      break;
    case ll_info:
      syslog(LOG_USER | LOG_INFO, "[INFO]%s", message);
      break;
    case ll_error:
      syslog(LOG_USER | LOG_ERR, "[ERROR]%s", message);
      break;
    case ll_fatal:
      syslog(LOG_USER | LOG_CRIT, "[FATAL]%s", message);
      break;
    default:
      break;
  }
}
#else
#error "unsupported platform"
#endif

#define LOG_MSG_BUFFER_LIMIT 4096

void
log_debug(const char* fmt, ...)
{
  std::vector<char> msg(LOG_MSG_BUFFER_LIMIT, 0);
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg.data(), LOG_MSG_BUFFER_LIMIT, fmt, args);
  va_end(args);
  cefView_log(ll_debug, msg.data());
}

void
log_info(const char* fmt, ...)
{
  std::vector<char> msg(LOG_MSG_BUFFER_LIMIT, 0);
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg.data(), LOG_MSG_BUFFER_LIMIT, fmt, args);
  va_end(args);
  cefView_log(ll_info, msg.data());
}

void
log_error(const char* fmt, ...)
{
  std::vector<char> msg(LOG_MSG_BUFFER_LIMIT, 0);
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg.data(), LOG_MSG_BUFFER_LIMIT, fmt, args);
  va_end(args);
  cefView_log(ll_error, msg.data());
}

void
log_fatal(const char* fmt, ...)
{
  std::vector<char> msg(LOG_MSG_BUFFER_LIMIT, 0);
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg.data(), LOG_MSG_BUFFER_LIMIT, fmt, args);
  va_end(args);
  cefView_log(ll_fatal, msg.data());
}
