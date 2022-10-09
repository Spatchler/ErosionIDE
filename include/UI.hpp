#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <variant>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <tuple>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "math.hpp"

void init();
void update();
void destroy();

namespace ui {
    class surface;
    class obj;
    class color;
    class rect;
    class hotkey;

    //visitors
    struct renderProcess {
        void operator()(surface*);
        void operator()(obj*);
        void operator()(std::pair<rect*, color*> p);
    };
    struct updateProcess {
        void operator()(surface*);
        void operator()(obj*);
        void operator()(std::pair<rect*, color*> p);
    };

    class color {
    public:
        color();
        color(uint8_t p_r, uint8_t p_g, uint8_t p_b);
        color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);

        SDL_Color* getSDLColor();

        uint8_t r, g, b, a;
    private:
        SDL_Color SDLColor;
    };

    class window {
    public:
        window(const window&) = delete;
        static window& get() { return instance; }
        void init(const char* p_title, const math::vec2i& p_size);

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs, bool p_first = false);

        void mvFocus(char v);
        void mvCurrentSurf(char v);
        void resizeCurrentSurf(char v);

        math::axis splitAxis;
        std::vector<uint8_t> splitRatio;
        std::vector<short> splitOffset;
        std::vector<surface*> layer;
        surface* currentSurf;
        bool running, initalized, renderState, updateState, updateSizes, currentSurfOutline;
        color currentSurfOutlineColor;
        uint32_t currentSurfOutlineThinkness;

        SDL_Renderer* getSDLRenderer();
        SDL_Window* getSDLWindow();
        
        ~window();
    private:
        window() {};
        static window instance;
        math::vec2i size;

        std::pair<char, bool> focusPos;

        SDL_Window* screen;
        SDL_Renderer* renderer;
    };

    class eventHandler {
    public:
        eventHandler(const eventHandler&) = delete;
        static eventHandler& get() { return instance; }

        void addHotkey(std::string h, std::string m, const std::function<void()>& f);
        template <int len>
        void cycleMode(std::array<std::string, len> cycleArray) {
            mode = cycleArray.at((cycleArray.size() + (std::find(cycleArray.begin(), cycleArray.end(), mode) - cycleArray.begin() + 1 % cycleArray.size())) % cycleArray.size());
            std::cout << mode << "\n";
        }
        void update();

        bool keys[332];
        std::vector<uint32_t> keyBuffer;
        math::vec2i mousePos;
        uint32_t mouseButtons;
        bool resize;
        std::string mode;
    private:
        eventHandler() {};
        static eventHandler instance;
        SDL_Event event;
        std::unordered_map<std::string, std::pair<std::function<void()>, std::string>> hotkeys;
        std::vector<bool> hotkeyStates;
    };

    class rect {
    public:
        rect();
        rect(math::vec2i p_pos, math::vec2i p_size);

        void update();
        void render(color* c);
        void render(color* c, color& oc, uint32_t& ot);

        SDL_Rect* getSDLRect();

        bool filled;
        math::vec2i pos;
        math::vec2i size;
    private:
        SDL_Rect SDLRect;
    };

    class texture {
    public:
        texture();

        void load(const char* p_filePath);

        void render(rect* renderRect);

        SDL_Texture* getSDLTexture();
    private:
        SDL_Texture* sdlTexture;
    };

    class font {
    public:
        font() {}

        void load(const char* p_filePath, const uint16_t& p_fontSize);
        TTF_Font* getFont();

        ~font();
    private:
        TTF_Font* f;
    };
    
    class surface {
    public:
        surface();

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs);
        void add(obj* p_i, math::vec2i pos);
        void add(surface* p_i, math::vec2i pos);

        std::vector<std::pair<std::vector<uint8_t>, math::axis>> splits;
        std::vector<std::variant<surface*, obj*>> layer;
        std::vector<std::pair<math::vec2i, uint32_t>> posS;
        bool enabled, outline;
        math::vec2i size;
        math::vec2i pos;
        color bgColor;
        rect r;
    private:
        //SDL_Surface* surf;
    };

    class obj {
    public:
        virtual void render() = 0;
        virtual void update() = 0;

        rect objRect;

        virtual ~obj() = default;
    private:
    };

    enum buttonState: uint8_t {
        BUTTONNORMAL, BUTTONHOVERING, BUTTONPRESSED
    };

    class textureButton: public obj {
    public:
        textureButton(rect p_buttonRect, texture* p_textureNormal, texture* p_textureHovering, texture* p_texturePressed);

        void render() override;
        void update() override;

        texture* textureNormal;
        texture* textureHovering;
        texture* texturePressed;

        buttonState state;
    private:
    };

    class rectButton: public obj {
    public:
        rectButton(rect p_rect, std::string p_content, color& p_normalColor, color& p_hoveringColor, color& p_pressedColor, const std::function<void()>& f);
        rectButton(rect p_rect, uint32_t p_outlineThickness, std::string p_content, std::pair<color, color> p_normalColors, std::pair<color, color> p_hoveringColors, std::pair<color, color> p_pressedColors, const std::function<void()>& f);

        void render() override;
        void update() override;

        bool outline, used;
        uint32_t outlineThinkness;
        std::pair<color, color> normalColors;
        std::pair<color, color> hoveringColors;
        std::pair<color, color> pressedColors;
        std::string content;
        buttonState state;
        std::function<void()> function;
    private:
    };

    class textBox: public obj {
    public:
        textBox(rect p_rect, font* f, color* c);
        ~textBox();

        void render() override;
        void update() override;

        void addText(const std::string& p_text);

        std::vector<std::string> text;

        font* textFont;
        color* textColor;
    private:
        SDL_Surface* textSurf;
        SDL_Rect textRect;
        SDL_Texture* textTexture;
    };
}