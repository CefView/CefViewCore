#ifndef CEFVIEWDIRECTORYPROVIDER_H
#define CEFVIEWDIRECTORYPROVIDER_H

#pragma once

#include <string>

#include <include/wrapper/cef_resource_manager.h>

/// <summary>
/// Provider of contents loaded from a directory on
/// the file system with support for HTTP range request.
/// </summary>
class CefViewDirectoryProvider : public CefResourceManager::Provider
{
public:
  CefViewDirectoryProvider(const std::string& url_path, const std::string& directory_path);

protected:
  CefViewDirectoryProvider(const CefViewDirectoryProvider&) = delete;
  CefViewDirectoryProvider& operator=(const CefViewDirectoryProvider&) = delete;

  bool OnRequest(scoped_refptr<CefResourceManager::Request> request) override;

private:
  std::string GetFilePath(const std::string& url);

  static void OpenOnFileThread(const std::string& file_path, scoped_refptr<CefResourceManager::Request> request);

  static void ContinueOpenOnIOThread(scoped_refptr<CefResourceManager::Request> request,
                                     const std::string& file_path,
                                     CefRefPtr<CefStreamReader> stream);

  std::string url_path_;
  std::string directory_path_;
};

#endif
