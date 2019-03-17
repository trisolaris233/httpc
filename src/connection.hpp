#pragma once

#include <memory>
#include <boost/asio.hpp>
#include "debug.hpp"
#include "request.hpp"
#include "response.hpp"
#include "http_router.hpp"
#include "noncopyable.hpp"
#include "request_parser.hpp"
#include "request_handler.hpp"

namespace httpc {
    class Connection 
        : public std::enable_shared_from_this<Connection>, Noncopyable {
    public:
        // static int conn_num;
        Connection(
            boost::asio::io_context& io_context,
            /*ConnectionManager& manager,*/ 
            HttpRouter&              router
            /*boost::asio::ip::tcp::socket&& socket*/
        ) 
            : http_router_(router)
            , socket_(io_context)
            , keep_alive_timer_(io_context)
            // , no(++conn_num)
        {
           // debug().dg("connection ctor").lf();
            //std::cout << "no." << this->no << " connection start" << std::endl;
        }

        // ~Connection() {
        //     debug().dg("connection dector").lf();
        // }

        void Start(/*std::function<void(std::shared_ptr<Connection>)> complete()*/) {
            // this->socket_.set_option(boost::asio::socket_base::keep_alive(true));
            //this->complete_callback_ = complete;
            DoRead();
        }

        boost::asio::ip::tcp::socket& Socket() {
            return this->socket_;
        }

        void Close() {
            this->socket_.close();
        }

        bool IsSocketOpen() const {
            return this->socket_.is_open();
        }

    private:
        //ConnectionManager&              manager_;
        HttpRouter&                     http_router_;
        boost::asio::ip::tcp::socket    socket_;
        std::array<char, 512>           buffer_;
        RequestParser                   parser_;
        Request                         request_;
        Response                        response_;
        RequestHandler                  handler_;
        // std::string                     write_buffer_;
        bool                            keep_alive_ {false};
        // bool                            hand_shake_ {false};
        // std::function<void(std::shared_ptr<Connection>)>
        //                                 complete_callback_;
        boost::asio::steady_timer       keep_alive_timer_;
        // int no;
        

        void ShutDownSocket_() {
            boost::system::error_code ignored_ec;
            this->socket_.shutdown(
                boost::asio::ip::tcp::socket::shutdown_both,
                ignored_ec
            );
        }

        void ActiveKeepAliveTimer() {
            //debug().dg("set timer").lf();
            // this->keep_alive_ = true;
            this->keep_alive_timer_.expires_after(std::chrono::seconds{60});
            this->keep_alive_timer_.async_wait([this, self = this->shared_from_this()]
            (boost::system::error_code const& ec) {
                if (ec) {
                    //debug().dg("timer handler: ").dg(ec.message()).lf();
                    return;
                }
                debug().dg("timer handler get closed").lf();
                //std::cout << "no." << this->no << " timer close" << std::endl;
                self->Socket().close();
            });
        }

        void HandleReadBody(boost::system::error_code const& ec, std::size_t bytes_transferred) {
            debug().dg("HandleReadBody() member function").lf();
        
            // debug().dg(this->request_).lf();

            // judge if user would like to receive the next file
            // if (this->request_.RecvNextFileOrNot()) {
            this->parser_.ParseBody(
                this->request_,
                this->buffer_.data(), 
                this->buffer_.data() + bytes_transferred
            );
            this->parser_.ParseFormData(
                this->request_,
                this->buffer_.data(), 
                this->buffer_.data() + bytes_transferred
            );
            debug().dg("parse post body complete").lf();
            // debug().dg(this->request_).lf();
            auto itr = this->response_.FindHeader("Content-Length");
            if (itr != this->response_.HeaderCEnd()) {
                debug().dg("Response Content-Length => ").dg(itr->value).lf();
            }
            // complete the parsement
            this->http_router_.Route(
                this->request_.method,
                this->request_.uri.GetUri(), 
                this->request_, 
                this->response_
            );
            // }
            
            // this->CheckKeepAlive_();

            // this->request_.uri.Update();
                // debug().dg(this->request_.uri.GetUri()).lf();
            
            //debug().dg(this->response_).lf();
            this->handler_.Respond(
                this->request_,
                this->response_
            );
            this->DoWrite();
        }

        void ReadBody() {
            debug().dg("enter the ReadBody() member function").lf();

            this->socket_.async_read_some(
                boost::asio::buffer(this->buffer_),
                [this, self = this->shared_from_this()] 
                (boost::system::error_code const& ec, std::size_t bytes_transferred) {
                    debug()
                        .dg("Read Body ec.message => ")
                        .dg(ec.message())
                        .lf()
                        .dg("size => ")
                        .dg(bytes_transferred)
                        .lf();
                    
                    // if no error occurs
                    if (!ec) {
                        debug().dg("try to invoke HandleReadBody() memeber function").lf();
                        HandleReadBody(ec, bytes_transferred);
                    } else {
                        // debug().dg("read eof do write").lf();
                        // this->CheckKeepAlive_();
                        debug().dg("Invoke route").lf();
                        
                            // debug().dg(this->request_.uri.GetUri()).lf();
                        this->http_router_.Route(
                            this->request_.method,
                            this->request_.uri.GetUri(), 
                            this->request_, 
                            this->response_
                        );
                        debug().dg("Make Respond").lf();
                        //debug().dg(this->response_).lf();
                        this->handler_.Respond(
                            this->request_,
                            this->response_
                        );
                        debug().dg("Do Write").lf();
                        this->DoWrite();
                    }
                }
            );
        }

        void ReadTillEof() {
            debug().dg("read till eof").lf();
            this->socket_.async_read_some(
                boost::asio::buffer(this->buffer_),
                [this, self = this->shared_from_this()]
                (boost::system::error_code const& ec, std::size_t bytes_transferred) {
                    debug().dg("cnm").lf();
                    if (ec) {

                    } else {

                    }
                }
            );
        }


        // parse the http headers
        void HandleRead_(
            boost::system::error_code const& ec, std::size_t bytes_transferred
        ) {
            auto parse_res = 
                this->parser_.Parse(
                    this->request_, this->buffer_.data(),
                    this->buffer_.data() + bytes_transferred
                );
            auto res_enum = std::get<0>(parse_res);
            // if a request if parsed completely
            if (res_enum == RequestParser::kGood) {
                // all the headers is read
                // then check the alive state
                this->CheckKeepAlive_();
                // update the uri required
                this->request_.uri.Update();
                auto itr = this->request_.FindHeader("Content-Length");
                if (itr == this->request_.HeaderCEnd()) {
                    // if not found
                    // find the route
                    this->http_router_.Route(
                        this->request_.method,
                        this->request_.uri.GetUri(), 
                        this->request_, 
                        this->response_
                    );
                    this->handler_.Respond(
                        this->request_,
                        this->response_
                    );
                    this->DoWrite();
                } else {
                    // call once the user bussiness
                    // pass the judgement of whether receive the next file
                    // to user.
                    this->http_router_.Route(
                        this->request_.method,
                        this->request_.uri.GetUri(), 
                        this->request_, 
                        this->response_
                    );

                    auto itr2 = this->request_.FindHeader("Content-Type");
                    if (itr2 != this->request_.HeaderCEnd()
                        && itr2->value.find_first_of("multipart/formdata") != std::string::npos) {

                        if (this->request_.RecvNextFileOrNot()) {
                            debug().dg("here try to read post body").lf();
                            // debug().dg(this->socket_.available()).lf();
                            this->ReadBody();
                            return;
                        }
                    }
                    this->handler_.Respond(
                        this->request_,
                        this->response_
                    );
                    this->DoWrite();
                    
                    
                }
                // debug().dg("invoke this->readbody()").lf();
                //this->ResetKeepAliveTimer();
            } else if (res_enum == RequestParser::kIndeterminate) {
                //this->ResetKeepAliveTimer();
                this->DoRead();
            } else {   // make a bad request.
                this->response_.SetDefault(HTTPStatusCodeEnum::kBadRequest);
                this->DoWrite();
            }
        }

        void DoRead() {
            // set up timer for each call
            this->ActiveKeepAliveTimer();

            // try to read from buffer
            socket_.async_read_some(
                boost::asio::buffer(this->buffer_),
                [this, self = this->shared_from_this()] 
                (boost::system::error_code const& ec, std::size_t bytes_transferred) {
                    // if no error occurs
                    if (!ec) {
                        // handle read.
                        this->HandleRead_(ec, bytes_transferred);
                    } else {
                        if (ec == boost::asio::error::eof) {
                            // this->hand_shake_ = false;
                            debug().dg("shut down send").lf();
                            boost::system::error_code ignore_ec;
                            this->Socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignore_ec);
                        }
                    }
                }
            );
        }

