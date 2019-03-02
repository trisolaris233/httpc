#pragma once

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <fstream>
// #include <filesystem>   // for C++17 filesystem
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
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

        inline bool IsAsyncWriting() const noexcept {
            return this->async_state_flag_;
        }

        inline bool Empty() const noexcept {
            return static_cast<int>(this->status_code) == 0 && this->headers.empty() &&
                 this->reason_phrase.empty() && this->message_body.empty();
        }

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

        template <typename T>
        std::enable_if_t<
            std::is_same_v<std::decay_t<T>, std::string> || 
            std::is_same_v<std::decay_t<T>, char*>,
            void
        >
        inline RenderFromStaticFile(T&& str) {
            std::ifstream local_file(
                std::forward<std::remove_reference_t<decltype(str)>>(str)
            );
            for (; local_file.is_open() && !local_file.eof(); ) {
                auto chr {local_file.get()};
                if (EOF == chr) break;
                this->message_body.push_back(
                    static_cast<typename decltype(this->message_body)::value_type>(chr)
                );
            }
        }

        template <typename T>
        std::enable_if_t<
            std::is_same_v<std::decay_t<T>, std::string> || 
            std::is_same_v<std::decay_t<T>, char*>,
            void
        >
        inline AsyncRenderFromStaticFile(T&& str) {
            auto handler = std::async(std::launch::async, [&str, this]() {
                // set the async writing state
                this->async_state_flag_ = true;
                std::ifstream local_file(
                    std::forward<std::remove_reference_t<decltype(str)>>(str)
                );
                for (; local_file.is_open() && !local_file.eof(); ) {
                    auto chr {local_file.get()};
                    if (EOF == chr) break;
                    this->message_body.push_back(
                        static_cast<typename decltype(this->message_body)::value_type>(chr)
                    );
                }
                this->async_state_flag_ = false;
            });
        }

        // template <typename T, typename Func>

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

        friend std::ostream& operator<< (std::ostream& os, const Response& response) {
            os << "HTTP/" << response.http_major_version << "." << response.http_minor_version
               << " " << static_cast<int>(response.status_code) << " " << GetHTTPReasonPhrase(response.status_code) << std::endl;
            for (const auto& header : response.headers) {
                os << header.field << ": " << header.value << std::endl;
            }
            os << response.message_body;
            return os;
        }

        template <typename StringT, typename ValueT>
        inline bool AddVariable(StringT&& str, ValueT&& value) {
            return this->variable_table_.insert(
                std::make_pair(
                    std::forward<std::remove_reference_t<decltype(str)>>(str),
                    std::forward<std::string>(
                        boost::lexical_cast<std::string>(
                            std::forward<std::remove_reference_t<decltype(value)>>(value)
                        )
                    )
                )
            ).second;
        }

    private:
        mutable bool    async_state_flag_{false};
        std::map<std::string, std::string>
                        variable_table_;

    };


}