#pragma once

#define HTTPC_DEBUG

#ifdef HTTPC_DEBUG
#undef HTTPC_DEBUG
#include <tuple>
#include <iostream>
#endif

namespace httpc {

     struct debug {
        template <typename... Args>
        debug& dg(Args&&... args) {
        #ifdef HTTPC_DEBUG
            std::apply([](auto&&... args) {
                ((std::cout << args), ...);
            }, std::make_tuple(std::forward<Args>(args)...));
        #endif
            return *this;
        }

        template <typename... Args>
        debug& ls(Args&&... args) {
        #ifdef HTTPC_DEBUG
            std::apply([](auto&&... args) {
                ((std::cout << args << "\n"), ...);
            }, std::make_tuple(std::forward<Args>(args)...));
        #endif
            return *this;
        }

        debug& lf() {
        #ifdef HTTPC_DEBUG
            std::cout << "\n";
        #endif
            return *this;
        }
    };

}