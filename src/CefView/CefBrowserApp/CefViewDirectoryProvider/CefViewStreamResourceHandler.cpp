#include "CefViewStreamResourceHandler.h"

#include <algorithm>
#include <filesystem>
#include <vector>
#include <utility>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "include/base/cef_logging.h"
#include "include/cef_task.h"
#include "include/wrapper/cef_helpers.h"

namespace {
using Range = std::pair<int64_t, int64_t>;

using RangeList = std::vector<Range>;

std::string
TrimString(const std::string& str)
{
  size_t first = 0;
  while (first < str.size() && std::isspace(static_cast<unsigned char>(str[first]))) {
    ++first;
  }

  size_t last = str.size();
  while (last > first && std::isspace(static_cast<unsigned char>(str[last - 1]))) {
    --last;
  }

  return str.substr(first, last - first);
}

void
SplitString(std::vector<std::string>& result, const std::string s, const char delimi)
{
  auto trimed_string = TrimString(s);
  std::stringstream ss(trimed_string);

  std::string item;
  while (std::getline(ss, item, delimi)) {
    result.push_back(TrimString(item));
  }
}

bool
ExtractRangeList(RangeList& result, const std::string& range_header, const int64_t& file_size)
{
  result.clear();

  if (range_header.empty()) {
    return false;
  }

  if (0 != range_header.find("bytes=")) {
    return true;
  }

  std::vector<std::string> rang_pairs_list;
  SplitString(rang_pairs_list, range_header.c_str() + 6, ',');
  for (auto& range_pair : rang_pairs_list) {
    Range ri;

    std::vector<std::string> range_values;
    if (range_pair[0] == '-') {
      // -5678
      SplitString(range_values, range_pair, '-');
      if (range_values.size() == 1) {
        ri.first = 0;
        ri.second = std::atoll(range_values[0].c_str());
      }
    } else if (range_pair[range_pair.length() - 1] == '-') {
      // 1234-
      SplitString(range_values, range_pair, '-');
      if (range_values.size() == 1) {
        ri.first = std::atoll(range_values[0].c_str());
        ri.second = file_size - 1;
      }
    } else {
      // 123-5678
      SplitString(range_values, range_pair, '-');
      if (range_values.size() == 2) {
        ri.first = std::atoll(range_values[0].c_str());
        ri.second = std::atoll(range_values[1].c_str());
      }
    }

    result.push_back(ri);
  }

  return true;
}

bool
IsValidRnage(const Range& range, const int64_t& file_size)
{
  if (true                          //
      && range.first >= 0           //
      && range.second < file_size   //
      && range.first < range.second //
  ) {
    return true;
  }
  return false;
}

int64_t
Make206Response(CefRefPtr<CefResponse>& response, const Range& range, const int64_t& file_size)
{
  response->SetStatus(206);
  response->SetStatusText("Partial Content");

  // Content-Range: bytes {first}-{second}/{file_size_}
  std::ostringstream oss;
  oss << "bytes " << range.first << "-" << range.second << "/" << file_size;
  response->SetHeaderByName("Content-Range", oss.str().c_str(), true);

  // Content-Length
  auto content_length = range.second + 1 - range.first;
  response->SetHeaderByName("Content-Length", std::to_string(content_length), true);

  return content_length;
}

int64_t
Make416Response(CefRefPtr<CefResponse>& response, const int64_t& file_size)
{
  response->SetStatus(416);
  response->SetStatusText("Range Not Satisfiable");

  // Content-Range: bytes */{file_size_}
  std::ostringstream oss;
  oss << "bytes */" << file_size;
  response->SetHeaderByName("Content-Range", oss.str().c_str(), true);

  // Content-Length
  response->SetHeaderByName("Content-Length", std::to_string(file_size), true);

  return file_size;
}

std::string
GetLastModifiedTime(const std::string& file_path)
{
  std::error_code ec;
  auto lwt = std::filesystem::last_write_time(file_path, ec);
  auto tt = std::chrono::system_clock::to_time_t( //
    std::chrono::system_clock::now() + (lwt - std::filesystem::file_time_type::clock::now()));
  std::tm tm = *std::gmtime(&tt);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
  return oss.str();
}
} // namespace

