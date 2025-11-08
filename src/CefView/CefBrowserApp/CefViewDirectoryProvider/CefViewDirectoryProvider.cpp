#include "CefViewDirectoryProvider.h"

#include <algorithm>

#include <include/wrapper/cef_stream_resource_handler.h>

#include "CefViewStreamResourceHandler.h"

#if defined(OS_WIN)
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

CefViewDirectoryProvider::CefViewDirectoryProvider(const std::string& url_path, const std::string& directory_path)
  : url_path_(url_path)
  , directory_path_(directory_path)
{
  DCHECK(!url_path_.empty());
  DCHECK(!directory_path_.empty());

  // Normalize the path values.
  if (url_path_[url_path_.size() - 1] != '/') {
    url_path_ += '/';
  }
  if (directory_path_[directory_path_.size() - 1] != PATH_SEP) {
    directory_path_ += PATH_SEP;
  }
}

bool
CefViewDirectoryProvider::OnRequest(scoped_refptr<CefResourceManager::Request> request)
{
  CEF_REQUIRE_IO_THREAD();

  const std::string& url = request->url();
  if (url.find(url_path_) != 0U) {
    return false;
  }

  const std::string& file_path = GetFilePath(url);

  // Open |file_path| on the FILE thread.
  CefPostTask(TID_FILE_USER_BLOCKING, base::BindOnce(&CefViewDirectoryProvider::OpenOnFileThread, file_path, request));

  return true;
}

std::string
CefViewDirectoryProvider::GetFilePath(const std::string& url)
{
  std::string path_part = url.substr(url_path_.length());
#if defined(OS_WIN)
  std::replace(path_part.begin(), path_part.end(), '/', '\\');
#endif
  return directory_path_ + path_part;
}

void
CefViewDirectoryProvider::OpenOnFileThread(const std::string& file_path,
                                           scoped_refptr<CefResourceManager::Request> request)
{
  CEF_REQUIRE_FILE_USER_BLOCKING_THREAD();

  CefRefPtr<CefStreamReader> stream = CefStreamReader::CreateForFile(file_path);

  // Continue loading on the IO thread.
  CefPostTask(TID_IO, base::BindOnce(&CefViewDirectoryProvider::ContinueOpenOnIOThread, request, file_path, stream));
}

void
CefViewDirectoryProvider::ContinueOpenOnIOThread(scoped_refptr<CefResourceManager::Request> request,
                                                 const std::string& file_path,
                                                 CefRefPtr<CefStreamReader> stream)
{
  CEF_REQUIRE_IO_THREAD();

  CefRefPtr<CefViewStreamResourceHandler> handler;
  if (stream.get()) {
    handler = new CefViewStreamResourceHandler(file_path, request->mime_type_resolver().Run(request->url()), stream);
  }
  request->Continue(handler);
}
