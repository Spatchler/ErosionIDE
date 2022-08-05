#include <math.hpp>

namespace math {
    vec2i::vec2i(const int& p_x, const int& p_y) {
        x = p_x;
        y = p_y;
    }
    vec2i::vec2i() {
        x = 0;
        y = 0;
    }

    void vec2i::update() {
        sdlPoint.x = x;
        sdlPoint.y = y;
    }

    int* vec2i::getAxis(axis a) {
        if (a == axis::AxisX)
            return &x;
        else
            return &y;
    }

    SDL_Point* vec2i::getSDLPoint() { return &sdlPoint; }
}