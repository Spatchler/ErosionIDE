#include "UI.hpp"

namespace ui {
    window::window(const char* title, const math::vec2& size)
    :screen(NULL), renderer(NULL), running(true)
    {
        if (SDL_Init(SDL_INIT_VIDEO) > 0)
            std::cout << "SDL_Init failed, SDL_ERROR: " << SDL_GetError() << std::endl;
        if (!(IMG_Init(IMG_INIT_PNG)))
            std::cout << "IMG_Init failed, Error: " << SDL_GetError() << std::endl;

        screen = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, 0);

        if (screen == NULL)
            std::cout << "Window init failed error:" << SDL_GetError() << std::endl;

        renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
    }

    void window::update() {
        for (surface* s: surfaces) {
            if (s->enabled)
                s->update();
        }
        for (obj* o: objs) {
            if (o->enabled)
                o->update();
        }
        for (rect* r: rects) {
            if (r)
        }
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
            }
        }
    }

    void window::split(std::vector<unsigned short> p_ratio) {
        for (unsigned short i: p_ratio)
            surfaces.emplace_back(surface());
    }

    window::~window() {
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    surface::surface() 
    :enabled(true) {
    }

    void surface::update() {
        for (surface* s: surfaces) {
            if (s->enabled)
                s->update();
        }
        for (obj* o: objs) {
            if (o->enabled)
                o->update();
        }
    }

    void surface::split(std::vector<unsigned short> p_ratio) {
        for (unsigned short i: p_ratio)
            surfaces.emplace_back(surface());
    }

    void surface::fill(unsigned char& p_r, unsigned char& p_g, unsigned char& p_b) {
    }

    obj::obj(surface* p_surf, const math::vec2& p_pos, short& p_layer) 
    :pos(p_pos), layer(p_layer), surf(p_surf), enabled(true) {
        if (layer < 0)
            surf->objs.emplace_back(this);
        else
            surf->objs.insert(surf->objs.begin() + layer, this);
    }

    void obj::update() {
    }

    color::color(unsigned char& p_r, unsigned char& p_g, unsigned char& p_b)
    :r(p_r), g(p_g), b(p_b) {
        SDLColor = {r, g, b};
    }

    color::getSDLColor() {
        return &SDLColor;
    }

    rect::rect(math::vec2& p_pos, math::vec2& p_size) 
    :pos(p_pos), size(p_size) {
        SDLRect.x = pos.x;
        SDLRect.y = pos.y;
        SDLRect.w = size.x;
        SDLRect.h = size.y;
    }

    SDL_Rect* rect::getSDLRect() {
        return &SDLRect;
    }
}