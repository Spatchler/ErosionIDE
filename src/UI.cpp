#include "UI.hpp"

int main(int argc, char* args[]) {
    static float startTime, waitTime;
    static float frameTime = 1000 / 50;
    init();
    while (ui::window::get().running) {
        startTime = SDL_GetTicks();
        ui::window::get().update();
        ui::eventHandler::get().update();
        update();
        if (ui::window::get().renderState)
            ui::window::get().render();
        ui::window::get().renderState = false;
        waitTime = frameTime - (SDL_GetTicks() - startTime);
        if (waitTime > 0)
            SDL_Delay(waitTime);
    }
    destroy();
    return 0;
}

namespace ui {
    window window::instance;
    eventHandler eventHandler::instance;

    void renderProcess::operator()(surface* s) {
        s->render();
    }
    void renderProcess::operator()(obj* o) {
        o->render();
    }

    void updateProcess::operator()(surface* s) {
        s->update();
    }
    void updateProcess::operator()(obj* o) {
        o->update();
    }

    void window::init(const char* p_title, const math::vec2i& p_size) {
        if (!initalized) {
            screen, renderer = NULL;

            if (SDL_Init(SDL_INIT_VIDEO) > 0)
                std::cout << "SDL_Init failed, SDL_ERROR: " << SDL_GetError() << std::endl;
            if (!(IMG_Init(IMG_INIT_PNG)))
                std::cout << "IMG_Init failed, Error: " << SDL_GetError() << std::endl;

            screen = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_size.x, p_size.y, SDL_WINDOW_RESIZABLE);

            if (screen == NULL)
                std::cout << "Window init failed error:" << SDL_GetError() << std::endl;

            renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED && SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            initalized = true;
            running = true;
            size = p_size;
            renderState = true;
        }
        else std::cout << "Cannot init twice" << std::endl;
    }

    void window::render() {
        //clear
        SDL_RenderClear(renderer);
        //update
        for (auto c: layer)
            std::visit(renderProcess{}, c);
        //render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    void window::update() {
        //update
        for (auto c: layer)
            std::visit(updateProcess{}, c);
    }

    void window::split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs) {
        int move = 0;
        for (std::size_t i = 0; i < p_surfs.size(); ++i) {
            auto currentSurf = p_surfs.at(i);
            if (p_axis == math::AxisX) {
                currentSurf->size = math::vec2i(std::ceil(size.x / (float)std::accumulate(p_ratio.begin(), p_ratio.end(), 0)) * p_ratio.at(i), size.y);
                currentSurf->pos.x = move;
                move = currentSurf->pos.x + currentSurf->size.x;
            }
            else {
                currentSurf->size = math::vec2i(size.x, std::ceil(size.y / (float)std::accumulate(p_ratio.begin(), p_ratio.end(), 0)) * p_ratio.at(i));
                currentSurf->pos.y = move;
                move = currentSurf->pos.y + currentSurf->size.y;
            }
            splits.emplace_back(std::make_pair(p_ratio, p_axis));
            layer.emplace_back(currentSurf);
        }
    }

    SDL_Renderer* window::getSDLRenderer() { return renderer; }
    SDL_Window* window::getSDLWindow() { return screen; }

    window::~window() {
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    void eventHandler::addHotkey(std::string h, const std::function<void()>& f) {
        if (h.at(0) == 'A') {
            altHotkeys.emplace_back(std::make_pair(f, false));
            h.erase(0, 1);
            altHotkeySearch.insert({h, altHotkeys.size() - 1});
        }
        else {
            ctrlHotkeys.emplace_back(std::make_pair(f, false));
            h.erase(0, 1);
            ctrlHotkeySearch.insert({h, ctrlHotkeys.size() - 1});
        }
    }

    void eventHandler::update() {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                window::get().running = false;
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    std::cout << "Window Resized!" << "\n";
                }
            }
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                std::cout << event.key.keysym.sym << "\n";
                keyBuffer.push_back(event.key.keysym.sym);
                // //letters
                // if (key > 96 && key < 123) {
                // }
                // //numbers
                // if (key > 47 && key < 58) {
                // }
                // //space
                // if (key == 32) {
                // }
                // //enter
                // if (key == 13) {
                // }
            }
            if (event.type == SDL_KEYUP && event.key.repeat == 0) {
                for (auto& k: ctrlHotkeySearch) {
                    if (event.key.keysym.sym == k.first.at(0))
                        ctrlHotkeys.at(k.second).second = false;
                }
                for (auto& k: altHotkeySearch) {
                    if (event.key.keysym.sym == k.first.at(0)) {
                        altHotkeys.at(k.second).second = false;
                    }
                }
                keyBuffer.erase(std::find(keyBuffer.begin(), keyBuffer.end(), event.key.keysym.sym));
            }
        }
        //ctrl hotkey checks
        if (keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_LCTRL || keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_RCTRL) {
            int i = 1;
            for (auto& k: ctrlHotkeySearch) {
                i = 1;
                for (auto& c: k.first) {
                    if (keyBuffer.size() >= i) {
                        if (keyBuffer.at(i) == c) {
                            if (!ctrlHotkeys.at(k.second).second) {
                                ctrlHotkeys.at(k.second).first();
                                ctrlHotkeys.at(k.second).second = true;
                            }
                        }
                    }
                    ++i;
                }
            }
        }
        //alt hotkey checks
        if (keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_LALT) {
            int i = 1;
            for (auto& k: altHotkeySearch) {
                i = 1;
                for (auto& c: k.first) {
                    if (keyBuffer.size() >= i) {
                        if (keyBuffer.at(i) == c) {
                            if (!altHotkeys.at(k.second).second) {
                                altHotkeys.at(k.second).first();
                                altHotkeys.at(k.second).second = true;
                            }
                        }
                    }
                    ++i;
                }
            }
        }
    }

    rect::rect(math::vec2i p_pos, math::vec2i p_size) 
    :filled(true), pos(p_pos), size(p_size) {
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

    void rect::render(color* c) {
        SDLRect.x = pos.x;
        SDLRect.y = pos.y;
        SDLRect.w = size.x;
        SDLRect.h = size.y;
        SDL_SetRenderDrawColor(window::get().getSDLRenderer(), c->r, c->g, c->b, c->a);
        if (filled)
            SDL_RenderFillRect(window::get().getSDLRenderer(), &SDLRect);
        else
            SDL_RenderDrawRect(window::get().getSDLRenderer(), &SDLRect);
        SDL_SetRenderDrawColor(window::get().getSDLRenderer(), 0, 0, 0, 255);
    }

    SDL_Rect* rect::getSDLRect() {
        return &SDLRect;
    }

    surface::surface()
    :enabled(true), r(math::vec2i(0, 0), math::vec2i(0, 0)), bgColor(0, 0, 0) {
    }

    void surface::render() {
        r.size = size;
        r.pos = pos;
        if (enabled) {
            r.render(&bgColor);
            for (auto c: layer)
                std::visit(renderProcess{}, c);
        }
    }

    void surface::update() {
        for (auto c: layer)
            std::visit(updateProcess{}, c);
    }

    void surface::split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs) {
        int move = 0;
        for (std::size_t i = 0; i < p_surfs.size(); ++i) {
            surface* currentSurf = p_surfs.at(i);
            if (p_axis == math::AxisX) {
                currentSurf->size = math::vec2i(std::ceil(size.x / (float)std::accumulate(p_ratio.begin(), p_ratio.end(), 0)) * p_ratio.at(i), size.y);
                currentSurf->pos.x = move;
                move = currentSurf->pos.x + currentSurf->size.x;
            }
            else {
                currentSurf->size = math::vec2i(size.x, std::ceil(size.y / (float)std::accumulate(p_ratio.begin(), p_ratio.end(), 0)) * p_ratio.at(i));
                currentSurf->pos.y = move;
                move = currentSurf->pos.y + currentSurf->size.y;
            }
            splits.emplace_back(std::make_pair(p_ratio, p_axis));
            layer.emplace_back(currentSurf);
        }
    }

    obj::obj(surface* p_surf, const math::vec2i& p_pos, short& p_layer) 
    :pos(p_pos), layer(p_layer), surf(p_surf), enabled(true) {
        if (layer < 0);
    }

    void obj::render() {
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