#pragma once

#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include "utility.hpp"
#include "response.hpp"
#include "noncopyable.hpp"
#include "request_parser.hpp"
#include "request_handler.hpp"

namespace httpc {
    // forward declaration
    class ConnectionManager;

    class Connection 
        : public std::enable_shared_from_this<Connection>, Noncopyable {
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

        void DoRead() {
            auto self(shared_from_this());
            socket_.async_read_some(
                boost::asio::buffer(buffer_),
                [this, self] (boost::system::error_code ec, std::size_t bytes_transferred) {
                    if(!ec) {
                        // std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
                        // std::cout << buffer_.data() << std::endl;
                        
                        // DoWrite();
                        // do the parse
                        auto parse_res = this->parser_.Parse(
                            this->request_, this->buffer_.data(),
                            this->buffer_.data() + bytes_transferred
                        );
                        // get the enum value of parse result.
                        auto res_enum = std::get<0>(parse_res);
                        
                        // if a request if parsed completely
                        if (res_enum == RequestParser::kGood) {
                            // call the handler to create the response according to 
                            // the request given
                            this->handler_.Respond(this->request_, this->response_);
                            // here we get the reponse & request
                            // find the router and call the user's function.
                            // if not found, search the Document Root like 
                            // what http server usually does.

                            
                            DoWrite();
                        } else if (res_enum == RequestParser::kIndeterminate) {
                            DoRead();
                        } else {   // make a bad request.

                        }
                    } else {
                        std::cerr << ec.message() << std::endl;
                    }
                }
            );
        }

        void DoWrite() {
            auto self(shared_from_this());
            // boost::asio::async_write(
            //     socket_,
            //     boost::asio::buffer(global, sizeof(global)),
            //     boost::asio::transfer_at_least(sizeof(global)),
            //     [this, self] (boost::system::error_code ec, std::size_t bytes_transferred) {
            //         if(!ec) {
            //             // ~~graceful~~ connection closure
            //             boost::system::error_code ignored_ec;
            //             socket_.shutdown(
            //                 boost::asio::ip::tcp::socket::shutdown_both,
            //                 ignored_ec
            //             );
            //         }
            //     }
            // );
        }
    };


} // httpc