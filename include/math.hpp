#pragma once
#include <cstdint>

namespace math {
    enum axis : uint8_t {
        AxisX, AxisY
    };
    struct vec2i {
    public:
        vec2i(const int& p_x, const int& p_y);
        vec2i();
        int x, y;
    private:
    };
}