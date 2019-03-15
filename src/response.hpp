#pragma once

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <fstream>
#include <type_traits>
// #include <filesystem>   // for C++17 filesystem
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#ifdef HTTPC_ENABLE_GZIP
#include "gzip.hpp"
#endif
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

        inline void Reset() {
            this->http_major_version = this->http_minor_version = 0;
            this->status_code = static_cast<HTTPStatusCodeEnum>(0);
            this->reason_phrase.clear();
            this->headers.clear();
            this->message_body.clear();
        }

        inline bool IsEmptyStatusCode() const noexcept {
            return static_cast<int>(this->status_code) == 0;
        }

        inline bool IsEmptyReasonPhrase() const noexcept {
            return this->reason_phrase.empty();
        }

        inline bool IsEmptyHeaders() const noexcept {
            return this->headers.empty();
        }

        inline bool IsEmptyMessageBody() const noexcept {
            return this->message_body.empty();
        }

        inline bool Empty() const noexcept {
            return static_cast<int>(this->status_code) == 0 && this->headers.empty() &&
                 this->reason_phrase.empty() && this->message_body.empty();
        }

        inline bool IsAsyncWriting() const noexcept {
            return this->async_state_flag_;
        }

        template <typename IntType>
        inline std::enable_if_t<std::is_integral_v<IntType>, bool>
        SetHTTPVersion(IntType version) {
            auto supported_version = GetHttpVersionSupported();
            auto itr = std::find(supported_version.begin(), supported_version.end(), version);
            if (itr == supported_version.end()) {
                return false;
            }
            this->http_major_version = (*itr) / 10 % 10;
            this->http_major_version = (*itr) % 10;
            return true;
        }

        inline void SetStatusCode(HTTPStatusCodeEnum status_code_enum) noexcept {
            this->status_code = status_code_enum;
            this->reason_phrase = GetHTTPReasonPhrase(this->status_code);
        } 
        
        template <typename StringT>
        inline Response& AddHeader(StringT&& field, StringT&& value) {
            bool flag = false;
            for (auto itr = this->headers.begin(); itr != this->headers.end(); ++itr) {
                if (itr->field == field) {
                    itr->value = value;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                headers.emplace_back(std::forward<Header>({
                    std::forward<StringT>(field),
                    std::forward<StringT>(value)
                }));
            }
            
            return *this;
        }

        template <typename CharT>
        inline std::enable_if_t<std::is_same_v<std::decay_t<CharT>, char>, Response&>
        AddHeader(CharT* field, CharT* value) {
            this->AddHeader(std::string(field), std::string(value));
            return *this;
        }

        inline Response& AddHeader(Header&& header) {
            return this->AddHeader(std::forward<std::string>(header.field), std::forward<std::string>(header.value));
        }

        inline void ClearHeaders() noexcept {
            this->headers.clear();
        }

        inline void SetDefaultHeaders() {
            //this->headers.clear();
            // this->headers.push_back({"Content-Type", "text/html"});
            if (this->http_major_version == 1 &&  this->http_minor_version == 1) {
                this->AddHeader("Connection", "keep-alive");
            }
            this->AddHeader("Server", "httpc");
            // bool connection_flag = false;
            // bool server_flag = false;
            // for(auto itr = this->headers.begin(); itr != this->headers.end(); ++itr) {
            //     if (itr->field == "Connection") {
            //         connection_flag = true;
            //         if (server_flag) break;
            //     } else if (itr->field == "Server") {
            //         server_flag = true;
            //         if (connection_flag) break;
            //     }
            // }
            // if (!connection_flag)
            //     this->headers.emplace_back(Header{"Connection", "keep-alive"});
            // if (!server_flag)
            //     this->headers.emplace_back(Header{"Server", "httpc"});
        #ifdef HTTPC_ENABLE_GZIP
            this->AddHeader("Content-Encoding", "gzip");
        #endif
        }

        #ifdef HTTPC_ENABLE_GZIP
        inline Response& GzipEncode() {
            httpc::compress(this->message_body, this->gzip_message_body_, true);
            return *this;
        }

        std::string GetGzipEncode() const noexcept {
            return this->gzip_message_body_;
        }
        #endif

        inline void Clear() noexcept {
            this->http_major_version = this->http_minor_version = 0;
            this->status_code = static_cast<decltype(this->status_code)>(0);
            this->reason_phrase.clear();
            this->headers.clear();
            this->message_body.clear();
        #ifdef HTTPC_ENABLE_GZIP
            this->gzip_message_body_.clear();
        #endif
        }

        inline void SetDefault(HTTPStatusCodeEnum status) {
            this->http_major_version = this->http_minor_version = 1;
            this->status_code = status;
            this->reason_phrase = GetHTTPReasonPhrase(status);
            this->headers.clear();
            this->AddHeader("Content-Type", "text/html");
        #ifdef HTTPC_ENABLE_GZIP
            this->headers.push_back({"Content-Encoding", "gzip"});
        #endif
            this->message_body
                .assign("<html><body><h1>")
                .append(GetHTTPReasonPhrase(status))
                .append("</h1></body></html>");
        #ifdef HTTPC_ENABLE_GZIP
            this->GzipEncode();
        #endif
            
            //MakeDefaultResponse(status, *this);
        }

        template <typename StringT>
        inline void RenderString(StringT&& str) {
            this->SetMessageBody_(
                std::forward<StringT>(str)
            );
            
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

        std::vector<boost::asio::const_buffer> ToBuffers() {
            std::vector<boost::asio::const_buffer> buffers;

            buffers.emplace_back(ToBuffer(this->status_code));

            for (const auto& header : this->headers) {
                buffers.emplace_back(boost::asio::buffer(header.field));
                buffers.emplace_back(boost::asio::buffer(NameValueSeparator));
                buffers.emplace_back(boost::asio::buffer(header.value));
                buffers.emplace_back(boost::asio::buffer(CRLF));
            }
            buffers.emplace_back(boost::asio::buffer(CRLF));
            buffers.emplace_back(boost::asio::buffer(this->message_body));
            
            // std::stringstream ss;
            // ss  << "HTTP/1" << this->http_major_version << "." << this->http_minor_version 
            //     << " " << static_cast<int>(this->status_code)
            //     << " " << GetHTTPReasonPhrase(this->status_code) << GetCRLF();
            // buffers.push_back(boost::asio::buffer(ss.str()));
            
            // for (const auto& header : this->headers) {
            //     ss.clear();
            //     ss << header.field << ": " << header.value << GetCRLF();
            //     buffers.emplace_back(
            //         boost::asio::buffer(ss.str())
            //     );
            // }
            // buffers.push_back(boost::asio::buffer(GetCRLF()));
            // buffers.push_back(boost::asio::buffer(this->message_body));

            return buffers;
        }

        void WriteBuffer(std::string& str) {
            std::stringstream ss;
            ss << "HTTP/" << this->http_major_version << "." << this->http_minor_version
               << " " << static_cast<int>(this->status_code) << " " << GetHTTPReasonPhrase(this->status_code) << CRLF;
            for (const auto& header : this->headers) {
                ss << header.field << ": " << header.value << CRLF;
            }
            ss << CRLF << this->message_body;
            // this->AddHeader({"Content-Length", boost::lexical_cast<std::string>(this->message_body.length())});
            str.assign(std::move(ss.str()));
        }

    #ifdef HTTPC_ENABLE_GZIP
        void WriteGzipBuffer(std::string& str) {
            std::stringstream ss;
            ss << "HTTP/" << this->http_major_version << "." << this->http_minor_version
               << " " << static_cast<int>(this->status_code) << " " << GetHTTPReasonPhrase(this->status_code) << GetCRLF();
            for (const auto& header : this->headers) {
                ss << header.field << ": " << header.value << GetCRLF();
            }
            this->GzipEncode();
            ss << GetCRLF() << GetGzipEncode();
            str.assign(std::move(ss.str()));
        }
    #endif

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
        mutable bool    async_state_flag_{ false };
        std::map<std::string, std::string>
                        variable_table_;
    #ifdef HTTPC_ENABLE_GZIP    
        std::string     gzip_message_body_;
    #endif

        inline void SetContentLength_() {
            for (auto itr = this->headers.begin(); itr != this->headers.end(); ++itr) {
                if (itr->field == "Content-Length") {
                    itr->value.assign(boost::lexical_cast<std::string>(this->message_body.length()));
                    return;
                }
            }
            this->headers.emplace_back(
                Header{
                    "Content-Length", 
                    boost::lexical_cast<std::string>(this->message_body.length())
                }
            );
        }

        template <typename StringT>
        inline void SetMessageBody_(StringT&& str) {
            this->message_body.assign(
                std::forward<StringT>(str)
            );
            this->SetContentLength_();
        #ifdef HTTPC_ENABLE_GZIP
            this->GzipEncode();
        #endif
            //std::cout << "SetMessageBody_" << std::endl;
            //std::cout << this->message_body << std::endl;
        }

    };


}