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

static ui::surface mainWorkspace, fileExplorer;

//textures
static ui::texture testButtonNormalTexture;
static ui::texture testButtonHoveringTexture;
static ui::texture testButtonPressedTexture;

//fonts
static ui::font mainFont;

//buttons
static ui::textureButton testButton(ui::rect(), &testButtonNormalTexture, &testButtonHoveringTexture, &testButtonPressedTexture);
static ui::rectButton testRectButton(ui::rect(math::vec2i(), math::vec2i(200, 100)), 5, "test content", &mainFont, {pallete["red_l"], pallete["red_h"], pallete["red_h"]}, {pallete["green_l"], pallete["green_h"], pallete["green_h"]}, {pallete["yellow_l"], pallete["yellow_h"], pallete["yellow_h"]}, []{std::cout << "h" << "\n";});

//text boxes
static ui::textBox testTextBox(ui::rect(math::vec2i(), math::vec2i(150, 25)), &mainFont, &pallete["aqua_l"]);

//initalization function called once on progam startup
void init() {
    //init window
    window.init("Erosion IDE - vALPHA", vec2i(1280, 720));

    //load textures
    testButtonNormalTexture.load("res/images/normal.png");
    testButtonHoveringTexture.load("res/images/hovering.png");
    testButtonPressedTexture.load("res/images/pressed.png");

    testTextBox.addText("put your text here");

    //load font
    mainFont.load("res/fonts/Roboto-Regular.ttf", 24);

    //hotkeys
    //layouting
    eventHandler.mode = "insert";   //set starting mode to insert
    eventHandler.addHotkey("A", "*",      []{eventHandler.cycleMode<2>({"layout", "insert"});});            //toggle layouting mode
    eventHandler.addHotkey("h", "layout", []{window.currentSurf->enabled = !window.currentSurf->enabled;}); //show and hide
    eventHandler.addHotkey("j", "layout", []{window.mvFocus(-1);});                                         //move focus left
    eventHandler.addHotkey("k", "layout", []{window.mvFocus(1);});                                          //move focus right
    eventHandler.addHotkey("i", "layout", []{window.mvCurrentSurf(1);});                                    //move surface right
    eventHandler.addHotkey("u", "layout", []{window.mvCurrentSurf(-1);});                                   //move surface left
    eventHandler.addHotkey("f", "layout", []{window.resizeCurrentSurf(10);});                               //increase size
    eventHandler.addHotkey("d", "layout", []{window.resizeCurrentSurf(-10);});                              //decrease size

    //add outline around focused surface
    window.currentSurfOutline = true;
    window.currentSurfOutlineColor = pallete["blue_h"];
    window.currentSurfOutlineThinkness = 5;

    //main workspace
    mainWorkspace.bgColor = pallete["bg"];

    //file explorer
    fileExplorer.bgColor = pallete["bg_h"];

    //add objects
    fileExplorer.add(&testRectButton, vec2i(50, 50));
    fileExplorer.add(&testTextBox, vec2i(50, 10));

    //split window
    window.split({1, 4}, AxisX, {&fileExplorer, &mainWorkspace});
}
//update function called every update "50hz"
void update() {
    //testTextBox.addText("a");
}
//destroy function called once on program end
void destroy() {
}