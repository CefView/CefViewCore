#ifndef CEFVIEWSTREAMRESOURCEHANDLER_H
#define CEFVIEWSTREAMRESOURCEHANDLER_H

#pragma once

#include <include/cef_resource_handler.h>

class CefViewStreamResourceHandler : public CefResourceHandler
{
public:
  CefViewStreamResourceHandler( //
    const CefString& file_path,
    const CefString& mime_type,
    CefRefPtr<CefStreamReader> stream);

protected:
  bool Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback) override;

  void GetResponseHeaders(CefRefPtr<CefResponse> response, int64_t& response_length, CefString& redirectUrl) override;

  bool Skip(int64_t bytes_to_skip, int64_t& bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) override;

  bool Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback) override;

  void Cancel() override;

private:
  const CefString file_path_;
  const CefString mime_type_;
  const CefRefPtr<CefStreamReader> stream_;

  int64_t file_size_;
  int64_t bytes_start_;
  int64_t bytes_end_;
  CefString range_header_;
  CefResponse::HeaderMap header_map_;

  IMPLEMENT_REFCOUNTING(CefViewStreamResourceHandler);
};

#endif