CefViewStreamResourceHandler::CefViewStreamResourceHandler( //
  const CefString& file_path,
  const CefString& mime_type,
  CefRefPtr<CefStreamReader> stream)
  : file_path_(file_path)
  , file_size_(0)
  , mime_type_(mime_type)
  , stream_(stream)
  , bytes_start_(0)
  , bytes_end_(-1)
{
  DCHECK(!mime_type_.empty());
  header_map_ = {
    { "Accept-Ranges", "bytes" },
  };

  std::error_code ec;
  file_size_ = std::filesystem::file_size(file_path_.ToWString(), ec);
}

bool
CefViewStreamResourceHandler::Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback)
{
  DCHECK(!CefCurrentlyOn(TID_UI) && !CefCurrentlyOn(TID_IO));

  range_header_ = request->GetHeaderByName("Range");
  handle_request = true;
  return true;
}

void
CefViewStreamResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                                 int64_t& response_length,
                                                 CefString& redirectUrl)
{
  CEF_REQUIRE_IO_THREAD();

  if (!header_map_.empty()) {
    response->SetHeaderMap(header_map_);
  }

  response->SetMimeType(mime_type_);

  auto modified_time = GetLastModifiedTime(file_path_.ToString());
  response->SetHeaderByName("Last-Modified", modified_time, true);

  RangeList range_list;
  if (ExtractRangeList(range_list, range_header_.ToString(), file_size_)) {
    // has range header
    if (range_list.size() == 1) {
      // range header with only 1 range
      if (IsValidRnage(range_list[0], file_size_)) {
        // range valid
        response_length = Make206Response(response, range_list[0], file_size_);
      } else {
        // range invalid
        response_length = Make416Response(response, file_size_);
      }
    } else if (range_list.size() > 1) {
      // range header with multiple ranges
      response_length = Make416Response(response, file_size_);

      // TODO: build multiparts range content
    } else {
      // invalid header
      response_length = Make416Response(response, file_size_);
    }
  } else {
    // no range header
    response->SetStatus(200);
    response->SetStatusText("OK");
    response->SetHeaderByName("Content-Length", std::to_string(file_size_), true);

    response_length = file_size_;
  }
}

bool
CefViewStreamResourceHandler::Skip(int64_t bytes_to_skip,
                                   int64_t& bytes_skipped,
                                   CefRefPtr<CefResourceSkipCallback> callback)
{
  DCHECK(stream_);

  if (0 == stream_->Seek(bytes_to_skip, SEEK_SET)) {
    // success;
    bytes_skipped = bytes_to_skip;
    bytes_start_ = bytes_skipped;
    return true;
  }

  return CefResourceHandler::Skip(bytes_to_skip, bytes_skipped, callback);
}

bool
CefViewStreamResourceHandler::Read(void* data_out,
                                   int bytes_to_read,
                                   int& bytes_read,
                                   CefRefPtr<CefResourceReadCallback> callback)
{
  DCHECK(!CefCurrentlyOn(TID_UI) && !CefCurrentlyOn(TID_IO));
  DCHECK_GT(bytes_to_read, 0);
  DCHECK(stream_);

  // Read until the buffer is full or until Read() returns 0 to indicate no
  // more data.
  bytes_read = 0;
  int read = 0;
  do {
    read = static_cast<int>(stream_->Read(static_cast<char*>(data_out) + bytes_read, 1, bytes_to_read - bytes_read));
    bytes_read += read;
  } while (read != 0 && bytes_read < bytes_to_read);

  bytes_end_ += (bytes_start_ + bytes_read);
  return (bytes_read > 0);
}

void
CefViewStreamResourceHandler::Cancel()
{
}
