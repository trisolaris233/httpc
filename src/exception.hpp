#pragma once

#include <string>
#include <exception>

namespace httpc {

    class HttpcExcepetion : public std::exception {
    };

    class HttpSocketError : public HttpcExcepetion {
    public:
        HttpSocketError(const std::string& str) :
            err_msg_(str) {}
        HttpSocketError(std::string&& str) :
            err_msg_(std::move(str)) {}
        HttpSocketError(const char* str) :
            err_msg_(str) {}

        virtual inline const char* what() const noexcept {
            return err_msg_.c_str();
        }

    private:
        std::string err_msg_;

    };


}