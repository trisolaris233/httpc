#pragma once 

#include <map>
#include <regex>
#include <string>
#include <utility>
#include <functional>
#include "utility.hpp"
#include "request.hpp"
#include "response.hpp"
#include "connection_manager.hpp"

namespace httpc {

    class HttpRouter {
    public:
        typedef std::function<void(Request&, Response&)> CallBackFunctionType;
        
        explicit HttpRouter(ConnectionManager& manager) :
            manager_(manager) { }

        // the rules of registering general routers:
        // 1. router must be started with '/', or registeration will fail
        // 2. router which has a char of '.' will be considered as static file
        // 3. /demo and /demo/ are different routers, 
        // /demo can only route /demo but /demo? can route both /demo and /demo/
        // 4. RegisterRouter method that takes a string can lead a fezzy router
        // such as /prefix/*. that matches /prefix/a and /prefix/aaa but doesnt 
        // match /prefix/a/b. To match it you need to pass /prefix/a/* or /prefix/*/*
        // but like /prefix* that you want to matches /prefixa or /prefixb is not work.
        // or you should use regex version.
        // 5. if you want to lead a stronger router, please use RegisterRouter which
        // takes a parameter of std::regex and use the regular expression to do your
        // routers.
        // 6. /prefix/* matches /prefix/fdfd but doesnt match /prefix
        template <HttpMethodEnum... Methods, typename Function, typename... Aspects>
        void RegisterRouter(
            const std::string&  router_directory,
            Function&&          callback,
            Aspects&&...        aspects 
        ) {
            // if has at least one http method.
            if constexpr(sizeof...(Methods) > 0) {
                auto methods = GetMethodsStr<Methods...>();
                
                for (auto& method : methods) {
                    // register router for every method
                    RegisterRouterImpl_(
                        method,
                        router_directory, 
                        std::forward<Function>(callback), 
                        std::move(aspects)...
                    );
                }
            } else {    // has no http method.
                RegisterRouterImpl_(
                    GetMethodStr<HttpMethodEnum::GET>(),
                    router_directory,
                    std::forward<Function>(callback),
                    std::move(aspects)...
                );
            }
        }

        template <HttpMethodEnum... Methods, typename Function, typename... Aspects>
        void RegisterRouter(
            std::regex&&    router_directory,
            Function&&      callback,
            Aspects&&...    aspects
        ) {
            if constexpr(sizeof...(Methods) > 0) {
                auto methods = GetMethodsStr<Methods...>();

                for (auto& method : methods) {
                    RegisterRouterImpl_(
                        method,
                        router_directory,
                        std::forward<Function>(callback),
                        std::move(aspects)...
                    );
                }
            } else {
                RegisterRouterImpl_(
                    GetMethodStr<HttpMethodEnum::GET>(),
                    router_directory,
                    std::forward<Function>(callback),
                    std::move(aspects)...
                );
            }
        }

        template <typename Function, typename... Aspects>
        void Invoke(
            Request&            request,
            Response&           response,
            Function            callback,
            Aspects...          aspects
        ) {
            // get all the aspects objects into a tuple
            std::tuple<Aspects...> aspects_tuple(std::move(aspects)...);
            // go through for every aspect and call the before() function
            std::apply([&request, &response](auto&&... aspect) {
                (aspect.before(request, response), ...);
            }, aspects_tuple);

            // do the bussiness
            std::forward<Function>(callback)(request, response);

            // do same for after()
            std::apply([&request, &response](auto&&... aspect) {
                (aspect.after(request, response), ...);
            }, aspects_tuple);
        }

        // search if there's a route that matches the route_directory,
        // if does, call the function that stored in map.
        void Route(
            const std::string& http_method,
            const std::string& route_directory, 
            Request& request, 
            Response& response
        ) {
            bool end_with_slash = (route_directory.back() == '/');
            // try to search directory
            std::string key(http_method + route_directory);
            auto res_itr = directly_router_map_.find(key);
            if (res_itr != directly_router_map_.end()) {
                return res_itr->second(request, response);
            } 


            res_itr = directly_router_map_.find(key + ((end_with_slash) ? "?" : "/?"));            
            if (res_itr != directly_router_map_.end()) {
                res_itr->second(request, response);
            }

            // try fezzy match
            std::size_t num_of_slash = 1;
            for (
                auto pos_of_slash = route_directory.rfind('/');
                pos_of_slash != std::string::npos;
                pos_of_slash = route_directory.rfind('/', pos_of_slash - 1)
            ) {
                std::string tmp(http_method + route_directory.substr(0, pos_of_slash + 1));
                for (auto i = 0; i < num_of_slash; ++i) {
                    tmp.push_back('*');
                    if (i + 1 < num_of_slash) {
                        tmp.push_back('/');
                    }
                }
                
                auto wildcard_itr = wildcard_router_map_.find(tmp);
                if (wildcard_itr != wildcard_router_map_.end()) {
                    return wildcard_itr->second(request, response);
                }

                num_of_slash++;
                
                if (pos_of_slash == 0) 
                    break;
            }
            
            // try regex match
            auto method_regex_list = regex_router_map_[http_method];
            
            for (const auto& reg : method_regex_list) {
                if (std::regex_match(route_directory, reg.first)) {
                    return reg.second(request, response);
                }   
            }
        }
        
    private:
        ConnectionManager& manager_;
        // this router is for directly router
        std::map<std::string, CallBackFunctionType> directly_router_map_;
        // for wildcard router
        std::map<std::string, CallBackFunctionType> wildcard_router_map_;
        // for regular expression
        std::map<std::string,
            std::vector<std::pair<std::regex, CallBackFunctionType>>>
                                                    regex_router_map_;

        
        template <typename Function, typename... Aspect>
        void RegisterRouterImpl_(
            const std::string&  http_method,
            const std::string&  router_directory,
            Function&&          callback,
            Aspect&&...         aspects
        ) {
            if (router_directory.empty() || router_directory[0] != '/') {
                return;
            }
            // if find char of '*' in the router location
            if (router_directory.find('*') != std::string::npos) {
                wildcard_router_map_[http_method + router_directory] = std::bind(
                    &HttpRouter::Invoke<Function, Aspect...>,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::forward<Function>(callback),
                    std::move(aspects)...
                );
            } else {
                directly_router_map_[http_method + router_directory] = std::bind(
                    &HttpRouter::Invoke<Function, Aspect...>,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::forward<Function>(callback),
                    std::move(aspects)...
                );
            }
        }

        template <typename Function, typename... Aspect>
        void RegisterRouterImpl_(
            const std::string&  http_method,
            std::regex&&        router_directory,
            Function&&          callback,
            Aspect&&...         aspects
        ) {
            regex_router_map_[http_method].push_back(
                std::make_pair(
                    std::regex(std::move(router_directory)),
                    std::bind(
                        &HttpRouter::Invoke<Function, Aspect...>,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2,
                        std::forward<Function>(callback),
                        std::move(aspects)...
                    )
                )
            );

        }
    
    };

} // httpc