#include "utility.hpp"

namespace httpc {
    std::string GetHTTPReasonPhrase(
                        HTTPStatusCodeEnum status_code) noexcept {
        switch (status_code) {
        case HTTPStatusCodeEnum::kContinue:
            return "Continue";
        case HTTPStatusCodeEnum::kSwitchingProtocols:
            return "Switching Protocols";
        case HTTPStatusCodeEnum::kOk:
            return "OK";
        case HTTPStatusCodeEnum::kCreated:
            return "Created";
        case HTTPStatusCodeEnum::kNonAuthoritativeInfomation:
            return "Non-Authoritative Infomation";
        case HTTPStatusCodeEnum::kNoContent:
            return "No Content";
        case HTTPStatusCodeEnum::kResetContent:
            return "Reset Content";
        case HTTPStatusCodeEnum::kParticalContent:
            return "Partical Content";
        case HTTPStatusCodeEnum::kMultipleChoices:
            return "Multiple Choices";
        case HTTPStatusCodeEnum::kMovedPermanently:
            return "Moved Permanently";
        case HTTPStatusCodeEnum::kFound:
            return "Found";
        case HTTPStatusCodeEnum::kSeeOther:
            return "See Other";
        case HTTPStatusCodeEnum::kNotModified:
            return "Not Modified";
        case HTTPStatusCodeEnum::kUseProxy:
            return "Use Proxy";
        case HTTPStatusCodeEnum::kTemporaryRedirect:
            return "Temporary Redirect";
        case HTTPStatusCodeEnum::kBadRequest:
            return "Bad Request";
        case HTTPStatusCodeEnum::kUnauthorized:
            return "Unauthorized";
        case HTTPStatusCodeEnum::kPaymentRequired:
            return "Payment Required";
        case HTTPStatusCodeEnum::kForbidden:
            return "Forbidden";
        case HTTPStatusCodeEnum::kNotFound:
            return "Not Found";
        case HTTPStatusCodeEnum::kMethodNotAllowed:
            return "Method Not Allowed";
        case HTTPStatusCodeEnum::kNotAcceptable:
            return "Not Acceptable";
        case HTTPStatusCodeEnum::kProxyAuthenticationRequired:
            return "Proxy Authentication Required";
        case HTTPStatusCodeEnum::kRequestTimeout:
            return "Request Time-out";
        case HTTPStatusCodeEnum::kConflict:
            return "Conflict";
        case HTTPStatusCodeEnum::kGone:
            return "Gone";
        case HTTPStatusCodeEnum::kLengthRequired:
            return "Length Required";
        case HTTPStatusCodeEnum::kPreconditionFailed:
            return "Precondition Failed";
        case HTTPStatusCodeEnum::kRequestEntityTooLarge:
            return "Request Entity Too Large";
        case HTTPStatusCodeEnum::kRequestURITooLarge:
            return "Request-URI Too Large";
        case HTTPStatusCodeEnum::kUnsupportedMediaType:
            return "Unsupported Media Type";
        case HTTPStatusCodeEnum::kRequestedRangeNotSatisfiable:
            return "Requested range not satisfiable";
        case HTTPStatusCodeEnum::kExpectationFailed:
            return "Expection Failed";
        case HTTPStatusCodeEnum::kInternalServerError:
            return "Internal Server Error";
        case HTTPStatusCodeEnum::kNotImplemented:
            return "Not Implemented";
        case HTTPStatusCodeEnum::kBadGateway:
            return "Bad Gateway";
        case HTTPStatusCodeEnum::kServiceUnavailable:
            return "Service Unavailable";
        case HTTPStatusCodeEnum::kGatewayTimeout:
            return "Gateway Time-out";
        case HTTPStatusCodeEnum::kHTTPVersionNotSupported:
            return "HTTP Version not supported";
        default:
            return "Unknown♂Case";
        }
        
    }

    template <HttpMethodEnum Method>
    std::string GetMethodStr() noexcept {
        return std::string();
    }

    template <>
    std::string GetMethodStr<HttpMethodEnum::GET>() noexcept {
        return std::string("GET");
    }

    template <>
    std::string GetMethodStr<HttpMethodEnum::POST>() noexcept {
        return std::string("POST");
    }

