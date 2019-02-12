#pragma once

#include <string>

namespace httpc {

// field-value pair
struct Header {
    std::string field;
    std::string value;
};

}