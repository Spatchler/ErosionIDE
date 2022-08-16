#include "UI.hpp"

using namespace math;

//singleton instances
static auto& window = ui::window::get();
static auto& eventHandler = ui::eventHandler::get();

//pallete - gruvbox
static std::unordered_map<std::string, ui::color> pallete {
    //bg
    {"bg",        ui::color( 40,  40,  40)},
    {"bg_h",      ui::color( 29,  32,  33)},
    {"bg_s",      ui::color( 50,  48,  47)},
    {"bg1",       ui::color( 60,  56,  54)},
    {"bg2",       ui::color( 80,  73,  69)},
    {"bg3",       ui::color(102,  92,  84)},
    {"bg4",       ui::color(124, 111, 100)},
    //fg
    {"fg",        ui::color(235, 219, 178)},
    {"fg1",       ui::color(251, 241, 199)},
    {"fg2",       ui::color(213, 196, 161)},
    {"fg3",       ui::color(189, 174, 147)},
    {"fg4",       ui::color(168, 153, 132)},
    //high saturation
    {"red_h",     ui::color(204,  36,  29)},
    {"green_h",   ui::color(152, 151,  26)},
    {"yellow_h",  ui::color(215, 153,  33)},
    {"blue_h",    ui::color( 69, 133, 136)},
    {"purple_h",  ui::color(177,  98, 134)},
    {"aqua_h",    ui::color(104, 157, 106)},
    {"orange_h",  ui::color(214,  93,  14)},
    {"gray_h",    ui::color(168, 153, 132)},
    //low saturation
    {"gray_l",    ui::color(146, 131, 116)},
    {"red_l",     ui::color(251,  73,  52)},
    {"green_l",   ui::color(184, 187,  38)},
    {"yellow_l",  ui::color(250, 189,  47)},
    {"blue_l",    ui::color(131, 165, 152)},
    {"purple_l",  ui::color(211, 134, 155)},
    {"aqua_l",    ui::color(142, 192, 124)},
    {"orange_l",  ui::color(254, 128,  25)}
};

//surfaces
static ui::surface mainWorkspace, fileExplorer;

//textures
static ui::texture testButtonNormalTexture;
static ui::texture testButtonHoveringTexture;
static ui::texture testButtonPressedTexture;

//objects
static ui::textureButton testButton(ui::rect(), &testButtonNormalTexture, &testButtonHoveringTexture, &testButtonPressedTexture);
static ui::rectButton testRectButton(ui::rect(math::vec2i(50, 50), math::vec2i(50, 50)), "test content", pallete["red_l"], pallete["green_l"], pallete["yellow_l"]);

//initalization function called once on progam startup
void init() {
    //init window
    window.init("Erosion IDE - vALPHA", vec2i(1280, 720));

    //load textures
    testButtonNormalTexture.load("res/images/normal.png");
    testButtonHoveringTexture.load("res/images/hovering.png");
    testButtonPressedTexture.load("res/images/pressed.png");

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

    //add outline around focused surface
    window.outline = true;
    window.outlineColor = pallete["blue_h"];
    window.outlineThinkness = 5;

    //main workspace
    mainWorkspace.bgColor = pallete["bg"];

    //file explorer
    fileExplorer.bgColor = pallete["bg_h"];

    //add objects
    //mainWorkspace.addObject(&testButton, vec2i(50, 50));
    mainWorkspace.add(&testRectButton, vec2i(50, 50));

    //split window
    window.split({1, 4}, AxisX, {&fileExplorer, &mainWorkspace});
}
//update function called every update "50hz"
void update() {
}
//destroy function called once on program end
void destroy() {
}