    template <HttpMethodEnum... Methods>
    std::vector<std::string> GetMethodsStr() noexcept {
        std::vector<std::string> res;
        (res.emplace_back((GetMethodStr<Methods>())), ...);
        return res;
    }

    bool is_char(int c) noexcept {
        return c >= 0 && c <= 127;
    }

    bool is_ctl(int c) noexcept {
        return (c >= 0 && c <= 31) || (c == 127);
    }

    bool is_tspecial(int c) noexcept {
        switch (c)
        {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t':
            return true;
        default:
            return false;
        }
    }

    const std::vector<int>& GetHttpVersionSupported() noexcept {
        static std::vector<int> HttpVersionSupported = {
            10, 11
        };
        return HttpVersionSupported;
    }

    const std::vector<std::string>& GetHttpMethodSupported() noexcept {
        static std::vector<std::string> HttpMethodSupported = {
            "GET", "POST"
        };
        return HttpMethodSupported;
    }

    const std::vector<HTTPStatusCodeEnum>& GetAllHttpStatus() noexcept {
        static std::vector<HTTPStatusCodeEnum> AllHTTPStatus = {
            HTTPStatusCodeEnum::kContinue,
            HTTPStatusCodeEnum::kSwitchingProtocols,
            HTTPStatusCodeEnum::kOk,
            HTTPStatusCodeEnum::kCreated,
            HTTPStatusCodeEnum::kAccepted,
            HTTPStatusCodeEnum::kNonAuthoritativeInfomation,
            HTTPStatusCodeEnum::kNoContent,
            HTTPStatusCodeEnum::kResetContent,
            HTTPStatusCodeEnum::kParticalContent,
            HTTPStatusCodeEnum::kMultipleChoices,
            HTTPStatusCodeEnum::kMovedPermanently,
            HTTPStatusCodeEnum::kFound,
            HTTPStatusCodeEnum::kSeeOther,
            HTTPStatusCodeEnum::kNotModified,
            HTTPStatusCodeEnum::kUseProxy,
            HTTPStatusCodeEnum::kTemporaryRedirect,
            HTTPStatusCodeEnum::kBadRequest,
            HTTPStatusCodeEnum::kUnauthorized,
            HTTPStatusCodeEnum::kPaymentRequired,
            HTTPStatusCodeEnum::kForbidden,
            HTTPStatusCodeEnum::kNotFound,
            HTTPStatusCodeEnum::kMethodNotAllowed,
            HTTPStatusCodeEnum::kNotAcceptable,
            HTTPStatusCodeEnum::kProxyAuthenticationRequired,
            HTTPStatusCodeEnum::kRequestTimeout,
            HTTPStatusCodeEnum::kConflict,
            HTTPStatusCodeEnum::kGone,
            HTTPStatusCodeEnum::kLengthRequired,
            HTTPStatusCodeEnum::kPreconditionFailed,
            HTTPStatusCodeEnum::kRequestEntityTooLarge,
            HTTPStatusCodeEnum::kRequestURITooLarge,
            HTTPStatusCodeEnum::kUnsupportedMediaType,
            HTTPStatusCodeEnum::kRequestedRangeNotSatisfiable,
            HTTPStatusCodeEnum::kExpectationFailed,
            HTTPStatusCodeEnum::kInternalServerError,
            HTTPStatusCodeEnum::kNotImplemented,
            HTTPStatusCodeEnum::kBadGateway,
            HTTPStatusCodeEnum::kServiceUnavailable,
            HTTPStatusCodeEnum::kGatewayTimeout,
            HTTPStatusCodeEnum::kHTTPVersionNotSupported
        };

        return AllHTTPStatus;
    }

    std::size_t GetCPUCoreNumber() noexcept
    {
        #if defined(WIN32)
            SYSTEM_INFO info;
            GetSystemInfo(&info);
            return info.dwNumberOfProcessors;
        #elif defined(__gnu_linux__) || defined(sun) || defined(__sun) || defined(_AIX)
            //#include <sys/sysinfo.h>
            return get_nprocs();
        #else
        #error unsupported os
        #endif
    }

} // httpc