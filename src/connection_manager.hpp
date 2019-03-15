#pragma once

#include <set>
#include <atomic>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include "debug.hpp"
#include "connection.hpp"
#include "noncopyable.hpp"
#include "http_router.hpp"

namespace httpc {

    class ConnectionManager : public Noncopyable {
    public:
        explicit ConnectionManager(boost::asio::io_context& io_context) noexcept :
            io_context_(io_context)
            , router_(*this) {
        }
        void    AddConnection(boost::asio::ip::tcp::socket&& socket);
        void    CloseAllConnection();
        void    CloseConnection(std::shared_ptr<Connection> ptr_conn) {
            // close connection
            debug().dg("enter close").lf();
            if (!ptr_conn->IsSocketOpen()) {
                debug().dg("frees a connection").lf();
                this->connections_.erase(ptr_conn);
            } else {
            }
            
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
                std::forward<Aspects>(aspects)...
                // std::move(aspects)...
            );
        }
        template <HttpMethodEnum... Methods, typename Function, typename... Aspects>
        void RegisterRouter(
            std::regex&& route,
            Function&& callback,
            Aspects&&... aspects
        ) {
            router_.RegisterRouter<Methods...>(
                // std::move(route), 
                std::forward<std::regex>(route),
                std::forward<Function>(callback), 
                std::forward<Aspects>(aspects)...
                // std::move(aspects)...
            );
        }
        void Route(
            const std::string&  http_method,
            const std::string&  route_directory,
            Request&            request,
            Response&           response
        );

        inline decltype(auto) GetIOContext() noexcept {
            return this->io_context_;
        }

    private:
        std::set<std::shared_ptr<Connection>>   
                                        connections_;
        HttpRouter                      router_;
        boost::asio::io_context&        io_context_;
        // std::atomic<bool>               atomic_mutex_;
        // std::shared_ptr<std::thread>    clean_thread_;


    };
}