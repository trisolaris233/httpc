#pragma once

#include <string>
#include <string_view>
#include <boost/asio/buffer.hpp>

namespace httpc {
    enum class HTTPStatusCodeEnum {
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

    inline std::string_view Ok = "OK";
	inline std::string_view Created = "<html>"
		"<head><title>Created</title></head>"
		"<body><h1>201 Created</h1></body>"
		"</html>";

	inline std::string_view Accepted =
		"<html>"
		"<head><title>Accepted</title></head>"
		"<body><h1>202 Accepted</h1></body>"
		"</html>";

	inline std::string_view NoContent =
		"<html>"
		"<head><title>No Content</title></head>"
		"<body><h1>204 Content</h1></body>"
		"</html>";

	inline std::string_view MultipleChoices =
		"<html>"
		"<head><title>Multiple Choices</title></head>"
		"<body><h1>300 Multiple Choices</h1></body>"
		"</html>";

	inline std::string_view MovedPermanently =
		"<html>"
		"<head><title>Moved Permanently</title></head>"
		"<body><h1>301 Moved Permanently</h1></body>"
		"</html>";

	inline std::string_view MovedTemporarily =
		"<html>"
		"<head><title>Moved Temporarily</title></head>"
		"<body><h1>302 Moved Temporarily</h1></body>"
		"</html>";

	inline std::string_view NotModified =
		"<html>"
		"<head><title>Not Modified</title></head>"
		"<body><h1>304 Not Modified</h1></body>"
		"</html>";

	inline std::string_view BadRequest =
		"<html>"
		"<head><title>Bad Request</title></head>"
		"<body><h1>400 Bad Request</h1></body>"
		"</html>";

	inline std::string_view Unauthorized =
		"<html>"
		"<head><title>Unauthorized</title></head>"
		"<body><h1>401 Unauthorized</h1></body>"
		"</html>";

	inline std::string_view Forbidden =
		"<html>"
		"<head><title>Forbidden</title></head>"
		"<body><h1>403 Forbidden</h1></body>"
		"</html>";

	inline std::string_view Notfound =
		"<html>"
		"<head><title>Not Found</title></head>"
		"<body><h1>404 Not Found</h1></body>"
		"</html>";

	inline std::string_view InternalServerError =
		"<html>"
		"<head><title>Internal Server Error</title></head>"
		"<body><h1>500 Internal Server Error</h1></body>"
		"</html>";

	inline std::string_view NotImplemented =
		"<html>"
		"<head><title>Not Implemented</title></head>"
		"<body><h1>501 Not Implemented</h1></body>"
		"</html>";

	inline std::string_view BadGateway =
		"<html>"
		"<head><title>Bad Gateway</title></head>"
		"<body><h1>502 Bad Gateway</h1></body>"
		"</html>";

	inline std::string_view ServiceUnavailable =
		"<html>"
		"<head><title>Service Unavailable</title></head>"
		"<body><h1>503 Service Unavailable</h1></body>"
		"</html>";

	inline std::string_view ResSwitchingProtocols = "HTTP/1.1 101 Switching Protocals\r\n";
	inline std::string_view ResOk = "HTTP/1.1 200 OK\r\n";
	inline std::string_view ResCreated = "HTTP/1.1 201 Created\r\n";
	inline std::string_view ResAccepted = "HTTP/1.1 202 Accepted\r\n";
	inline std::string_view ResNoContent = "HTTP/1.1 204 No Content\r\n";
	inline std::string_view ResPartialContent = "HTTP/1.1 206 Partial Content\r\n";
	inline std::string_view ResMultipleChoices = "HTTP/1.1 300 Multiple Choices\r\n";
	inline std::string_view ResMovedPermanently =	"HTTP/1.1 301 Moved Permanently\r\n";
	inline std::string_view ResMovedTemporarily =	"HTTP/1.1 302 Moved Temporarily\r\n";
	inline std::string_view ResNotModified = "HTTP/1.1 304 Not Modified\r\n";
	inline std::string_view ResBadRequest = "HTTP/1.1 400 Bad Request\r\n";
	inline std::string_view ResUnauthorized = "HTTP/1.1 401 Unauthorized\r\n";
	inline std::string_view ResForbidden =	"HTTP/1.1 403 Forbidden\r\n";
	inline std::string_view ResNotFound =	"HTTP/1.1 404 Not Found\r\n";
	inline std::string_view ResInternalServerError = "HTTP/1.1 500 Internal Server Error\r\n";
	inline std::string_view ResNotImplemented = "HTTP/1.1 501 Not Implemented\r\n";
	inline std::string_view ResBadGateway = "HTTP/1.1 502 Bad Gateway\r\n";
	inline std::string_view ResServiceUnavailable = "HTTP/1.1 503 Service Unavailable\r\n";

