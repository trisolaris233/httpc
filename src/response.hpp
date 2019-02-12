#pragma once

#include <vector>
#include "header.hpp"

namespace httpc {

enum HTTPStatusCodeEnum {

};

struct Response {
    int http_version_major;
    int http_version_minor;
    HTTPStatusCodeEnum status_code;
    std::string reason_phrase;
    std::vector<Header> headers;

    Response& operator << (const Header& header);
    Response& operator << (Header&& header);
};


}