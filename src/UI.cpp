#include "UI.hpp"

namespace ui {
    window::window(const char* title, const math::vec2& size)
    :running(true)
    {
        screen, renderer = NULL;

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
        for (auto c: updateLayer) {
            std::visit(process{}, c);
        }
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
            }
        }
        SDL_RenderClear(renderer);
    }

    void window::split(std::vector<unsigned short>& p_ratio, std::vector<surface*>& p_surfs) {
        for (std::size_t i = 0; i < updateLayer.size(); ++i)
            updateLayer.emplace_back(p_surfs.at(i));
    }

    void window::process::operator()(surface* s) {
        s->update();
    }
    void window::process::operator()(obj* o) {
        o->update();
    }
    void window::process::operator()(std::pair<rect*, color*> p) {
        rect* r = std::get<0>(p);
        r->update();
        r->render(renderer, std::get<1>(p));
    }

    window::~window() {
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    surface::surface() 
    :enabled(true) {
    }

    void surface::update() {
        for (auto c: children) {
            std::visit(process{}, c);
        }
    }

    void surface::split(std::vector<unsigned short>& p_ratio, std::vector<surface*>& p_surfs) {
        for (std::size_t i = 0; i < children.size(); ++i)
            children.emplace_back(p_surfs.at(i));
    }

    void surface::fill(unsigned char& p_r, unsigned char& p_g, unsigned char& p_b) {
    }

    obj::obj(surface* p_surf, const math::vec2& p_pos, short& p_layer) 
    :pos(p_pos), layer(p_layer), surf(p_surf), enabled(true) {
        if (layer < 0)
            surf->children.emplace_back(this);
        else
            surf->children.insert(surf->children.begin() + layer, this);
    }

    void obj::update() {
    }

    color::color(uint8_t& p_r, uint8_t& p_g, uint8_t& p_b)
    :r(p_r), g(p_g), b(p_b) {
        SDLColor = {r, g, b};
    }
    color::color(uint8_t& p_r, uint8_t& p_g, uint8_t& p_b, uint8_t* p_a)
    :r(p_r), g(p_g), b(p_b), a(p_a) {
        SDLColor = {r, g, b, a};
    }

    SDL_Color* color::getSDLColor() {
        return &SDLColor;
    }

    rect::rect(math::vec2& p_pos, math::vec2& p_size) 
    :pos(p_pos), size(p_size) {
        SDLRect.x = pos.x;
        SDLRect.y = pos.y;
        SDLRect.w = size.x;
        SDLRect.h = size.y;
    }

    void rect::update() {
        SDLRect.x = pos.x;
        SDLRect.y = pos.y;
        SDLRect.w = size.x;
        SDLRect.h = size.y;
    }

    void rect::render(SDL_Renderer* r, color* c) {
        SDL_SetRenderDrawColor(r, c->r, c->g, c->b, c->a);
        SDL_RenderDrawRect(r, &SDLRect);
    }
}