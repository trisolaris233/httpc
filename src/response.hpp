#pragma once

#include <vector>
#include <string>
#include "utility.hpp"

namespace httpc {

    struct Response {
        int http_major_version{0};
        int http_minor_version{0};
        int status_code{0};
        std::string reason_phrase;
        std::vector<Header> headers;
        std::string message_body;

        // std::vector<boost::asio::const_buffer> ToBuffers() {
        //     std::vector<boost::asio::const_buffer> buffers;
        //     buffers.push_back(
        //         boost::asio::buffer(std::string("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n"))
        //     );
        //     // buffers.push_back(boost::asio::buffer(std::string("Content-Type: text/html;\r\n")));
            
        //     // buffers.push_back(boost::asio::buffer("\r\n"));
        //     // buffers.push_back(boost::asio::buffer(message_body));
        //     return buffers;
        // }
    };


}