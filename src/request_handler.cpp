#include "utility.hpp"
#include "request_handler.hpp"

namespace httpc {
    
    void RequestHandler::Respond(Request& request, Response& response) {
        // check whether http version is supported
        auto HttpVersionSupported = GetHttpVersionSupported();
        if (std::find(
            HttpVersionSupported.begin(), 
            HttpVersionSupported.end(), 
            request.http_major_version * 10 + request.http_minor_version
        ) == HttpVersionSupported.end()) {
            return;
        }
        
        // check whether the http method is supported
        auto HttpMethodSupported = GetHttpMethodSupported();
        if (std::find(
            HttpMethodSupported.begin(),
            HttpMethodSupported.end(), 
            request.method
        ) == HttpMethodSupported.end()) {
            // MakeDefaultResponse(
            //     HTTPStatusCodeEnum::kMethodNotAllowed,
            //     response
            // );
            return;
        }

        response.http_major_version = response.http_minor_version = 1;
        response.status_code = HTTPStatusCodeEnum::kOk;
        response.reason_phrase = GetHTTPReasonPhrase(response.status_code);
    }
} // httpc