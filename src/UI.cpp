#include "UI.hpp"

int main(int argc, char* args[]) {
    static float startTime, waitTime;
    static float frameTime = 1000 / 50;
    init();
    while (ui::window::get().running) {
        startTime = SDL_GetTicks();
        if (ui::window::get().updateState)
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
        //rescale window contents
        if (eventHandler::get().resize || updateSizes) {
            if (updateSizes) {
                updateSizes = false;
            }
            eventHandler::get().resize = false;
            SDL_GetWindowSize(screen, &size.x, &size.y);
            int move = 0;
            for (std::size_t i = 0; i < layer.size(); ++i) {
                surface* currentSurf = layer.at(i);
                if (splitAxis == math::AxisX) {
                    currentSurf->size = math::vec2i(std::ceil(size.x / (float)std::accumulate(splitRatio.begin(), splitRatio.end(), 0)) * splitRatio.at(i), size.y);
                    currentSurf->pos.x = move;
                    move = currentSurf->pos.x + currentSurf->size.x;
                }
                else {
                    currentSurf->size = math::vec2i(size.x, std::ceil(size.y / (float)std::accumulate(splitRatio.begin(), splitRatio.end(), 0)) * splitRatio.at(i));
                    currentSurf->pos.y = move;
                    move = currentSurf->pos.y + currentSurf->size.y;
                }
            }
        }
        //update
        for (auto c: layer)
            c->render();
        //render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    void window::update() {
        //update
        for (auto c: layer)
            c->update();
    }

    void window::split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs) {
        splitAxis = p_axis;
        splitRatio = p_ratio;
        layer = p_surfs;
        int move = 0;
        for (std::size_t i = 0; i < layer.size(); ++i) {
            surface* currentSurf = layer.at(i);
            if (splitAxis == math::AxisX) {
                currentSurf->size = math::vec2i(std::ceil(size.x / (float)std::accumulate(splitRatio.begin(), splitRatio.end(), 0)) * splitRatio.at(i), size.y);
                currentSurf->pos.x = move;
                move = currentSurf->pos.x + currentSurf->size.x;
            }
            else {
                currentSurf->size = math::vec2i(size.x, std::ceil(size.y / (float)std::accumulate(splitRatio.begin(), splitRatio.end(), 0)) * splitRatio.at(i));
                currentSurf->pos.y = move;
                move = currentSurf->pos.y + currentSurf->size.y;
            }
        }
    }
    
    void window::mvFocus(char v) {
        focusPos.first += v;
        focusPos.first = (layer.size() + (focusPos.first % layer.size())) % layer.size();
    }
    void window::mvCurrentSurf(char v) {
        std::cout << "mvCurrentSurf: " << (int)focusPos.first << ", " << layer.size() << "\n";
        std::iter_swap(layer.begin() + focusPos.first, layer.begin() + (focusPos.first - v));
    }
    void window::resizeCurrentSurf(char v) {
        std::cout << "resizeCurrentSurf: " << (int)focusPos.first << ", " << layer.size() << "\n";
        *layer.at(focusPos.first)->size.getAxis(splitAxis) += v;
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
                    resize = true;
                    window::get().renderState = true;
                }
                if (event.window.event == SDL_WINDOWEVENT_SHOWN)
                    window::get().renderState = true;
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    window::get().updateState = true;
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                    window::get().updateState = false;
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
                    if (event.key.keysym.sym == k.first.at(k.first.size() - 1))
                        ctrlHotkeys.at(k.second).second = false;
                }
                for (auto& k: altHotkeySearch) {
                    if (event.key.keysym.sym == k.first.at(k.first.size() - 1))
                        altHotkeys.at(k.second).second = false;
                }
                keyBuffer.erase(std::find(keyBuffer.begin(), keyBuffer.end(), event.key.keysym.sym));
            }
        }
        //propiatery hotkeys for window
        if (keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_LALT) {
            //alt keybinds
            //show hide current surface
            if (keyBuffer.at(1) == SDLK_f)
                window::get().currentSurf->enabled = !window::get().currentSurf->enabled;
            //cycle current surface focus forward
            if (keyBuffer.at(1) == SDLK_k)
                window::get().mvFocus(1);
            //cycle current surface focus backward
            if (keyBuffer.at(1) == SDLK_j)
                window::get().mvFocus(-1);
            if (keyBuffer.size() > 2 && (keyBuffer.at(1) == SDLK_LCTRL || keyBuffer.at(1) == SDLK_RCTRL)) {
                //alt ctrl keybinds
                //move current surface forward
                if (keyBuffer.at(2) == SDLK_k)
                    window::get().mvCurrentSurf(1);
                //move current surface backward
                if (keyBuffer.at(2) == SDLK_j)
                    window::get().mvCurrentSurf(-1);
            }
            if (keyBuffer.size() > 2 && (keyBuffer.at(1) == SDLK_LSHIFT || keyBuffer.at(1) == SDLK_RSHIFT)) {
                //alt shift keybinds
                //increase currfent surface size
                if (keyBuffer.at(2) == SDLK_k)
                    window::get().resizeCurrentSurf(10);
                //decrease currfent surface size
                if (keyBuffer.at(2) == SDLK_j)
                    window::get().resizeCurrentSurf(-10);
            }
        }
        //ctrl hotkey checks
        if (keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_LCTRL || keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_RCTRL) {
            for (auto& k: ctrlHotkeySearch) {
                if (k.first.size() == 1) {
                    if (keyBuffer.at(1) == k.first.at(0)) {
                        if (!ctrlHotkeys.at(k.second).second) {
                            ctrlHotkeys.at(k.second).first();
                            ctrlHotkeys.at(k.second).second = true;
                        }
                    }
                }
                else if ((keyBuffer.at(1) == SDLK_LSHIFT || keyBuffer.at(1) == SDLK_RSHIFT) && keyBuffer.size() == 3) {
                    if (keyBuffer.at(2) == k.first.at(1)) {
                        if (!ctrlHotkeys.at(k.second).second) {
                            ctrlHotkeys.at(k.second).first();
                            ctrlHotkeys.at(k.second).second = true;
                        }
                    }
                }
            }
        }
        //alt hotkey checks
        if (keyBuffer.size() > 1 && keyBuffer.at(0) == SDLK_LALT) {
            for (auto& k: altHotkeySearch) {
                if (k.first.size() == 1) {
                    if (keyBuffer.at(1) == k.first.at(0)) {
                        if (!altHotkeys.at(k.second).second) {
                            altHotkeys.at(k.second).first();
                            altHotkeys.at(k.second).second = true;
                        }
                    }
                }
                else if ((keyBuffer.at(1) == SDLK_LSHIFT || keyBuffer.at(1) == SDLK_RSHIFT) && keyBuffer.size() == 3) {
                    if (keyBuffer.at(2) == k.first.at(1)) {
                        if (!altHotkeys.at(k.second).second) {
                            altHotkeys.at(k.second).first();
                            altHotkeys.at(k.second).second = true;
                        }
                    }
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