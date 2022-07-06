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

    /*  add file explorer resize hotkey "Ctrl + Alt + (hjkl)"
        all hotkeys start with "Ctrl"                    */
    //resize hotkeys
    //left
    eventHandler.addHotkey("h", [](){
        mainWorkspace.size.x -= 10;
        ui::window::get().renderState = true;
    });
    //right
    eventHandler.addHotkey("l", [](){
        mainWorkspace.size.x += 10;
        ui::window::get().renderState = true;
    });
    //up
    eventHandler.addHotkey("k", [](){
        mainWorkspace.size.y -= 10;
        ui::window::get().renderState = true;
    });
    //down
    eventHandler.addHotkey("j", [](){
        mainWorkspace.size.y += 10;
        ui::window::get().renderState = true;
    });

    //file explorer
    fileExplorer.bgColor = pallete["dark grey"];

    /*  add file explorer show and hide hotkey "Ctrl + F"
        add file explorer resize hotkey "Ctrl + Alt + (hjkl)"
        all hotkeys start with "Ctrl"                    */
    eventHandler.addHotkey("f", [](){
        fileExplorer.enabled = !fileExplorer.enabled;
        ui::window::get().renderState = true;
    });

    //resize hotkeys
    //left
    eventHandler.addHotkey("h", [](){
        fileExplorer.size.x -= 10;
        ui::window::get().renderState = true;
    });
    //right
    eventHandler.addHotkey("l", [](){
        fileExplorer.size.x += 10;
        ui::window::get().renderState = true;
    });
    //up
    eventHandler.addHotkey("k", [](){
        fileExplorer.size.y -= 10;
        ui::window::get().renderState = true;
    });
    //down
    eventHandler.addHotkey("j", [](){
        fileExplorer.size.y += 10;
        ui::window::get().renderState = true;
    });

    //split window
    window.split({1, 4}, ui::X, {&fileExplorer, &mainWorkspace});
}
//update function called every update "50hz"
void update() {
}
//destroy function called once on program end
void destroy() {
}