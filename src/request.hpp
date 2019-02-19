#include <set>
#include "header.hpp"
#include "response.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <string_view>


namespace httpc {

// the structure of http request
struct Request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<Header> headers;
    std::string message_body;
};


class RequestHandler {
public:
    void Respond(const Request& request, Response& response);

    static bool url_decode(const std::string& in, std::string& out) {
        out.clear();
        out.reserve(in.size());
        for (std::size_t i = 0; i < in.size(); ++i) {
            if (in[i] == '%') {
                if (i + 3 <= in.size()) {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));
                    if (is >> std::hex >> value) {
                        out += static_cast<char>(value);
                        i += 2;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }
            else if (in[i] == '+') {
                out += ' ';
            } else {
                out += in[i];
            }
        }
        return true;
    }

private:

};





} // httpc