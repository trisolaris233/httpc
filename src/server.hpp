#pragma once

#include <regex>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "connection_manager.hpp"

namespace httpc {

    class Server {
    public:
        Server(
            const std::string& address,
            const std::string& port,
            const std::string& document_root
        )
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
            // thread_.reset(new std::thread(
            //     [this] {
            //         io_context_.run();
            //     }
            // ));
            // // should be tested before use.
            // // but that means I should make a exception class
            // // I am lazy...
            // thread_->detach();

            // use std::async instead
            std::async(std::launch::async, [this] {
                io_context_.run(); 
            });
        }


        template <HttpMethodEnum... Method, typename Function, typename... Args>
        void AddRouter(
            const std::string& route, 
            Function&& callback,
            Args&&... aspects
        ) {
            manager_.RegisterRouter<Method...>(
                route,
                std::forward<Function>(callback),
                std::move(aspects)...
            );
            // manager_.RegisterRouter(
            //             route,
            //             std::forward<Function>(callback),
            //             std::move(aspects)...
            //         );
        }

        template <HttpMethodEnum... Method, typename Function, typename... Args>
        void AddRouter(
            std::regex&& route, 
            Function&& callback,
            Args&&... aspects
        ) {
            manager_.RegisterRouter<Method...>(
                std::move(route),
                std::forward<Function>(callback),
                std::move(aspects)...
            );
            // manager_.RegisterRouter(
            //             route,
            //             std::forward<Function>(callback),
            //             std::move(aspects)...
            //         );
        }
        
        

    private:
        boost::asio::io_context io_context_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::string document_root_;
        ConnectionManager manager_;
        // std::shared_ptr<std::thread> 
        //                     thread_;


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

} // httpc