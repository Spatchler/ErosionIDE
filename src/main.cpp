#include "UI.hpp"

using namespace math;

//singleton instances
static auto& window = ui::window::get();
static auto& eventHandler = ui::eventHandler::get();

//pallete
static std::unordered_map<std::string, ui::color> pallete {
    {"dark grey", ui::color(33, 37, 43)},
    {"grey",      ui::color(40, 44, 52)}
};

//surfaces
static ui::surface mainWorkspace, fileExplorer;

//initalization function called once on progam startup
void init() {
    //init window
    window.init("Erosion IDE - vALPHA", vec2i(1280, 720));

    //main workspace
    mainWorkspace.bgColor = pallete["grey"];

    //file explorer
    fileExplorer.bgColor = pallete["dark grey"];

    //split window
    window.split({1, 4}, AxisX, {&fileExplorer, &mainWorkspace});
}
//update function called every update "50hz"
void update() {
}
//destroy function called once on program end
void destroy() {
}