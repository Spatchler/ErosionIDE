#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "math.hpp"

namespace ui {
    class surface;
    class obj;
    class color;
    class rect;

    class window {
    public:
        window(const char* title, const math::vec2& size);

        void update();
        void split(std::vector<unsigned short>& p_ratio, std::vector<surface*>& p_surfs);

        std::vector<std::variant<surface*, obj*, std::pair<rect*, color*>>> updateLayer;
        std::vector<SDL_Event> events;
        bool running;
        
        ~window();
    private:
        struct process {
            void operator()(surface*);
            void operator()(obj*);
            void operator()(std::pair<rect*, color*> p);
        };

        static SDL_Window* screen;
        static SDL_Renderer* renderer;
        SDL_Event event;
    };

    class surface {
    public:
        surface();

        void update();
        void split(std::vector<unsigned short>& p_ratio, std::vector<surface*>& p_surfs);
        void fill(unsigned char& p_r, unsigned char& p_g, unsigned char& p_b);

        std::vector<std::variant<surface*, obj*, std::pair<rect*, color*>>> updateLayer;
        bool enabled;
        rect r;
    private:
    };

    class obj {
    public:
        obj(surface* p_surf, const math::vec2& p_pos, short& p_layer);

        void update();

        math::vec2 pos;
        surface* surf;
        short layer;
        bool enabled;
    private:
    };

    class color {
    public:
        color(uint8_t& p_r, uint8_t& p_g, uint8_t& p_b);

        SDL_Color* getSDLColor();

        uint8_t r, g, b, a;
    private:
        SDL_Color SDLColor;
    };

    class rect {
    public:
        rect(math::vec2& p_pos, math::vec2& p_size);

        void update();
        void render(std::variant<surface*, window*> s, color& c);

        math::vec2 pos;
        math::vec2 size;
    private:
        SDL_Rect SDLRect;
    };
}