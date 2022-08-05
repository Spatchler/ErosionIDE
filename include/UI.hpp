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
        bool running, initalized, renderState, updateState, updateSizes, outline;
        color outlineColor;
        uint32_t outlineThinkness;

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

        void addHotkey(std::string h, const std::function<void()>& f);
        void update();

        bool keys[332];
        std::vector<uint32_t> keyBuffer;
        math::vec2i mousePos;
        uint32_t mouseButtons;
        bool resize;
    private:
        eventHandler() {};
        static eventHandler instance;
        SDL_Event event;
        std::unordered_map<std::string, std::function<void()>> hotkeys;
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
    
    class surface {
    public:
        surface();

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs);
        void addObject(obj* p_obj, math::vec2i pos);

        std::vector<std::pair<std::vector<uint8_t>, math::axis>> splits;
        std::vector<std::variant<surface*, obj*>> layer;
        bool enabled, outline;
        math::vec2i size;
        math::vec2i pos;
        color bgColor;
        rect r;
    private:
    };

    class obj {
    public:
        virtual void render() = 0;
        virtual void update() = 0;

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

        rect buttonRect;
        buttonState state;
    private:
    };

    class rectButton: public obj {
    public:
        rectButton(rect p_rect, std::string p_content, color& p_normalColor, color& p_hoveringColor, color& p_pressedColor);
        rectButton(rect p_rect, uint32_t& p_outlineThickness, std::string p_content, std::pair<color, color> p_normalColors, std::pair<color, color> p_hoveringColors, std::pair<color, color> p_pressedColors);

        void render() override;
        void update() override;

        rect buttonRect;
        bool outline;
        uint32_t outlineThinkness;
        std::pair<color, color> normalColors;
        std::pair<color, color> hoveringColors;
        std::pair<color, color> pressedColors;
        std::string content;
        buttonState state;
    private:
    };
}