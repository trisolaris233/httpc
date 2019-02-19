#include <set>
#include <map>
#include <tuple>
#include <string>
#include <array>
#include <cctype>
//#include <thread>
#include <utility>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <future>
#include <type_traits>
#include <functional>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


enum HttpMethodEnum {
    GET,
    POST
};

template <HttpMethodEnum Method>
std::string GetStr() {
    return std::string();
}

template <>
std::string GetStr<HttpMethodEnum::GET>() {
    return std::string("GET");
}

template <>
std::string GetStr<HttpMethodEnum::POST>() {
    return std::string("POST");
}

template <HttpMethodEnum... Methods>
std::vector<std::string> GetMethodInStr() {
    std::vector<std::string> res;
    (res.push_back((GetStr<Methods>())), ...);
    return res;
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

struct Header {
    std::string field;
    std::string value;
};


struct Request {
    std::string method;
    std::string uri;
    int http_major_version{0};
    int http_minor_version{0};
    std::vector<Header> headers;
    std::string message_body;

    std::vector<Header>::iterator HeaderBegin() {
        return headers.begin();
    }
    std::vector<Header>::iterator HeaderEnd() {
        return headers.end();
    }
    std::vector<Header>::const_iterator HeaderCBegin() const {
        return headers.begin();
    }
    std::vector<Header>::const_iterator HeaderCEnd() const {
        return headers.end();
    }
};

struct Response {
    int http_major_version{0};
    int http_minor_version{0};
    int status_code{0};
    std::string reason_phrase;
    std::vector<Header> headers;
    std::string message_body;

    std::vector<boost::asio::const_buffer> ToBuffers() {
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(
            boost::asio::buffer(std::string("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n"))
        );
        // buffers.push_back(boost::asio::buffer(std::string("Content-Type: text/html;\r\n")));
        
        // buffers.push_back(boost::asio::buffer("\r\n"));
        // buffers.push_back(boost::asio::buffer(message_body));
        return buffers;
    }
};

struct URI {
    std::string addr;
    std::map<std::string, std::string> query; 
};

class URIParser {
    enum UriStatusEnum {
        kQueryName,
        kQueryValue,
        kNormalChars
    };

public:
    URIParser(const std::string& uri) :
        uri_(uri) {    
    }

    URI Parse() {
        for (auto x : uri_) {
            ParseURI(x);
        }
        return uri_obj_;
    }

private:
    std::string uri_;
    URI         uri_obj_;
    UriStatusEnum uri_parse_status_{kNormalChars};
    std::string tmp_;

    template <typename CharT>
    int ParseURI(CharT chr) {
        switch(uri_parse_status_) {
            case kNormalChars:
                if (chr == '?') {
                    uri_parse_status_ = kQueryName;
                    return 1;
                } else if (is_ctl(chr)) {
                    return 0;
                } else {
                    uri_obj_.addr.push_back(chr);
                    return 1;
                }

            case kQueryName:
                if (chr == '&') {
                    return 0;
                } else if (chr == '=') {
                    uri_obj_.query.insert(
                        std::move(std::make_pair(std::string(tmp_), std::string()))
                    );
                    uri_parse_status_ = kQueryValue;
                    return 1;
                } else if (is_ctl(chr)) {
                    return 0;
                } else {
                    tmp_.push_back(chr);
                    return 1;
                }
            
            case kQueryValue:
                if (chr == '&') {
                    tmp_.clear();
                    uri_parse_status_ = kQueryName;
                    return 1;
                } else if (is_ctl(chr)) {
                    return 0;
                } else {
                    uri_obj_.query.at(tmp_).push_back(chr);
                }

            default:
                return 0;
                
        }
    }

};


class RequestParser {
    
    enum ParseStatusEnum {
        kStart,
        kHTTPMethod,
        kURI,
        kH,
        kT1,
        kT2,
        kP,
        kSlash,
        kMajorVersion,
        kPointBetweenMajorAndMinor,
        kMinorVersion,
        kFirstLF,
        kHeaderLineStart,
        kHeaderName,
        kHeaderValue,
        kHeaderLws,
        kSpaceBeforeHeaderValue,
        kExceptingNormalNewLine,
        kExceptingNewLineEnding
    };

public:
    enum ResultType {
        kGood,
        kBad,
        kIndeterminate
    };

    RequestParser() {}

    explicit RequestParser(const std::string_view& request) :
        request_str_(request),
        parse_enum_(kStart) {
    }

    template <typename InputIterator>
    std::tuple<ResultType, InputIterator> Parse(Request& request, 
        InputIterator begin, InputIterator end) {
        ResultType res = kBad;
        for (; begin != end; ++begin) {
            if(ParseRequest(request, *begin) == kGood) {
                return std::make_tuple(kGood, begin);
            }

        }
        std::cout << "end of parse" << std::endl;
        return std::make_tuple(kIndeterminate, begin);
        
    }
    

private:
    std::string      request_str_;
    ParseStatusEnum  parse_enum_;
    int              power_{1};
    bool             first_flag_{false};

    int CountSpace(const std::string_view& str) {
        int res = 0;
        for(auto chr : str) {
            if (chr == '\r' || chr == '\n') {
                break; 
            } else if (chr == ' ') {
                ++res;
            }
        }
        return res;
    }
    

    template <typename CharT>
    ResultType ParseRequest(
            Request& request_, CharT chr,
            bool simplified = false) {
        // there is a problem with the original parsing function
        // the case is that when request line is like this "GET HTTP/1.1"
        // is also available on many broswers, but error will occurs on
        // the original one.
        switch(parse_enum_) {
            case kStart:
                if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                    return kBad;
                } else {
                    request_.method.push_back(chr);
                    parse_enum_ = kHTTPMethod;
                    return kIndeterminate;
                }

            case kHTTPMethod:
                if (chr == ' ') {
                    parse_enum_ = (simplified) ? kH : kURI;
                    return kIndeterminate;
                } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                    return kBad;
                } else {
                    request_.method.push_back(chr);
                    return kIndeterminate;
                }

            case kURI:
                if (chr == ' ') {
                    parse_enum_ = kH;
                    return kIndeterminate;
                } else if (is_ctl(chr)) {
                    return kBad;
                } else {
                    request_.uri.push_back(chr);
                    return kIndeterminate;
                }

            case kH:
                if (chr == 'H') {
                    parse_enum_ = kT1;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kT1:
                if (chr == 'T') {
                    parse_enum_ = kT2;
                    return kIndeterminate;
                } else {
                    return kBad;
                }
                
            case kT2:
                if (chr == 'T') {
                    parse_enum_ = kP;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kP:
                if (chr == 'P') {
                    parse_enum_ = kSlash;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kSlash:
                if (chr == '/') {
                    parse_enum_ = kMajorVersion;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kMajorVersion:
                if (chr == '.') {
                    power_ = 1;
                    parse_enum_ = kMinorVersion;
                    return kIndeterminate;
                } else if (std::isdigit(chr)) {
                    request_.http_major_version += power_ * static_cast<int>(chr - 0x30);
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kMinorVersion:
                if (chr == '\r') {
                    parse_enum_ = kFirstLF;
                    return kIndeterminate;
                } else if (std::isdigit(chr)) {
                    request_.http_minor_version += power_ * static_cast<int>(chr - 0x30);
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kFirstLF:
                if (chr == '\n') {
                    parse_enum_ = kHeaderLineStart;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kHeaderLineStart:
                if (chr == '\r') {
                    parse_enum_ = kExceptingNewLineEnding;
                    return kIndeterminate;
                } else if (!request_.headers.empty() && (chr == ' ' || chr == '\t')) {
                    parse_enum_ = kHeaderLws;
                    return kIndeterminate;
                } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                    return kBad;
                } else {
                    request_.headers.push_back(Header());
                    request_.headers.back().field.push_back(chr);
                    parse_enum_ = kHeaderName;
                    return kIndeterminate;
                }

            case kHeaderLws:
                if (chr == '\r') {
                    parse_enum_ = kExceptingNormalNewLine;
                    return kIndeterminate;
                } else if (chr == ' ' || chr == '\t') {
                    return kIndeterminate;
                } else if (is_ctl(chr)) {
                    return kBad;
                } else {
                    parse_enum_ = kHeaderValue;
                    request_.headers.back().value.push_back(chr);
                    return kIndeterminate;;
                }

            case kHeaderName:
                if (chr == ':') {
                    parse_enum_ = kSpaceBeforeHeaderValue;
                    return kIndeterminate;
                } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                    return kBad;
                } else {
                    request_.headers.back().field.push_back(chr);
                    return kIndeterminate;
                }

            case kSpaceBeforeHeaderValue:
                if (chr == ' ') {
                    parse_enum_ = kHeaderValue;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kHeaderValue:
                if (chr == '\r') {
                    parse_enum_ = kExceptingNormalNewLine;
                    return kIndeterminate;
                } else if (!is_char(chr) || is_ctl(chr) || is_tspecial(chr)) {
                    return kBad;
                } else {
                    request_.headers.back().value.push_back(chr);
                    return kIndeterminate;
                }

            case kExceptingNormalNewLine:
                if (chr == '\n') {
                    parse_enum_ = kHeaderLineStart;
                    return kIndeterminate;
                } else {
                    return kBad;
                }

            case kExceptingNewLineEnding:
                return chr == '\n' ? kGood : kBad;
                
            
            default:
                return kBad;
        }
    }

};

class RequestHandler {
public:

    // RequestHandler(const Request& request) :
    //     request_(request)
    //     { }

    void Respond(Request& request, Response& response) {
        response.http_major_version = 1;
        response.http_major_version = 1;
        response.message_body = "<html><body><h1>Fuck You!</h1></body></html>";
        response.reason_phrase = "OK";
        response.status_code = 200;
    }


private:
    Request request_;

};

class ConnectionManager;

class Connection 
    : public std::enable_shared_from_this<Connection> {
public:
    Connection(ConnectionManager& manager, 
        boost::asio::ip::tcp::socket&& socket
    ) 
        : manager_(manager)
        , socket_(std::move(socket)) {
    }

    void Start() {
        DoRead();
    }

    void Close() {
        socket_.close();
    }

private:
    ConnectionManager&              manager_;
    boost::asio::ip::tcp::socket    socket_;
    std::array<char, 512>           buffer_;
    RequestParser                   parser_;
    Request                         request_;
    Response                        response_;
    RequestHandler                  handler_;
    std::string                     send_buffer_;

    void DoRead() {
        auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(buffer_),
            [this, self] (boost::system::error_code ec, std::size_t bytes_transferred) {
                if(!ec) {
                    std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
                    std::cout << buffer_.data() << std::endl;
                    // begin
                    auto res = 
                        parser_.Parse(request_, buffer_.data(),
                             buffer_.data() + bytes_transferred);
                    
                    // if the parsement is complete, then call the request handler to 
                    // make a response.
                    if(std::get<0>(res) == RequestParser::kGood) {
                        std::cout << "good parsement" << std::endl;
                        //handler_.Respond(request_, response_);
                        send_buffer_ = "fsdfjdshfsdjf";
                        DoWrite(); // in this function call the user's function.
                        // after
                    } else if (std::get<0>(res) == RequestParser::kIndeterminate) {
                        std::cout << "kIndeterminate" << std::endl;
                        DoRead();
                    } else {
                        std::cout << "bad request" << std::endl;
                        // bad request
                    }
                } else {
                    std::cerr << ec.message() << std::endl;
                }
            }
        );
    }

    void DoWrite() {
        std::cout << "start writing" << std::endl;
        auto self(shared_from_this());
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(send_buffer_, send_buffer_.length()),
            boost::asio::transfer_at_least(send_buffer_.length()),
            [this, self] (boost::system::error_code ec, std::size_t bytes_transferred) {
                if(!ec) {
                    std::cout << ec.message() << std::endl;
                    std::cout << "write " << bytes_transferred << " bytes" << std::endl;
                    boost::system::error_code ignored_ec;
                    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                        ignored_ec);
                } else {
                    std::cout << "err: " << ec.message() << std::endl;
                }
            });
    }
};

class ConnectionManager {
public:
    using CallBackFunc = std::function<void(Request&, Response&)>;
    using RouteTable = std::map<std::string, CallBackFunc>;
    ConnectionManager() = default;

    void AddConnection(boost::asio::ip::tcp::socket&& socket) {
        auto ptr = std::make_shared<Connection>(*this, std::move(socket));
        connections_.insert(ptr);
        ptr->Start();
    }

    void CloseAllConnection() {
        for(auto conn : connections_) {
            conn->Close();
        }
    }

    template <typename Function, typename... Args>
    void Invoke(
        Request& request,
        Response& response,
        Function callback,
        Args... aspects
    ) {
        // push all aspect objects into a tuple
        std::tuple<Args...> aspects_tuple((std::move(aspects))...);
        // go through every aspect & call the before() function
        std::apply(
            [&request, &response](auto&&... aspect) {
                (aspect.before(request, response), ...);
            }, aspects_tuple);
        // call the bussiness
        std::forward<Function>(callback)(request, response);
        // go through every aspect & call the after() function
        std::apply(
            [&request, &response](auto&&... aspect) {
                (aspect.after(request, response), ...);
            }, aspects_tuple);
    }

    template <HttpMethodEnum... Methods, typename Function, typename... Args>
    void RegisterRouter(
        const std::string& route,
        Function&& callback,
        Args&&... args
    ) {
        auto methods = GetMethodInStr<Methods...>();
        std::string router(route);
        if(router.back() == '*') {

        }
        // go through every method that passed
        for (auto& method : methods) {
            routes_[method][route] = std::bind(
                &ConnectionManager::Invoke<Function, Args...>,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::move(callback),
                std::move(args)...
            );

        }
    }


private:
    std::set<std::shared_ptr<Connection>>   connections_;
    // RouteTable    routes_;
    // std::map<std::string, std::function<void(Request&, Response&)>> invokes_;
    std::map<
        std::string,
        std::map<std::string, std::function<void(Request&, Response&)>>
    > routes_;
        
    

};

class Server {
public:
    Server(
        const std::string& address,
        const std::string& port,
        const std::string& document_root)
    : io_context_(1)
    , acceptor_(io_context_)
    , document_root_(document_root) {
        boost::asio::ip::tcp::resolver resolver(io_context_);
        boost::asio::ip::tcp::endpoint endpoint = 
            *(resolver.resolve(address, port)).begin();
        acceptor_.open(endpoint.protocol());
        acceptor_.bind(endpoint);
        acceptor_.listen();

        Accept();
    }


    void Start() {
        thread_.reset(new std::thread(
            [this] {
                io_context_.run();
            }
        ));
        // should be tested before use.
        // but that means I should make a exception class
        // I am lazy...
        thread_->detach();

        // use std::async instead
        // auto s = std::async(std::launch::async,
        //             [this]{
        //                io_context_.run(); 
        //             });
    }


    template <HttpMethodEnum... Method, typename Function, typename... Args>
    void AddRouter(
        const std::string& route, 
        Function&& callback,
        Args&&... aspects
    ) {
        manager_.RegisterRouter(
                    route,
                    std::forward<Function>(callback),
                    std::move(aspects)...
                );
    }
    

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string document_root_;
    ConnectionManager manager_;
    std::shared_ptr<std::thread> 
                        thread_;


    void Accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
                // pass it to connection_manager
                if(!ec) {
                    std::cout << "add Connection" << std::endl;
                    manager_.AddConnection(std::move(socket));
                } else {
                    std::cout << "err: " << ec.message() << std::endl;
                }
                Accept();
            }
        );
    }
};



int main(int argc, char* argv[]) {
    
    Server server("127.0.0.1", boost::lexical_cast<std::string>(argv[1]), "/home/sakakiyukiho/www");
    server.Start();
    for(;;) {}

    return 0;
}