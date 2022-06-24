#include <math.hpp>

namespace math {
    vec2::vec2(const int& p_x, const int& p_y) {
        x = p_x;
        y = p_y;
    }
    vec2::vec2() {
        x = 0;
        y = 0;
    }
}