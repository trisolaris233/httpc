#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include "connection.hpp" 
#include "request_parser.hpp"
#include "connection_manager.hpp"

namespace httpc {

    void Connection::Start() {
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
                        // std::cout << "good parsement" << std::endl;
                        this->handler_.Respond(
                            this->request_,
                            this->response_
                        );
                        // std::cout << "request: " << std::endl << this->request_ << std::endl;
                        // std::cout << "uri: " << this->request_.uri.GetUri() << std::endl;
                        this->request_.uri.Update();
                        this->manager_.Route(
                            this->request_.method,
                            this->request_.uri.GetUri(), 
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
        this->response_.WriteBuffer(this->write_buffer_);
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
            }
        );
    }
    
}