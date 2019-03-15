#pragma once

#define HTTPC_DEBUG

#include <string>
#include <vector>
#include <memory>
#ifdef HTTPC_DEBUG
#include <iostream>
#endif
#include <functional>
#include <boost/lexical_cast.hpp>

#if defined(__gnu_linux__) || defined(sun) || defined(__sun) || defined(_AIX)
#include <sys/sysinfo.h>   
#elif defined(WIN32)
#include <windows.h>
#endif

#include "response_cv.hpp"

namespace httpc {
    // the name-value pair 
    struct Header {
        std::string field;
        std::string value;
        
        Header() = default;

        Header(std::string const& f, std::string const& v) :
            field(f), value(v) 
        {}

    };

    enum class HttpMethodEnum {
        GET,
        POST
    };
    
    std::string GetHTTPReasonPhrase(
                        HTTPStatusCodeEnum status_code) noexcept;

    // decltype(auto) GetCRLF() noexcept {
    //     static const char* crlf = "\r\n";
    //     return crlf;
    // }

    template <HttpMethodEnum Method>
    std::string GetMethodStr() noexcept;

    template <>
    std::string GetMethodStr<HttpMethodEnum::GET>() noexcept;

    template <>
    std::string GetMethodStr<HttpMethodEnum::POST>() noexcept;
    
    template <HttpMethodEnum... Methods>
    std::vector<std::string> GetMethodsStr() noexcept;

    bool is_char(int c) noexcept;
    bool is_ctl(int c) noexcept;
    bool is_tspecial(int c) noexcept;

    const std::vector<int>& GetHttpVersionSupported() noexcept;
    const std::vector<std::string>& GetHttpMethodSupported() noexcept;
    const std::vector<HTTPStatusCodeEnum>& GetAllHttpStatus() noexcept;

    std::size_t GetCPUCoreNumber() noexcept;
    constexpr int Http11() {
        return 11;
    }

    // bind a member function into a general function
    template <typename ResultT, typename ClassT, typename... Args>
    std::function<ResultT(Args...)> BindMember(std::shared_ptr<ClassT> obj, ResultT(ClassT::*method)(Args...)) {
        return [=](Args&&... args) -> ResultT {
            auto ptr = obj.get();
            return (ptr->*method)(std::forward<Args>(args)...);
        };
    }


} // httpc
