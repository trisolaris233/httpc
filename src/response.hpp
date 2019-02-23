#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include "utility.hpp"
#include "request.hpp"

namespace httpc {

    struct Response {
        int http_major_version{0};
        int http_minor_version{0};
        HTTPStatusCodeEnum status_code;
        std::string reason_phrase;
        std::vector<Header> headers;
        std::string message_body;

        inline void SetDefault(HTTPStatusCodeEnum status) {
            this->http_major_version = this->http_minor_version = 1;
            this->status_code = status;
            this->reason_phrase = GetHTTPReasonPhrase(status);
            this->headers.clear();
            this->headers.push_back({"ContentType", "text/html"});
            this->message_body
                .assign("<html><body><h1>")
                .append(GetHTTPReasonPhrase(status))
                .append("</h1></body></html>");
            //MakeDefaultResponse(status, *this);
        }

        inline void RenderString(const std::string& str) {
            message_body.assign(str);
        }

        std::vector<boost::asio::const_buffer> ToBuffers() {
            std::vector<boost::asio::const_buffer> buffers;
            
            std::stringstream ss;
            ss  << "HTTP/1" << this->http_major_version << "." << this->http_minor_version 
                << " " << static_cast<int>(this->status_code)
                << " " << GetHTTPReasonPhrase(this->status_code) << GetCRLF();
            buffers.push_back(boost::asio::buffer(ss.str()));
            
            for (const auto& header : this->headers) {
                ss.clear();
                ss << header.field << ": " << header.value << GetCRLF();
                buffers.push_back(
                    boost::asio::buffer(ss.str())
                );
            }
            buffers.push_back(boost::asio::buffer(GetCRLF()));
            buffers.push_back(boost::asio::buffer(this->message_body));

            return buffers;
        }

        void WriteBuffer(std::string& str) {
            std::stringstream ss;
            ss << "HTTP/" << this->http_major_version << "." << this->http_minor_version
               << " " << static_cast<int>(this->status_code) << " " << GetHTTPReasonPhrase(this->status_code) << GetCRLF();
            for (const auto& header : this->headers) {
                ss << header.field << ": " << header.value << GetCRLF();
            }
            ss << GetCRLF() << this->message_body;
            str.assign(std::move(ss.str()));
        }

        // auto ToBuffer() -> decltype(boost::asio::buffer(std::string())) {
        //     std::stringstream ss;
        //     ss << "HTTP/" << this->http_major_version << "." << this->http_minor_version
        //        << " " << static_cast<int>(this->status_code) << " " << GetHTTPReasonPhrase(this->status_code) << GetCRLF();
        //     for (const auto& header : this->headers) {
        //         ss << header.field << ": " << header.value << GetCRLF();
        //     }
        //     ss << GetCRLF() << this->message_body;
        //     return boost::asio::buffer(ss.str());
        // }

        friend std::ostream& operator<< (std::ostream& os, const Response& response) {
            os << "HTTP/" << response.http_major_version << "." << response.http_minor_version
               << " " << static_cast<int>(response.status_code) << " " << GetHTTPReasonPhrase(response.status_code) << std::endl;
            for (const auto& header : response.headers) {
                os << header.field << ": " << header.value << std::endl;
            }
            os << response.message_body;
            return os;
        }
    };


}