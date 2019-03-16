
#define HTTPC_ENABLE_GZIP
#include "connection.hpp"
// #include "gzip.hpp"
#include "http_router.hpp"
#include "noncopyable.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"
#include "request.hpp"
#include "response.hpp"
#include "server.hpp"
#include "debug.hpp"
#include "utility.hpp"
//#include "connection_manager.hpp"
#include <iostream>
// #undef HTTPC_DEBUG

using httpc::Request;
using httpc::Response;

// struct logf2 {
//     void before(Request& q, Response& p) {
//         std::cout << "start log" << std::endl;
//     }
//     void after(Request& q, Response& p) {
//         std::cout << "end log" << std::endl;
//     }
// };

// struct logfa {
//     void before(Request&, Response&) {
//         std::cout << "start log2" << std::endl;
//     }
//     void after(Request&, Response&) {
//         std::cout << "end log2" << std::endl;
//     }
// };

// class testA {
// public:
//     void test(Request&, Response&) {
//         std::cout << "router /prefix/postfix/*" << std::endl;
//     }
// };

// void testRouter1() {
//     httpc::ConnectionManager manager;
//     httpc::HttpRouter router(manager);
//     router.RegisterRouter<
//         httpc::HttpMethodEnum::GET, 
//         httpc::HttpMethodEnum::POST
//     >("/", [](Request&, Response&){
//         std::cout << "route /" << std::endl;
//     });

//     router.RegisterRouter("/index?", [](Request&, Response&){
//         std::cout << "route /index?" << std::endl;
//     });

//     router.RegisterRouter("/index/*", [](Request&, Response&) {
//         std::cout << "router /index/*" << std::endl;
//     });
//     testA a;

//     router.RegisterRouter(
//         "/prefix/postfix/*",
//         std::function<void(Request&, Response&)>(
//             std::bind(&testA::test, &a, std::placeholders::_1, std::placeholders::_2)
//         )
//     );
//     {
//         Request req;
//         Response response;
        
        
//         std::cout << "GET/ -> ";
//         router.Route("GET","/", req, response);
//         std::cout << std::endl;
//         std::cout << "POST/ -> ";
//         router.Route("POST", "/", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/index -> "; 
//         router.Route("GET", "/index", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/index/ -> "; 
//         router.Route("GET", "/index/", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/index/dsf -> "; 
//         router.Route("GET", "/index/dsf", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/index/234/dsf -> "; 
//         router.Route("GET", "/index/234/dsf", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/index/324/-> "; 
//         router.Route("GET", "/index/324/", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/prefix/postfix/ -> "; 
//         router.Route("GET", "/prefix/postfix/", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/prefix/postfix/woc -> "; 
//         router.Route("GET", "/prefix/postfix/woc", req, response);
//         std::cout << std::endl;
//         std::cout << "GET/prefix/postfix/fdsf/ -> "; 
//         router.Route("GET", "/prefix/postfix/fdsf/", req, response);
//         std::cout << std::endl;

//     }
// }

// void testRouter2() {
//     httpc::ConnectionManager manager;
//     httpc::HttpRouter router(manager);
//     // router.RegisterRouter(
//     //     std::regex("/fds")
//     // )
//     router.RegisterRouter(
//         std::regex("/index[^/]*"),
//         [](Request&, Response&) {
//             std::cout << "route /index.*" << std::endl;
//         }
//     );

//     router.RegisterRouter(
//         std::regex("/index[0-9]+/pre"),
//         [](Request&, Response&) {
//             std::cout << "route /index[0-9]+/pre" << std::endl;
//         }
//     );

//     router.RegisterRouter(
//         std::regex("/[^/]+/[^/]+/[^/]+/{0,1}"),
//         [](Request&, Response&) {
//             std::cout << "route /[^/]+/[^/]+/[^/]+/{0,1}" << std::endl;
//         }
//     );

//     Request req;
//     Response res;

