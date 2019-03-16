#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "uri.hpp"
#include "file.hpp"
#include "utility.hpp"

namespace httpc {

    class RequestParser;
    // the structure of http request
    struct Request {
        friend class RequestParser;
        std::string method;
        // Uri         uri2;
        Uri         uri;
        // std::string uri;
        int         http_major_version{0};
        int         http_minor_version{0};
        std::vector<Header> 
                    headers;
        std::string message_body;
        // std::unordered_map<std::string, std::string>
        //             key_values;
        

        inline decltype(auto) HeaderBegin() noexcept {
            return headers.begin();
        }
        inline decltype(auto) HeaderEnd() noexcept {
            return headers.end();
        }
        inline decltype(auto) HeaderCBegin() const noexcept {
            return headers.begin();
        }
        inline decltype(auto) HeaderCEnd() const noexcept {
            return headers.end();
        }
        inline const int HttpVersion() const noexcept {
            return this->http_major_version * 10 + this->http_minor_version;
        }

        template <typename StringT>
        auto FindHeader(StringT&& str) const {
            for (auto itr = this->HeaderCBegin(); itr != this->HeaderCEnd(); ++itr) {
                if (itr->field == str) {
                    return itr;
                }
            }
            return this->HeaderCEnd();
        }


        inline void Reset() {
            this->method.clear();
            this->uri.Reset();
            this->http_major_version = this->http_minor_version = 0;
            this->headers.clear();
            this->message_body.clear();
            this->files_.clear();
            this->fields_.clear();
        }

        inline std::size_t GetFileSize() const {
            if (this->method == "POST") {
                auto itr = this->FindHeader("Content-Length");
                if (itr != this->HeaderCEnd()) {
                    return boost::lexical_cast<std::size_t>(itr->value);
                }
            }
            return 0;
        }

        inline void RecvNextFile() {
            this->file_recv_flag_ = true;
        }

        inline void DenyNextFile() noexcept {
            this->file_recv_flag_ = false;
        }

        inline bool RecvNextFileOrNot() const noexcept {
            return this->file_recv_flag_;
        }

        inline bool RecvComplete() const noexcept {
            return this->file_recv_complete_flag_;
        }

        inline bool RecvSuccessfully() const noexcept {
            return this->file_recv_successfully_flag_;
        }

        const std::vector<File>& GetFiles() const {
            return this->files_;
        }
        
        friend std::ostream& operator<<(std::ostream& os, const Request& request) {
            os << request.method 
               << " " << request.uri 
               << " " << "HTTP/" << request.http_major_version << "." << request.http_minor_version
               << std::endl;
            
            for (const auto& header : request.headers) {
                os << header.field << ": " << header.value << std::endl;
            }
            os << request.message_body << std::endl;

            for (const auto& file : request.files_) {
                os << file << std::endl;
            }
            return os;
        }

    private:
        std::vector<File>                               files_;
        std::unordered_map<std::string, std::string>    fields_;
        bool                                            file_recv_flag_ {false};
        bool                                            file_recv_complete_flag_ {false};
        bool                                            file_recv_successfully_flag_ {false};

    };


} // httpc