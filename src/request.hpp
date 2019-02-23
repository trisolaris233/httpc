#pragma once

#include <string>
#include <vector>
#include <iostream>
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

        friend std::ostream& operator<<(std::ostream& os, const Request& request) {
            os << request.method 
               << " " << request.uri 
               << " " << "HTTP/" << request.http_major_version << "." << request.http_minor_version
               << std::endl;
            
            for (const auto& header : request.headers) {
                os << header.field << ": " << header.value << std::endl;
            }
            os << request.message_body;
            return os;
        }
    };


} // httpc