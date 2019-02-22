#pragma once

namespace httpc {

    class RequestHandler {
    public:

        // RequestHandler(const Request& request) :
        //     request_(request)
        //     { }

        void Respond(Request& request, Response& response) {
            
        }


    private:
        Request request_;

    };


} // httpc