#pragma once

#include <set>
#include <string>
#include <memory>
#include <functional>
#include "connection.hpp"
#include "noncopyable.hpp"
#include "http_router.hpp"

namespace httpc {

    class ConnectionManager : Noncopyable {
    public:

        ConnectionManager() noexcept :
            router_(*this) {
        }
        void    AddConnection(boost::asio::ip::tcp::socket&& socket);
        void    CloseAllConnection();
        void    CloseConnection(std::shared_ptr<Connection> ptr_conn) {
            // close connection
            connections_.erase(ptr_conn);
        }
        template <HttpMethodEnum... Methods, typename Function, typename... Aspects>
        void RegisterRouter(
            const std::string& route,
            Function&& callback,
            Aspects&&... aspects
        ) {
            router_.RegisterRouter<Methods...>(
                route, 
                std::forward<Function>(callback), 
                std::move(aspects)...
            );
        }
        template <HttpMethodEnum... Methods, typename Function, typename... Aspects>
        void RegisterRouter(
            std::regex&& route,
            Function&& callback,
            Aspects&&... aspects
        ) {
            router_.RegisterRouter<Methods...>(
                std::move(route), 
                std::forward<Function>(callback), 
                std::move(aspects)...
            );
        }
        void Route(
            const std::string&  http_method,
            const std::string&  route_directory,
            Request&            request,
            Response&           response
        );

    private:
        std::set<std::shared_ptr<Connection>>   
                    connections_;
        HttpRouter  router_;
            
    };
}