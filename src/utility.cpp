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
    std::string GetMethodStr() {
        return std::string();
    }

    template <>
    std::string GetMethodStr<HttpMethodEnum::GET>() {
        return std::string("GET");
    }

    template <>
    std::string GetMethodStr<HttpMethodEnum::POST>() {
        return std::string("POST");
    }

    bool is_char(int c) {
        return c >= 0 && c <= 127;
    }

    bool is_ctl(int c) {
        return (c >= 0 && c <= 31) || (c == 127);
    }

    bool is_tspecial(int c) {
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

    template <HttpMethodEnum... Methods>
    std::vector<std::string> GetMethodsStr() {
        std::vector<std::string> res;
        (res.push_back((GetMethodStr<Methods>())), ...);
        return res;
    }

    const std::vector<int>& GetHttpVersionSupported() {
        static std::vector<int> HttpVersionSupported = {
            11
        };
        return HttpVersionSupported;
    }

    const std::vector<std::string>& GetHttpMethodSupported() {
        static std::vector<std::string> HttpMethodSupported = {
            "GET", "POST"
        };
        return HttpMethodSupported;
    }

    const std::vector<HTTPStatusCodeEnum>& GetAllHttpStatus() {
        static std::vector<HTTPStatusCodeEnum> AllHTTPStatus = {
            kContinue,
            kSwitchingProtocols,
            kOk,
            kCreated,
            kAccepted,
            kNonAuthoritativeInfomation,
            kNoContent,
            kResetContent,
            kParticalContent,
            kMultipleChoices,
            kMovedPermanently,
            kFound,
            kSeeOther,
            kNotModified,
            kUseProxy,
            kTemporaryRedirect,
            kBadRequest,
            kUnauthorized,
            kPaymentRequired,
            kForbidden,
            kNotFound,
            kMethodNotAllowed,
            kNotAcceptable,
            kProxyAuthenticationRequired,
            kRequestTimeout,
            kConflict,
            kGone,
            kLengthRequired,
            kPreconditionFailed,
            kRequestEntityTooLarge,
            kRequestURITooLarge,
            kUnsupportedMediaType,
            kRequestedRangeNotSatisfiable,
            kExpectationFailed,
            kInternalServerError,
            kNotImplemented,
            kBadGateway,
            kServiceUnavailable,
            kGatewayTimeout,
            kHTTPVersionNotSupported
        };

        return AllHTTPStatus;
    }

} // httpc