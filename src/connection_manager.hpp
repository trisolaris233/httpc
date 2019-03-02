#pragma once

#include <set>
#include <atomic>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include "connection.hpp"
#include "noncopyable.hpp"
#include "http_router.hpp"

namespace httpc {

    class ConnectionManager : public Noncopyable {
    public:

        ConnectionManager() noexcept :
            router_(*this),
            atomic_mutex_(false) {
            AssignCleanTaskThread_();
            this->clean_thread_->detach();
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

    private:
        std::set<std::shared_ptr<Connection>>   
                                        connections_;
        HttpRouter                      router_;
        std::atomic<bool>               atomic_mutex_;
        std::shared_ptr<std::thread>    clean_thread_;

        
        void AssignCleanTaskThread_() {
            clean_thread_ = std::make_shared<std::thread>([this](){
                for (auto itr = this->connections_.begin(); itr != this->connections_.end(); ++itr) {
                    if (!(*itr)->IsSocketOpen()) {
                        this->atomic_mutex_ = true;
                        this->connections_.erase(itr);
                        this->atomic_mutex_ = false;
                    }
                }
                std::this_thread::sleep_for(std::chrono::seconds{5});
            });
        }

    };
}