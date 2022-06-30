#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <variant>
#include <utility>
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

    struct process {
        void operator()(surface*);
        void operator()(obj*);
        void operator()(std::pair<rect*, color*> p);
    };

    enum axis: unsigned char {
        X, Y
    };

    class window {
    public:
        window(const window&) = delete;
        static window& get() {
            return instance;
        }
        void init(const char* p_title, const math::vec2i& p_size);

        void update();
        void split(std::vector<unsigned char> p_ratio, axis p_axis, std::vector<surface*> p_surfs);

        std::vector<std::variant<surface*, obj*, std::pair<rect*, color*>>> updateLayer;
        std::vector<SDL_Event> events;
        bool running, initalized;

        SDL_Renderer* getSDLRenderer();
        SDL_Window* getSDLWindow();
        
        ~window();
    private:
        window() {};
        static window instance;
        math::vec2i size;

        SDL_Window* screen;
        SDL_Renderer* renderer;
        SDL_Event event;
    };

    class color {
    public:
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
        void render(SDL_Renderer* r, color* c);

        SDL_Rect* getSDLRect();

        math::vec2i pos;
        math::vec2i size;
    private:
        SDL_Rect SDLRect;
    };
    
    class surface {
    public:
        surface();

        void update();
        void split(std::vector<unsigned char> p_ratio, axis p_axis, std::vector<surface*> p_surfs);
        void fill(color c);

        std::vector<std::variant<surface*, obj*, std::pair<rect*, color*>>> updateLayer;
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

        void update();

        math::vec2i pos;
        surface* surf;
        short layer;
        bool enabled;
    private:
    };
}