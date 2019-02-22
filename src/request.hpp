#pragma once

#include <string>
#include <vector>
#include "utility.hpp"

namespace httpc {

    // the structure of http request
    struct Request {
        std::string method;
        std::string uri;
        int http_major_version{0};
        int http_minor_version{0};
        std::vector<Header> headers;
        std::string message_body;

        std::vector<Header>::iterator HeaderBegin() {
            return headers.begin();
        }
        std::vector<Header>::iterator HeaderEnd() {
            return headers.end();
        }
        std::vector<Header>::const_iterator HeaderCBegin() const {
            return headers.begin();
        }
        std::vector<Header>::const_iterator HeaderCEnd() const {
            return headers.end();
        }
    };


} // httpc