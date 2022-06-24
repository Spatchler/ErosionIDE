#pragma once
#include <iostream>
#include <vector>
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
        void split(std::vector<unsigned short> p_ratio);

        std::vector<surface*> surfaces;
        std::vector<obj*> objs;
        std::vector<rect*> rects;
        std::vector<SDL_Event> events;
        bool running;
        
        ~window();
    private:
        SDL_Window* screen;
        SDL_Renderer* renderer;
        SDL_Event event;
    };

    class surface {
    public:
        surface();

        void update();
        void split(std::vector<unsigned short> p_ratio);
        void fill(unsigned char& p_r, unsigned char& p_g, unsigned char& p_b);

        std::vector<surface*> surfaces;
        std::vector<obj*> objs;
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
        color(unsigned char& p_r, unsigned char& p_g, unsigned char& p_b);

        SDL_Color* getSDLColor();

        unsigned char r, g, b;
    private:
        SDL_Color SDLColor;
    };

    class rect {
    public:
        rect(math::vec2& p_pos, math::vec2& p_size);

        SDL_Rect* getSDLRect();

        math::vec2 pos;
        math::vec2 size;
    private:
        SDL_Rect SDLRect;
    }
}