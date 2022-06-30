#include "UI.hpp"

using namespace math;

static auto& window = ui::window::get();
static ui::surface mainWorkspace, fileExplorer;

void init() {
    window.init("Erosion IDE - vALPHA", vec2i(1280, 720));
    ui::color fileExplorerBGcolor(33, 37, 43);
    ui::color mainWorkspaceBGcolor(40, 44, 52);
    mainWorkspace.bgColor = mainWorkspaceBGcolor;
    fileExplorer.bgColor = fileExplorerBGcolor;
    window.split({1, 6}, ui::X, {&fileExplorer, &mainWorkspace});
}

void update() {
}

void destroy() {
}