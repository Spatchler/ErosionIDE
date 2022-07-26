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
    //show hide hotkey
    eventHandler.addHotkey("Af", []{
        std::cout << "show/hide" << "\n";
        window.currentSurf->enabled = !window.currentSurf->enabled;
    });
    //cycle current surface focus forward
    eventHandler.addHotkey("Aj", []{
        std::cout << "move focus left" << "\n";
        window.mvFocus(-1);
    });
    //cycle current surface focus backward
    eventHandler.addHotkey("Ak", []{
        std::cout << "move focus right" << "\n";
        window.mvFocus(1);
    });
    //move current surface forward
    eventHandler.addHotkey("ACj", []{
        std::cout << "move left" << "\n";
        window.mvCurrentSurf(-1);
    });
    //move current surface backward
    eventHandler.addHotkey("ACk", []{
        std::cout << "move right" << "\n";
        window.mvCurrentSurf(1);
    });
    //increase current surface size
    eventHandler.addHotkey("ASj", []{
        std::cout << "resize left" << "\n";
        window.resizeCurrentSurf(-10);
    });
    //decrease current surface size
    eventHandler.addHotkey("ASk", []{
        std::cout << "resize right" << "\n";
        window.resizeCurrentSurf(10);
    });

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