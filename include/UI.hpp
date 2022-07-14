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

    enum h : uint8_t { h ,b};

    class window {
    public:
        window(const window&) = delete;
        static window& get() { return instance; }
        void init(const char* p_title, const math::vec2i& p_size);

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs);

        std::vector<std::pair<std::vector<uint8_t>, math::axis>> splits;
        std::vector<std::variant<surface*, obj*>> layer;
        bool running, initalized, renderState;

        SDL_Renderer* getSDLRenderer();
        SDL_Window* getSDLWindow();
        
        ~window();
    private:
        window() {};
        static window instance;
        math::vec2i size;

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
    private:
        eventHandler() {};
        static eventHandler instance;
        SDL_Event event;
        std::vector<std::pair<std::function<void()>, bool>> ctrlHotkeys;
        std::unordered_map<std::string, uint8_t> ctrlHotkeySearch;
        std::vector<std::pair<std::function<void()>, bool>> altHotkeys;
        std::unordered_map<std::string, uint8_t> altHotkeySearch;
    };

    class color {
    public:
        color() {}
        color(uint8_t p_r, uint8_t p_g, uint8_t p_b);
        color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);

        SDL_Color* getSDLColor();

        uint8_t r, g, b, a;
    private:
        SDL_Color SDLColor;
    };

    class rect {
    public:
        rect(math::vec2i p_pos, math::vec2i p_size);

        void update();
        void render(color* c);

        SDL_Rect* getSDLRect();

        bool filled;
        math::vec2i pos;
        math::vec2i size;
    private:
        SDL_Rect SDLRect;
    };
    
    class surface {
    public:
        surface();

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs);

        std::vector<std::pair<std::vector<uint8_t>, math::axis>> splits;
        std::vector<std::variant<surface*, obj*>> layer;
        bool enabled;
        math::vec2i size;
        math::vec2i pos;
        color bgColor;
        rect r;
    private:
    };

    class obj {
    public:
        obj(surface* p_surf, const math::vec2i& p_pos, short& p_layer);

        void render();
        void update();

        math::vec2i pos;
        surface* surf;
        short layer;
        bool enabled;
    private:
    };
}