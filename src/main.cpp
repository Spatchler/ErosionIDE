#include "UI.hpp"

using namespace math;

// void init() {
//     auto& window = ui::window::get();
//     window.init("Erosion IDE - vALPHA", vec2i(1280, 720));

//     ui::surface mainWorkspace, fileExplorer;

//     // window.split({1, 6}, {&fileExplorer, &mainWorkspace});

//     ui::rect r(vec2i(), vec2i(500, 500));
//     ui::color c(255, 255, 255);
//     window.updateLayer.emplace_back(std::make_pair(&r, &c));
// }

// void update() {
// }

int main(int argc, char* args[]) {
    auto& window = ui::window::get();
    window.init("Erosion IDE - vALPHA", vec2i(1280, 720));

    ui::surface mainWorkspace, fileExplorer;

    // window.split({1, 6}, {&fileExplorer, &mainWorkspace});

    ui::rect r(vec2i(), vec2i(10, 10));
    ui::color c(255, 200, 0);
    window.updateLayer.emplace_back(std::make_pair(&r, &c));

    while (window.running) {
        window.update();
    }

    return 0;
}