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

    /*  add current surface show and hide hotkey "Alt + f"
        add current surface resize hotkey "Alt + (hjkl)"
        A for Alt, S for Shift, C for control
        hotkeys can only start with control or alt        */

    //show hide hotkey
    eventHandler.addHotkey("Af", [](){
        fileExplorer.enabled = !fileExplorer.enabled;
        ui::window::get().renderState = true;
    });
    //resize hotkeys
    //left
    eventHandler.addHotkey("Ah", [](){
        fileExplorer.size.x -= 10;
        ui::window::get().renderState = true;
    });
    //right
    eventHandler.addHotkey("Al", [](){
        fileExplorer.size.x += 10;
        ui::window::get().renderState = true;
    });
    //up
    eventHandler.addHotkey("Ak", [](){
        fileExplorer.size.y -= 10;
        ui::window::get().renderState = true;
    });
    //down
    eventHandler.addHotkey("Aj", [](){
        fileExplorer.size.y += 10;
        ui::window::get().renderState = true;
    });

    //split window
    window.split({1, 4}, AxisX, {&fileExplorer, &mainWorkspace});
}
//update function called every update "50hz"
void update() {
}
//destroy function called once on program end
void destroy() {
}