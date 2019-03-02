#include <string>
#include "connection_manager.hpp"

namespace httpc {

    void ConnectionManager::AddConnection(boost::asio::ip::tcp::socket&& socket)  {
        auto ptr = std::make_shared<Connection>(*this, std::move(socket));
        // while (this->atomic_mutex_);
        // this->atomic_mutex_ = true;
        connections_.insert(ptr);
        ptr->Start(
            std::bind(
                &ConnectionManager::CloseConnection,
                this,
                std::placeholders::_1
            )
        );
        // this->atomic_mutex_ = false;
    }

    void ConnectionManager::CloseAllConnection() {
        for(auto conn : connections_) {
            conn->Close();
        }
    }

    void ConnectionManager::Route(
            const std::string&  http_method,
            const std::string&  route_directory,
            Request&            request,
            Response&           response
    ) {
        router_.Route(
            http_method,
            route_directory, 
            request, 
            response
        );
    }

} // httpc