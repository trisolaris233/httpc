#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "noncopyable.hpp"
#include "request_handler.hpp"

namespace httpc {

class ConnectionManager;
class Connection 
    : Noncopyable, std::enable_shared_from_this<Connection> {

public:
    explicit Connection(boost::asio::ip::tcp::socket socket,
        ConnectionManager& manager);

private:
    boost::asio::ip::tcp::socket socket_;

};

using std::shared_ptr<Connection> ConnectionPtr;


}