//     std::cout << "/indexfdsfsdf/fdsfdf -> ";
//     router.Route("GET", "/indexfdsfsdf/fdsfdf", req, res);
//     std::cout << std::endl;
//     std::cout << "/indexfds -> ";
//     router.Route("GET", "/indexfds", req, res);
//     std::cout << std::endl;
//     std::cout << "/index2333/pre -> ";
//     router.Route("GET", "/index2333/pre", req, res);
//     std::cout << std::endl;
//     std::cout << "/fdsf/fdsfd/dsfdfs -> ";
//     router.Route("GET", "/fdsf/fdsfd/dsfdfs", req, res);
//     std::cout << std::endl;
//     std::cout << "/fdsf/fdsafdsf/fdjghz/ -> ";
//     router.Route("GET", "/fdsf/fdsafdsf/fdjghz/", req, res);
//     std::cout << std::endl;
//     std::cout << "/fdsfsdaf/fdsfsd/fdsfsdf/fdsfsd -> ";
//     router.Route("GET", "/fdsfsdaf/fdsfsd/fdsfsdf/fdsfsd", req, res);
//     std::cout << std::endl;
// }

void testDefaultResponse() {
    Response res;
    for (const auto& status_code : httpc::GetAllHttpStatus()) {
        res.SetDefault(status_code);
        std::cout << res << std::endl;
    }
}

int main(int argc, char* argv[]) {

    httpc::Server server(
        "127.0.0.1", 
        std::string(argv[1]), 
        "/home/sakakiyukiho/dev/cpp/httpc/static"
    );
    // constexpr auto GET = httpc::HttpMethodEnum::GET;
    // constexpr auto POST = httpc::HttpMethodEnum::POST;
    server.AddRouter<httpc::HttpMethodEnum::GET, httpc::HttpMethodEnum::POST>
    ("/root", [](Request& req, Response& res) {
        
        httpc::debug().dg("bussiness filesize received => ").dg(req.GetFileSize()).lf();
        if (req.GetFileSize() != 0) {
            // req.DenyNextFile(); // refuse to recv next file
            req.RecvNextFile();
        } else {
            // std::cout << "53?" << std::endl;
            res.RenderString("hello, world");
        }
        if (req.RecvComplete() && req.RecvSuccessfully()) {
            // std::cout << "go render" << std::endl;
            res.RenderString(req.GetFiles()[0].GetContent());
        }
    });
    server.AddRouter("/root/*",[](Request&, Response& res) {
        res.RenderString("hello, root");
    });
    server.AddRouter("/static_file", [&server](Request& req, Response& res) {
        res.RenderFromStaticFile(server.GetDocumentRoot() + "/index.html");
    });

    // server.AddRouter("/", [](Request&, Response& res){
    //     //std::cout << "enter bussiness" << std::endl;
    //     res.RenderString("hello world");
    //     httpc::debug().dg("root").lf();
    // });
    // server.AddRouter("/index/?", [](Request&, Response& res) {
    //     std::cout << "enter /index?" << std::endl;
    //     res.RenderString("This is the index page of router /index?");
    // });
    // server.AddRouter(
    //     std::regex("/[^/]+/[^/]+/[^/]+/{0,1}"),
    //     [](Request&, Response& res) {
    //         std::cout << "enter regex" << std::endl;
    //         res.RenderString("If you see this page that means you've entered /*/*[/] route");
    //     });
    // server.AddRouter("/index.html", [&server](Request&, Response& res) {
    //     res.RenderFromStaticFile(server.GetDocumentRoot() + "/index.html");
    // });
    // server.AddRouter("/index2.html", [&server](Request&, Response& res) {
    //     res.AsyncRenderFromStaticFile(server.GetDocumentRoot() + "/index.html");
    // });

    server.Start();

    for(;;) {}
    // testDefaultResponse();
    return 0;
    
    return 0;
}