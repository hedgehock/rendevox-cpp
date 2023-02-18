#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <string>

namespace Rendevox {
    class Window {
    public:
        const char* renderType;
        int width;
        int height;

        const char* title;
        bool fullscreen;
        Window(const char* renderType, int width, int height, const char* title, bool fullscreen);
    };
}

#endif
