#include "UI.hpp"

int main(int argc, char* args[]) {
    init();
    while (ui::window::get().running) {
        ui::window::get().update();
        update();
    }
    destroy();
    return 0;
}

namespace ui {
    window window::instance;

    void process::operator()(surface* s) {
        s->update();
    }
    void process::operator()(obj* o) {
        o->update();
    }
    void process::operator()(std::pair<rect*, color*> p) {
        rect* r = std::get<0>(p);
        r->update();
        r->render(window::get().getSDLRenderer(), std::get<1>(p));
    }

    void window::init(const char* p_title, const math::vec2i& p_size) {
        if (!initalized) {
            screen, renderer = NULL;

            if (SDL_Init(SDL_INIT_VIDEO) > 0)
                std::cout << "SDL_Init failed, SDL_ERROR: " << SDL_GetError() << std::endl;
            if (!(IMG_Init(IMG_INIT_PNG)))
                std::cout << "IMG_Init failed, Error: " << SDL_GetError() << std::endl;

            screen = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_size.x, p_size.y, 0);

            if (screen == NULL)
                std::cout << "Window init failed error:" << SDL_GetError() << std::endl;

            renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED && SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            initalized = true;
            running = true;
            size = p_size;
        }
        else std::cout << "Cannot init twice" << std::endl;
    }

    void window::update() {
        //clear
        SDL_RenderClear(renderer);
        //update
        for (auto c: updateLayer)
            std::visit(process{}, c);
        //get events
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
            }
        }
        //render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    void window::split(std::vector<unsigned char> p_ratio, axis p_axis, std::vector<surface*> p_surfs) {
        int move = 0;
        for (std::size_t i = 0; i < p_surfs.size(); ++i) {
            surface* currentSurf = p_surfs.at(i);
            if (p_axis == X) {
                currentSurf->size = math::vec2i(std::ceil(size.x / (float)std::accumulate(p_ratio.begin(), p_ratio.end(), 0)) * p_ratio.at(i), size.y);
                // if (i > 0) {
                //     surface* prevSurf = p_surfs.at(i - 1);
                //     currentSurf->pos.x = prevSurf->pos.x + prevSurf->size.x;
                // }
                currentSurf->pos.x = move;
                move = currentSurf->pos.x + currentSurf->size.x;
            }
            else {
                currentSurf->size = math::vec2i(size.x, std::ceil(size.y / (float)std::accumulate(p_ratio.begin(), p_ratio.end(), 0)) * p_ratio.at(i));
                if (i > 0) {
                    surface* prevSurf = p_surfs.at(i - 1);
                    currentSurf->pos.y = prevSurf->pos.y + prevSurf->size.y;
                }
            }
            updateLayer.emplace_back(currentSurf);
        }
    }

    SDL_Renderer* window::getSDLRenderer() { return renderer; }
    SDL_Window* window::getSDLWindow() { return screen; }

    window::~window() {
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    rect::rect(math::vec2i p_pos, math::vec2i p_size) 
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
        SDLRect.x = pos.x;
        SDLRect.y = pos.y;
        SDLRect.w = size.x;
        SDLRect.h = size.y;
        SDL_SetRenderDrawColor(r, c->r, c->g, c->b, c->a);
        SDL_RenderFillRect(r, &SDLRect);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    }

    SDL_Rect* rect::getSDLRect() {
        return &SDLRect;
    }

    surface::surface()
    :enabled(true), r(math::vec2i(0, 0), math::vec2i(0, 0)), bgColor(0, 0, 0) {
    }

    void surface::update() {
        r.size = size;
        r.pos = pos;
        r.render(window::get().getSDLRenderer(), &bgColor);
        //std::cout << "size: " << r.size.x << "," << r.size.y << "\n";
        //std::cout << "pos: " << r.pos.x << "," << r.pos.y << "\n";
        for (auto c: updateLayer) {
            std::visit(process{}, c);
        }
    }

    void surface::split(std::vector<unsigned char> p_ratio, axis p_axis, std::vector<surface*> p_surfs) {
        for (std::size_t i = 0; i < p_surfs.size(); ++i) {
            if (p_axis = X) {
                p_surfs.at(i)->size.x = size.x / (std::accumulate(p_ratio.begin(), p_ratio.end(), 0) * p_ratio.at(i));
                p_surfs.at(i)->size.y = size.y;
            }
            else {
                p_surfs.at(i)->size.x = size.x / (std::accumulate(p_ratio.begin(), p_ratio.end(), 0) * p_ratio.at(i));
                p_surfs.at(i)->size.x = size.x;
            }
            updateLayer.emplace_back(p_surfs.at(i));
        }
    }

    obj::obj(surface* p_surf, const math::vec2i& p_pos, short& p_layer) 
    :pos(p_pos), layer(p_layer), surf(p_surf), enabled(true) {
        if (layer < 0)
            surf->updateLayer.emplace_back(this);
        else
            surf->updateLayer.insert(surf->updateLayer.begin() + layer, this);
    }

    void obj::update() {
    }

    color::color(uint8_t p_r, uint8_t p_g, uint8_t p_b)
    :r(p_r), g(p_g), b(p_b) {
        SDLColor = {r, g, b};
    }
    color::color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
    :r(p_r), g(p_g), b(p_b), a(p_a) {
        SDLColor = {r, g, b, a};
    }

    SDL_Color* color::getSDLColor() {
        return &SDLColor;
    }
}