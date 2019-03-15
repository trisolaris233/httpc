#pragma once

#include <regex>
#include <array>
#include <mutex>
#include <atomic>
#include <future>
#include <thread>
#include <vector>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "utility.hpp"
#include "connection.hpp"
#include "thread_pool.hpp"
#include "http_router.hpp"
//#include "connection_manager.hpp"

namespace httpc {

    class Server {
    public:
        Server(
            const std::string& address,
            const std::string& port,
            const std::string& document_root,
            std::size_t num_thread = GetCPUCoreNumber()
        )
        : io_context_(1)
        , acceptor_(io_context_)
        //, manager_(io_context_)
        , document_root_(document_root)
        , num_of_thread_(num_thread) {
            boost::asio::ip::tcp::resolver resolver(io_context_);
            boost::asio::ip::tcp::endpoint endpoint = 
                *(resolver.resolve(address, port)).begin();
            this->acceptor_.open(endpoint.protocol());
            this->acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            this->acceptor_.bind(endpoint);
            this->acceptor_.listen();

            // Accept();
            //f();

            //this->AssignMultithreadAccept_();
            //this->Accept_();
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
            this->Accept_();
            std::async(std::launch::async, [this] {
                io_context_.run(); 
            });
        }


        template <HttpMethodEnum... Method, typename Function, typename... Aspects>
        void AddRouter(
            const std::string& route, 
            Function&& callback,
            Aspects&&... aspects
        ) {
            this->http_router_.RegisterRouter<Method...>(
                route,
                std::forward<Function>(callback),
                std::forward<Aspects>(aspects)...
            );
            // manager_.RegisterRouter<Method...>(
            //     route,
            //     std::forward<Function>(callback),
            //     std::forward<Aspects>(aspects)...
            // );
            // manager_.RegisterRouter(
            //             route,
            //             std::forward<Function>(callback),
            //             std::move(aspects)...
            //         );
        }

        template <HttpMethodEnum... Method, typename Function, typename... Aspects>
        void AddRouter(
            std::regex&& route, 
            Function&& callback,
            Aspects&&... aspects
        ) {
            this->http_router_.RegisterRouter<Method...>(
                std::forward<std::regex>(route),
                std::forward<Function>(callback),
                std::forward<Aspects>(aspects)...
            );
            // manager_.RegisterRouter(
            //             route,
            //             std::forward<Function>(callback),
            //             std::move(aspects)...
            //         );
        }

        inline std::string GetDocumentRoot() const noexcept {
            return document_root_;
        }
        
        

    private:
        boost::asio::io_context             io_context_;
        boost::asio::ip::tcp::acceptor      acceptor_;
        std::string                         document_root_;
        //ConnectionManager                   manager_;
        HttpRouter                          http_router_;
        std::mutex                          acceptor_mutex_;
        std::atomic<bool>                   atomic_accept_flag_{false};
        std::size_t                         num_of_thread_;
        std::shared_ptr<ThreadPool>         thread_pool_;

        void AssignMultithreadAccept_() {
            // this->thread_pool_ = std::make_shared<ThreadPool>(num_of_thread_);
            // for (std::size_t i = 0; i < this->num_of_thread_; ++i) {
            //     this->thread_pool_->AddTask([this]{
            //         this->Accept_();
            //     });
            // }
        }
        
        // std::shared_ptr<std::thread> 
        //                     thread_;
        void Accept_() {
            //while (this->atomic_accept_flag_);
            //this->atomic_accept_flag_ = true;
            
            std::shared_ptr<Connection> ptr_conn(new Connection(this->io_context_, this->http_router_));
            acceptor_.async_accept(
                ptr_conn->Socket(), 
                [this, ptr_conn] (boost::system::error_code ec) {
                    if(!ec) {
                        debug().dg("accept: ").dg(ec.message()).lf();
                        //debug().dg("start").lf();
                        ptr_conn->Socket().set_option(boost::asio::ip::tcp::no_delay(true));
                        ptr_conn->Start();
                    } else {
                        std::cout << "err: " << ec.message() << std::endl;
                    }
                    //this->atomic_accept_flag_ = false;
                    this->Accept_();
                }
            );
        }
    };

} // httpc