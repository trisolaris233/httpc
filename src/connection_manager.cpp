#include <string>
#include "connection_manager.hpp"

namespace httpc {

    void ConnectionManager::AddConnection(boost::asio::ip::tcp::socket&& socket)  {
        auto ptr = std::make_shared<Connection>(*this, std::move(socket));
        connections_.insert(ptr);
        ptr->Start();
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