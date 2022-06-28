#include <iostream>

namespace math {
    struct vec2i {
    public:
        vec2i(const int& p_x, const int& p_y);
        vec2i();
        int x, y;
    private:
    };
}