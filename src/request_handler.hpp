#pragma once

#include "utility.hpp"
#include "request.hpp"
#include "response.hpp"

namespace httpc {

    class RequestHandler {
    public:

        void Respond(Request& request, Response& response);


    private:
        //Request             request_;

    };


} // httpc