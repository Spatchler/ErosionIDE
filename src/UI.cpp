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
                std::cout << "ERR: SDL_Init failed, SDL_ERROR: " << SDL_GetError() << "\n";
            if (!(IMG_Init(IMG_INIT_PNG)))
                std::cout << "ERR: IMG_Init failed, Error: " << SDL_GetError() << "\n";

            screen = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_size.x, p_size.y, SDL_WINDOW_RESIZABLE);

            if (screen == NULL)
                std::cout << "ERR: Window init failed error:" << SDL_GetError() << "\n";

            renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED && SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            initalized = true;
            running = true;
            size = p_size;
            renderState = true;
        }
        else std::cout << "ERR: Cannot init twice" << "\n";
    }

    void window::render() {
        //clear
        SDL_RenderClear(renderer);
        //rescale window contents
        if (eventHandler::get().resize || updateSizes) {
            SDL_GetWindowSize(screen, &size.x, &size.y);
            this->split(splitRatio, splitAxis, layer, true);
        }
        //update
        uint8_t i = 0;
        for (surface* s: layer) {
            if (i == focusPos.first)
                s->outline = true;
            else
                s->outline = false;
            s->render();
            ++i;
        }
        //render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    void window::update() {
        //update
        for (auto c: layer)
            c->update();
        updateSizes = false;
        eventHandler::get().resize = false;
    }

    void window::split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs, bool p_first) {
        if (!p_first) {
            splitAxis = p_axis;
            splitRatio = p_ratio;
            layer = p_surfs;
            for (uint8_t i: splitRatio)
                splitOffset.push_back(0);
        }
        int move = 0;
        for (std::size_t i = 0; i < layer.size(); ++i) {
            surface* currentSurf = layer.at(i);
            if (splitAxis == math::AxisX) {
                currentSurf->size = math::vec2i(std::ceil(size.x / (float)std::accumulate(splitRatio.begin(), splitRatio.end(), 0)) * splitRatio.at(i) + splitOffset.at(i), size.y);
                currentSurf->pos.x = move;
                move = currentSurf->pos.x + currentSurf->size.x;
            }
            else {
                currentSurf->size = math::vec2i(size.x, std::ceil(size.y / (float)std::accumulate(splitRatio.begin(), splitRatio.end(), 0)) * splitRatio.at(i) + splitOffset.at(i));
                currentSurf->pos.y = move;
                move = currentSurf->pos.y + currentSurf->size.y;
            }
        }
    }
    
    void window::mvFocus(char v) {
        focusPos.first += v;
        focusPos.first = (layer.size() + (focusPos.first % layer.size())) % layer.size();
        renderState = true;
    }
    void window::mvCurrentSurf(char v) {
        std::iter_swap(layer.begin() + focusPos.first, layer.begin() + (layer.size() + ((focusPos.first - v) % layer.size())) % layer.size());
        std::iter_swap(splitRatio.begin() + focusPos.first, splitRatio.begin() + (splitRatio.size() + ((focusPos.first - v) % splitRatio.size())) % splitRatio.size());
        std::iter_swap(splitOffset.begin() + focusPos.first, splitOffset.begin() + (splitOffset.size() + ((focusPos.first - v) % splitOffset.size())) % splitOffset.size());
        this->mvFocus(v);
        updateSizes = true;
        renderState = true;
    }
    void window::resizeCurrentSurf(char v) {
        splitOffset.at(focusPos.first) += v;
        splitOffset.at((layer.size() + (focusPos.first + 1 % layer.size())) % layer.size()) -= v;
        updateSizes = true;
        renderState = true;
    }

    SDL_Renderer* window::getSDLRenderer() { return renderer; }
    SDL_Window* window::getSDLWindow() { return screen; }

    window::~window() {
        SDL_DestroyWindow(screen);
        SDL_Quit();
    }

    void eventHandler::addHotkey(std::string h, std::string m, const std::function<void()>& f) {
        hotkeys.insert({h, {f, m}});
        hotkeyStates.push_back(false);
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
                // letters        key > 96 && key < 123
                // numbers        key > 47 && key < 58
            }
            if (event.type == SDL_KEYUP && event.key.repeat == 0) {
                uint32_t currentKey = event.key.keysym.sym;
                uint8_t i = 0;
                for (auto h: hotkeys) {
                    if (h.first.back() == 'A' && currentKey == SDLK_LALT)
                        hotkeyStates.at(i) = false;
                    else if (h.first.back() == 'C' && (currentKey == SDLK_LCTRL || currentKey == SDLK_RCTRL))
                        hotkeyStates.at(i) = false;
                    else if (h.first.back() == 'S' && (currentKey == SDLK_LSHIFT || currentKey == SDLK_RSHIFT))
                        hotkeyStates.at(i) = false;
                    else if (h.first.back() == 'T' && currentKey == SDLK_TAB)
                        hotkeyStates.at(i) = false;
                    else if (h.first.back() == currentKey - 47)
                        hotkeyStates.at(i) = false;
                    else if (h.first.back() == currentKey)
                        hotkeyStates.at(i) = false;
                    ++i;
                }
                keyBuffer.erase(std::find(keyBuffer.begin(), keyBuffer.end(), currentKey));
            }
        }

        //mouse
        mouseButtons = SDL_GetMouseState(&mousePos.x, &mousePos.y);
        mousePos.update();

        //hotkey check
        if (keyBuffer.size() > 0) {
            uint8_t hotkeyN = 0;
            for (auto h: hotkeys) {
                uint8_t i = 0;
                std::vector<bool> kState;
                for (auto k: h.first) {
                    if (keyBuffer.size() - 1 < i) {
                        kState.push_back(false);
                        break;
                    }
                    uint32_t currentKey = keyBuffer.at(i);
                    if (k == 'A' && currentKey == SDLK_LALT)
                        kState.push_back(true);
                    else if (k == 'C' && (currentKey == SDLK_LCTRL || currentKey == SDLK_RCTRL))
                        kState.push_back(true);
                    else if (k == 'S' && (currentKey == SDLK_LSHIFT || currentKey == SDLK_RSHIFT))
                        kState.push_back(true);
                    else if (k == 'T' && currentKey == SDLK_TAB)
                        kState.push_back(true);
                    else if (k == currentKey - 47)
                        kState.push_back(true);
                    else if (k == currentKey)
                        kState.push_back(true);
                    else
                        kState.push_back(false);
                    ++i;
                }
                if (std::all_of(kState.begin(), kState.end(), [](bool v){ return v; }) && !hotkeyStates.at(hotkeyN)) {
                    if (hotkeys.at(h.first).second == "*")
                        hotkeys.at(h.first).first();
                    else if (mode == hotkeys.at(h.first).second)
                        hotkeys.at(h.first).first();
                    hotkeyStates.at(hotkeyN) = true;
                }
                ++hotkeyN;
            }
        }
    }

    rect::rect()
    :filled(true), pos(math::vec2i()), size(math::vec2i()) {
        this->update();
    }

    rect::rect(math::vec2i p_pos, math::vec2i p_size) 
    :filled(true), pos(p_pos), size(p_size) {
        this->update();
    }

    void rect::update() {
        SDLRect.x = pos.x;
        SDLRect.y = pos.y;
        SDLRect.w = size.x;
        SDLRect.h = size.y;
    }

    //normal render
    void rect::render(color* c) {
        this->update();
        SDL_SetRenderDrawColor(window::get().getSDLRenderer(), c->r, c->g, c->b, c->a);
        if (filled)
            SDL_RenderFillRect(window::get().getSDLRenderer(), &SDLRect);
        else
            SDL_RenderDrawRect(window::get().getSDLRenderer(), &SDLRect);
        SDL_SetRenderDrawColor(window::get().getSDLRenderer(), 0, 0, 0, 255);
    }
    //outline render
    void rect::render(color* c, color& oc, uint32_t& ot) {
        SDLRect.x = pos.x + ot / 2;
        SDLRect.y = pos.y + ot / 2;
        SDLRect.w = size.x - ot;
        SDLRect.h = size.y - ot;
        SDL_Rect outlineRect = {pos.x, pos.y, size.x, size.y};
        SDL_SetRenderDrawColor(window::get().getSDLRenderer(), oc.r, oc.g, oc.b, oc.a);
        SDL_RenderFillRect(window::get().getSDLRenderer(), &outlineRect);
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

    texture::texture()
    :sdlTexture(nullptr) {
    }

    void texture::load(const char* p_filePath) {
        sdlTexture = IMG_LoadTexture(window::get().getSDLRenderer(), p_filePath);
        if (sdlTexture == nullptr)
            std::cout << "ERR: Failed loading texture: " << p_filePath << ", " << SDL_GetError() << "\n";
    }

    void texture::render(rect* renderRect) {
        std::cout << "on texture render" << "\n";
        SDL_RenderCopy(window::get().getSDLRenderer(), sdlTexture, NULL, renderRect->getSDLRect());
        std::cout << "passed texture render" << "\n";
    }

    SDL_Texture* texture::getSDLTexture() { return sdlTexture; }

    surface::surface()
    :enabled(true), r(math::vec2i(0, 0), math::vec2i(0, 0)), bgColor(0, 0, 0) {
    }

    void surface::render() {
        r.size = size;
        r.pos = pos;
        if (enabled) {
            if (outline)
                r.render(&bgColor, window::get().currentSurfOutlineColor, window::get().currentSurfOutlineThinkness);
            else
                r.render(&bgColor);
            for (auto c: layer)
                std::visit(renderProcess{}, c);
        }
    }

    void surface::update() {
        if (eventHandler::get().resize || window::get().updateSizes) {
            for (auto u: posS) {
                auto v = std::get_if<ui::obj*>(&layer.at(u.second));
                if (v != nullptr) {
                    u.first = math::vec2i(size.x / (100 / u.first.x) - (*v)->objRect.size.x / 2 + pos.x, size.y / (100 / u.first.y) - (*v)->objRect.size.y / 2 + pos.y);
                    (*v)->objRect.pos = u.first;
                }
                else {
                    u.first = math::vec2i(size.x / (100 / u.first.x) - std::get<ui::surface*>(layer.at(u.second))->size.x / 2, size.y / (100 / u.first.y) - std::get<ui::surface*>(layer.at(u.second))->size.y / 2);
                    std::get<ui::surface*>(layer.at(u.second))->pos = u.first;
                }
            }
        }
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
    
    void surface::add(obj* p_i, math::vec2i pos) {
        layer.emplace_back(p_i);
        posS.emplace_back(std::make_pair(pos, layer.size() - 1));
    }
    void surface::add(surface* p_i, math::vec2i pos) {
        layer.emplace_back(p_i);
        posS.emplace_back(std::make_pair(pos, layer.size() - 1));
    }

    color::color()
    :r(0), g(0), b(0) {
    }

    color::color(uint8_t p_r, uint8_t p_g, uint8_t p_b)
    :r(p_r), g(p_g), b(p_b) {
        SDLColor = {r, g, b};
    }
    color::color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a)
    :r(p_r), g(p_g), b(p_b), a(p_a) {
        SDLColor = {r, g, b, a};
    }

    SDL_Color* color::getSDLColor() { return &SDLColor; }

    textureButton::textureButton(rect p_buttonRect, texture* p_textureNormal, texture* p_textureHovering, texture* p_texturePressed)
    :state(BUTTONNORMAL) {
        objRect = p_buttonRect;
    }

    void textureButton::render() {
        if (state == BUTTONNORMAL) {
            std::cout << "on normal render" << "\n";
            textureNormal->render(&objRect);
            std::cout << "after normal render" << "\n";
        }
        else if (state == BUTTONHOVERING)
            textureHovering->render(&objRect);
        else if (state == BUTTONPRESSED)
            texturePressed->render(&objRect);
    }
    void textureButton::update() {
        if (SDL_PointInRect(eventHandler::get().mousePos.getSDLPoint(), objRect.getSDLRect()))
            state = BUTTONHOVERING;
        else
            state = BUTTONNORMAL;
    }

    rectButton::rectButton(rect p_rect, std::string p_content, color& p_normalColor, color& p_hoveringColor, color& p_pressedColor)
    :outline(false), content(p_content), normalColors(p_normalColor, color()), hoveringColors(p_hoveringColor, color()), pressedColors(p_pressedColor, color()) {
        objRect = p_rect;
    }
    rectButton::rectButton(rect p_rect, uint32_t& p_outlineThickness, std::string p_content, std::pair<color, color> p_normalColors, std::pair<color, color> p_hoveringColors, std::pair<color, color> p_pressedColors)
    :outline(true), content(p_content), outlineThinkness(p_outlineThickness), normalColors(p_normalColors), hoveringColors(p_hoveringColors), pressedColors(p_pressedColors) {
        objRect = p_rect;
    }

    void rectButton::render() {
        if (state == BUTTONNORMAL) {
            if (outline)
                objRect.render(&normalColors.first, normalColors.second, outlineThinkness);
            else
                objRect.render(&normalColors.first);
        }
        else if (state == BUTTONHOVERING) {
            if (outline)
                objRect.render(&hoveringColors.first, hoveringColors.second, outlineThinkness);
            else
                objRect.render(&hoveringColors.first);
        }
        else if (state == BUTTONPRESSED) {
            if (outline)
                objRect.render(&pressedColors.first, pressedColors.second, outlineThinkness);
            else
                objRect.render(&pressedColors.first);
        }
    }
    void rectButton::update() {
        if (SDL_PointInRect(eventHandler::get().mousePos.getSDLPoint(), objRect.getSDLRect())) {
            state = BUTTONHOVERING;
            if ((eventHandler::get().mouseButtons & SDL_BUTTON_LMASK) != 0)
                state = BUTTONPRESSED;
            window::get().renderState = true;
        }
        else {
            state = BUTTONNORMAL;
            window::get().renderState = true;
        }
    }
}