	inline const char NameValueSeparator[] = { ':', ' ' };
	//inline std::string_view crlf = "\r\n";

	inline const char CRLF[] = { '\r', '\n' };
	inline const char LastChunk[] = { '0', '\r', '\n' };
	// inline const std::string http_chunk_header =
	// 	"HTTP/1.1 200 OK\r\n"
	// 	"Transfer-Encoding: chunked\r\n";
	// 	/*"Content-Type: video/mp4\r\n"
	// 	"\r\n";*/

	// inline const std::string http_range_chunk_header =
	// 			"HTTP/1.1 206 Partial Content\r\n"
	// 			"Transfer-Encoding: chunked\r\n";
	// 			/*"Content-Type: video/mp4\r\n"
	// 			"\r\n";*/

    inline boost::asio::const_buffer ToBuffer(HTTPStatusCodeEnum status) {
        switch (status) {
        case HTTPStatusCodeEnum::kSwitchingProtocols:
            return boost::asio::buffer(ResSwitchingProtocols.data(), ResSwitchingProtocols.length());
        case HTTPStatusCodeEnum::kOk:
            return boost::asio::buffer(ResOk.data(), ResOk.length());
        case HTTPStatusCodeEnum::kCreated:
            return boost::asio::buffer(ResCreated.data(), ResCreated.length());
        case HTTPStatusCodeEnum::kAccepted:
            return boost::asio::buffer(ResAccepted.data(), ResAccepted.length());
        case HTTPStatusCodeEnum::kNoContent:
            return boost::asio::buffer(ResNoContent.data(), ResNoContent.length());
        case HTTPStatusCodeEnum::kParticalContent:
            return boost::asio::buffer(ResPartialContent.data(), ResNoContent.length());
        case HTTPStatusCodeEnum::kMultipleChoices:
            return boost::asio::buffer(ResMultipleChoices.data(), ResMultipleChoices.length());
        case HTTPStatusCodeEnum::kMovedPermanently:
            return boost::asio::buffer(ResMovedPermanently.data(), ResMovedPermanently.length());
        case HTTPStatusCodeEnum::kNotModified:
            return boost::asio::buffer(ResNotModified.data(), ResNotModified.length());
        case HTTPStatusCodeEnum::kBadRequest:
            return boost::asio::buffer(ResBadRequest.data(), ResBadRequest.length());
        case HTTPStatusCodeEnum::kUnauthorized:
            return boost::asio::buffer(ResUnauthorized.data(), ResUnauthorized.length());
        case HTTPStatusCodeEnum::kNotFound:
            return boost::asio::buffer(ResNotFound.data(), ResNotFound.length());
        case HTTPStatusCodeEnum::kForbidden:
            return boost::asio::buffer(ResForbidden.data(), ResForbidden.length());
        case HTTPStatusCodeEnum::kInternalServerError:
            return boost::asio::buffer(ResInternalServerError.data(), ResInternalServerError.length());
        case HTTPStatusCodeEnum::kNotImplemented:
            return boost::asio::buffer(ResNotImplemented.data(), ResNotImplemented.length());
        case HTTPStatusCodeEnum::kBadGateway:
            return boost::asio::buffer(ResBadGateway.data(), ResBadGateway.length());
        default:
            return boost::asio::buffer(ResServiceUnavailable.data(), ResServiceUnavailable.length());
        }
    }

    inline std::string_view ToString(HTTPStatusCodeEnum status) {
        switch (status) {
        case HTTPStatusCodeEnum::kOk:
            return Ok;
        case HTTPStatusCodeEnum::kCreated:
            return Created;
        case HTTPStatusCodeEnum::kAccepted:
            return Accepted;
        case HTTPStatusCodeEnum::kNoContent:
            return NoContent;
        case HTTPStatusCodeEnum::kMultipleChoices:
            return MultipleChoices;
        case HTTPStatusCodeEnum::kMovedPermanently:
            return MovedPermanently;
        case HTTPStatusCodeEnum::kNotModified:
            return NotModified;
        case HTTPStatusCodeEnum::kBadRequest:
            return BadRequest;
        case HTTPStatusCodeEnum::kUnauthorized:
            return Unauthorized;
        case HTTPStatusCodeEnum::kForbidden:
            return Forbidden;
        case HTTPStatusCodeEnum::kNotFound:
            return Notfound;
        case HTTPStatusCodeEnum::kInternalServerError:
            return InternalServerError;
        case HTTPStatusCodeEnum::kBadGateway:
            return BadGateway;
        case HTTPStatusCodeEnum::kNotImplemented:
            return NotImplemented;
        case HTTPStatusCodeEnum::kServiceUnavailable:
            return ServiceUnavailable;
        default: 
            return InternalServerError;   
        }
    }

}