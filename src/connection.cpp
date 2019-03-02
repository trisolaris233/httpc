#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include "connection.hpp" 
#include "request_parser.hpp"
#include "connection_manager.hpp"

namespace httpc {

    void Connection::Start(std::function<void(std::shared_ptr<Connection>)> complete) {
        this->complete_callback_ = complete;
        DoRead();
    }

    void Connection::Close() {
        this->socket_.close();
    }

    void Connection::DoRead() {
        // auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(buffer_),
            [this/*, self*/] 
            (boost::system::error_code ec, std::size_t bytes_transferred) {
                if(!ec) {
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
                        // first forward to user bussiness
                        // std::cout << "bussiness" << std::endl;
                        this->request_.uri.Update();
                        this->manager_.Route(
                            this->request_.method,
                            this->request_.uri.GetUri(), 
                            this->request_, 
                            this->response_
                        );
                        // std::cout << std::boolalpha << this->response_.Empty() << std::endl;
                        // complete the response.
                        this->handler_.Respond(
                            this->request_,
                            this->response_
                        );

                        // wait till the async operation is complete
                        while (response_.IsAsyncWriting());
                        DoWrite();
                    } else if (res_enum == RequestParser::kIndeterminate) {
                        DoRead();
                    } else {   // make a bad request.
                        this->response_.SetDefault(HTTPStatusCodeEnum::kBadRequest);
                        DoWrite();
                    }
                } else {
                    std::cerr << ec.message() << "?" << std::endl;
                }
            }
        );
    }

    void Connection::DoWrite()  {
        // auto self{shared_from_this()};
        // std::cout << "write" << std::endl;
        // this->write_buffer_ = response_.ToBuffers();
    #ifdef HTTPC_ENABLE_GZIP 
        this->response_.WriteGzipBuffer(this->write_buffer_);
    #else
        this->response_.WriteBuffer(this->write_buffer_);
    #endif
        // std::cout << response_ << std::endl;
        boost::asio::async_write(
            socket_,
            boost::asio::buffer(this->write_buffer_),
            [this](boost::system::error_code ec, std::size_t bytes_transferred) {
                if (!ec) {
                    // std::cout << "byte_transferred = " << bytes_transferred << std::endl;
                    boost::system::error_code ignored_ec;
                    socket_.shutdown(
                        boost::asio::ip::tcp::socket::shutdown_both,
                        ignored_ec
                    );
                    // delete the connection
                    //if (this->parser_)
                } else {
                    std::cerr << ec.message() << std::endl;
                }
                // try to close the connection
                //this->manager_.CloseConnection(this->shared_from_this());
                this->complete_callback_(this->shared_from_this());
            }
        );
    }
    
}