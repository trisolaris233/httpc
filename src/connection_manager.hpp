#pragma once

#include <map>
#include <set>
#include <tuple>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include "connection.hpp"
#include "noncopyable.hpp"

namespace httpc {

    class ConnectionManager : Noncopyable {
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

        void CloseConnection(std::shared_ptr<Connection> ptr_conn) {
            // close connection
            connections_.erase(ptr_conn);
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
            auto methods = GetMethodsStr<Methods...>();
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
}