        void CheckKeepAlive_() {
            if (this->request_.http_major_version == 1) {
                bool conn_flag = false;
                auto itr = this->request_.FindHeader("Connection");
                if (itr != this->request_.HeaderCEnd()) {
                    if (itr->value == "close") {
                        this->keep_alive_ = false;
                    } else if (itr->value == "keep-alive") {
                        this->keep_alive_ = true;
                    }
                } else {
                    if (this->request_.http_minor_version == 1) {
                        this->keep_alive_ = true;
                    } else if (this->request_.http_minor_version == 0) {
                        this->keep_alive_ = false;
                    }
                }
            }
            if (this->keep_alive_) {
                this->response_.AddHeader(Header{"Connection", "keep-alive"});
            } else {
                this->response_.AddHeader(Header{"Connection", "close"});
            }
            
        }

        void HandleWrite_(boost::system::error_code ec, std::size_t bytes_transferred) {
            if (this->keep_alive_) {
                this->parser_.Reset();
                this->request_.Reset();
                this->response_.Reset();
                //std::cout << "keep-alive" << std::endl;
                this->DoRead();
            }
            else {
                this->keep_alive_timer_.cancel();
                this->ShutDownSocket_();
                this->Close();
            }
            
        }

        void DoWrite()  {
            // auto self{shared_from_this()};
            // std::cout << "write" << std::endl;
            // this->write_buffer_ = response_.ToBuffers();
// #ifdef HTTPC_ENABLE_GZIP 
//             this->response_.WriteGzipBuffer(this->write_buffer_);
//             // this->response_.WriteBuffer(this->write_buffer_);
// #endif
            // std::vector<boost::asio::const_buffer> buffers = this->response_.ToBuffers();
            // for(auto x : buffers) {
            //     std::cout << static_cast<const char*>(x.data()) << std::endl;
            // }
            // std::cout << response_ << std::endl;
            // this->ResetKeepAliveTimer();
            // debug().dg(this->response_).lf();
            // this->ActiveKeepAliveTimer();
            boost::asio::async_write(
                socket_,
                this->response_.ToBuffers(),
                [this, self = this->shared_from_this()]
                (boost::system::error_code ec, std::size_t bytes_transferred) {
                    //std::cout << ec.message() << std::endl;
                    //std::cout << bytes_transferred << std::endl;
                    this->HandleWrite_(ec, bytes_transferred);
                }
            );
        }

        
    };

    // int Connection::conn_num = 0;
}