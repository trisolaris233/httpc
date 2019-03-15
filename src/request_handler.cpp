#include "debug.hpp"
#include "utility.hpp"
#include "request_handler.hpp"

namespace httpc {
    
    void RequestHandler::Respond(Request& request, Response& response) {
        if (response.Empty()) {
            //std::cout << "empty" << std::endl;
            // check whether http version is supported
            auto HttpVersionSupported = GetHttpVersionSupported();
            if (std::find(
                HttpVersionSupported.begin(), 
                HttpVersionSupported.end(), 
                request.http_major_version * 10 + request.http_minor_version
            ) == HttpVersionSupported.end()) {
                //debug().dg("ver").lf();
                response.SetDefault(HTTPStatusCodeEnum::kHTTPVersionNotSupported);
                return;
            }

            // check whether the http method is supported
            auto HttpMethodSupported = GetHttpMethodSupported();
            if (std::find(
                HttpMethodSupported.begin(),
                HttpMethodSupported.end(), 
                request.method
            ) == HttpMethodSupported.end()) {
                response.SetDefault(HTTPStatusCodeEnum::kMethodNotAllowed);
                return;
            }

            return response.SetDefault(HTTPStatusCodeEnum::kNotFound);   
        }
        
        response.http_major_version = request.http_major_version;
        response.http_minor_version = request.http_minor_version;

        if (response.http_major_version == 1) {
            // http/1.1
            if (response.http_minor_version == 1 || response.http_minor_version == 0) {
                if (response.IsEmptyReasonPhrase() || response.IsEmptyStatusCode()) {
                    response.SetStatusCode(HTTPStatusCodeEnum::kOk);
                }
                if (response.IsEmptyMessageBody()) {
                    response.RenderString(GetHTTPReasonPhrase(response.status_code));
                }
                
                response.SetDefaultHeaders();
            }
        }
    }
} // httpc