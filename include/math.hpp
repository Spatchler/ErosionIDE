#pragma once
#include <cstdint>
#include <SDL2/SDL.h>

namespace math {
    enum axis : uint8_t {
        AxisX, AxisY
    };
    struct vec2i {
    public:
        vec2i(const int& p_x, const int& p_y);
        vec2i();

        void update();

        int* getAxis(axis a);

        SDL_Point* getSDLPoint();

        int x, y;
    private:
        SDL_Point sdlPoint;
    };
}