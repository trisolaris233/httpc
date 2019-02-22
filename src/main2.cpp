#include "connection.hpp"
#include "connection_manager.hpp"
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include "noncopyable.hpp"
#include "request_parser.hpp"
#include "http_router.hpp"
#include "request_handler.hpp"

using httpc::Request;
using httpc::Response;

struct logf2 {
    void before(Request& q, Response& p) {
        std::cout << "start log" << std::endl;
    }
    void after(Request& q, Response& p) {
        std::cout << "end log" << std::endl;
    }
};

struct logfa {
    void before(Request&, Response&) {
        std::cout << "start log2" << std::endl;
    }
    void after(Request&, Response&) {
        std::cout << "end log2" << std::endl;
    }
};

class testA {
public:
    void test(Request&, Response&) {
        std::cout << "router /prefix/postfix/*" << std::endl;
    }
};

int main() {
    httpc::ConnectionManager manager;
    httpc::HttpRouter router(manager);
    router.RegisterRouter<
        httpc::HttpMethodEnum::GET, 
        httpc::HttpMethodEnum::POST
    >("/", [](Request&, Response&){
        std::cout << "route /" << std::endl;
    });

    router.RegisterRouter("/index?", [](Request&, Response&){
        std::cout << "route /index?" << std::endl;
    });

    router.RegisterRouter("/index/*", [](Request&, Response&) {
        std::cout << "router /index/*" << std::endl;
    });
    testA a;

    router.RegisterRouter(
        "/prefix/postfix/*",
        std::function<void(Request&, Response&)>(
            std::bind(&testA::test, &a, std::placeholders::_1, std::placeholders::_2)
        )
    );
    {
        Request req;
        Response response;
        
        
        std::cout << "GET/ -> ";
        router.Route("GET","/", req, response);
        std::cout << std::endl;
        std::cout << "POST/ -> ";
        router.Route("POST", "/", req, response);
        std::cout << std::endl;
        std::cout << "GET/index -> "; 
        router.Route("GET", "/index", req, response);
        std::cout << std::endl;
        std::cout << "GET/index/ -> "; 
        router.Route("GET", "/index/", req, response);
        std::cout << std::endl;
        std::cout << "GET/index/dsf -> "; 
        router.Route("GET", "/index/dsf", req, response);
        std::cout << std::endl;
        std::cout << "GET/index/234/dsf -> "; 
        router.Route("GET", "/index/234/dsf", req, response);
        std::cout << std::endl;
        std::cout << "GET/index/324/-> "; 
        router.Route("GET", "/index/324/", req, response);
        std::cout << std::endl;
        std::cout << "GET/prefix/postfix/ -> "; 
        router.Route("GET", "/prefix/postfix/", req, response);
        std::cout << std::endl;
        std::cout << "GET/prefix/postfix/woc -> "; 
        router.Route("GET", "/prefix/postfix/woc", req, response);
        std::cout << std::endl;
        std::cout << "GET/prefix/postfix/fdsf/ -> "; 
        router.Route("GET", "/prefix/postfix/fdsf/", req, response);
        std::cout << std::endl;

    }
    return 0;
}