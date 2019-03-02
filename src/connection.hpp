#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "request.hpp"
#include "response.hpp"
#include "noncopyable.hpp"
#include "request_parser.hpp"
#include "request_handler.hpp"

namespace httpc {
    
    class ConnectionManager;
    class Connection 
        : public std::enable_shared_from_this<Connection>, Noncopyable {
    public:
        Connection(ConnectionManager& manager, 
            boost::asio::ip::tcp::socket&& socket
        ) 
            : manager_(manager)
            , socket_(std::move(socket))
        {}

        void Start();

        void Close();

        bool IsSocketOpen() const {
            return this->socket_.is_open();
        }

    private:
        ConnectionManager&              manager_;
        boost::asio::ip::tcp::socket    socket_;
        std::array<char, 512>           buffer_;
        RequestParser                   parser_;
        Request                         request_;
        Response                        response_;
        RequestHandler                  handler_;
        std::string                     write_buffer_;
        
        void DoRead();
        void DoWrite();
        
    };
}