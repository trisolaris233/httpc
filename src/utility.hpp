#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>

namespace httpc {
    // the name-value pair 
    struct Header {
        std::string field;
        std::string value;
    };

    enum HttpMethodEnum {
        GET,
        POST
    };

    enum HTTPStatusCodeEnum {
        kContinue = 100,
        kSwitchingProtocols = 101,
        kOk = 200,
        kCreated = 201,
        kAccepted = 202,
        kNonAuthoritativeInfomation = 203,
        kNoContent = 204,
        kResetContent = 205,
        kParticalContent = 206,
        kMultipleChoices = 300,
        kMovedPermanently = 301,
        kFound = 302,
        kSeeOther = 303,
        kNotModified = 304,
        kUseProxy = 305,
        kTemporaryRedirect = 307,
        kBadRequest = 400,
        kUnauthorized = 401,
        kPaymentRequired = 402,
        kForbidden = 403,
        kNotFound = 404,
        kMethodNotAllowed = 405,
        kNotAcceptable = 406,
        kProxyAuthenticationRequired = 407,
        kRequestTimeout = 408,
        kConflict = 409,
        kGone = 410,
        kLengthRequired = 411,
        kPreconditionFailed = 412,
        kRequestEntityTooLarge = 413,
        kRequestURITooLarge = 414,
        kUnsupportedMediaType = 415,
        kRequestedRangeNotSatisfiable = 416,
        kExpectationFailed = 417,
        kInternalServerError = 500,
        kNotImplemented = 501,
        kBadGateway = 502,
        kServiceUnavailable = 503,
        kGatewayTimeout = 504,
        kHTTPVersionNotSupported = 505
    };

    
    std::string GetHTTPReasonPhrase(
                        HTTPStatusCodeEnum status_code) noexcept;

    inline std::string GetCRLF() noexcept {
        return "\r\n";
    }

    template <HttpMethodEnum Method>
    std::string GetMethodStr();

    template <>
    std::string GetMethodStr<HttpMethodEnum::GET>();

    template <>
    std::string GetMethodStr<HttpMethodEnum::POST>();
    
    template <HttpMethodEnum... Methods>
    std::vector<std::string> GetMethodsStr();

    bool is_char(int c);
    bool is_ctl(int c);
    bool is_tspecial(int c);

    const std::vector<int>& GetHttpVersionSupported();
    const std::vector<std::string>& GetHttpMethodSupported();
    const std::vector<HTTPStatusCodeEnum>& GetAllHttpStatus();

} // httpc

#endif