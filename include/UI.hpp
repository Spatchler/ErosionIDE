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
        window(const window&) = delete;
        static window& get() {
            return instance;
        }
        void init(const char* title, const math::vec2i& size);

        void update();
        void split(std::vector<unsigned short>& p_ratio, std::vector<surface*>& p_surfs);

        std::vector<std::variant<surface*, obj*, std::pair<rect*, color*>>> updateLayer;
        std::vector<SDL_Event> events;
        bool running;
        bool initalized;

        struct {
            void operator()(surface*);
            void operator()(obj*);
            void operator()(std::pair<rect*, color*> p);
        } process;
        
        ~window();
    private:
        window() {};
        static window instance;

        static SDL_Window* screen;
        static SDL_Renderer* renderer;
        SDL_Event event;
    };

    class rect {
    public:
        rect(math::vec2i p_pos, math::vec2i p_size);

        void update();
        void render(SDL_Renderer* r, color* c);

        math::vec2i pos;
        math::vec2i size;
    private:
        SDL_Rect SDLRect;
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
        obj(surface* p_surf, const math::vec2i& p_pos, short& p_layer);

        void update();

        math::vec2i pos;
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
}