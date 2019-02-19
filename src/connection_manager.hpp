#include <set>
#include "connection.hpp"

namespace httpc {

class ConnectionManager {

private:
    std::set<ConnectionPtr> connections_;
};

}