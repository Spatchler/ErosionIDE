#pragma once
#include <iostream>
#include <vector>
#include <numeric>
#include <variant>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <tuple>
#include <string>
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
    class hotkey;

    //visitors
    struct renderProcess {
        void operator()(surface*);
        void operator()(obj*);
        void operator()(std::pair<rect*, color*> p);
    };
    struct updateProcess {
        void operator()(surface*);
        void operator()(obj*);
        void operator()(std::pair<rect*, color*> p);
    };

    class color {
    public:
        color() {}
        color(uint8_t p_r, uint8_t p_g, uint8_t p_b);
        color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a);

        SDL_Color* getSDLColor();

        uint8_t r, g, b, a;
    private:
        SDL_Color SDLColor;
    };

    class window {
    public:
        window(const window&) = delete;
        static window& get() { return instance; }
        void init(const char* p_title, const math::vec2i& p_size);

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs, bool p_first = false);

        void mvFocus(char v);
        void mvCurrentSurf(char v);
        void resizeCurrentSurf(char v);

        math::axis splitAxis;
        std::vector<uint8_t> splitRatio;
        std::vector<short> splitOffset;
        std::vector<surface*> layer;
        surface* currentSurf;
        bool running, initalized, renderState, updateState, updateSizes, outline;
        color outlineColor;
        uint32_t outlineThinkness;

        SDL_Renderer* getSDLRenderer();
        SDL_Window* getSDLWindow();
        
        ~window();
    private:
        window() {};
        static window instance;
        math::vec2i size;

        std::pair<char, bool> focusPos;

        SDL_Window* screen;
        SDL_Renderer* renderer;
    };

    class eventHandler {
    public:
        eventHandler(const eventHandler&) = delete;
        static eventHandler& get() { return instance; }

        void addHotkey(std::string h, const std::function<void()>& f);
        void update();

        bool keys[332];
        std::vector<uint32_t> keyBuffer;
        bool resize;
    private:
        eventHandler() {};
        static eventHandler instance;
        SDL_Event event;
        std::unordered_map<std::string, std::function<void()>> hotkeys;
        std::vector<bool> hotkeyStates;
    };

    class rect {
    public:
        rect(math::vec2i p_pos, math::vec2i p_size);

        void update();
        void render(color* c);
        void render(color* c, color& oc, uint32_t& ot);

        SDL_Rect* getSDLRect();

        bool filled;
        math::vec2i pos;
        math::vec2i size;
    private:
        SDL_Rect SDLRect;
    };

    class texture {
    public:
        texture(const char* p_filePath);

        SDL_Texture* getSDLTexture();
    private:
        SDL_Texture* sdlTexture;
    };
    
    class surface {
    public:
        surface();

        void render();
        void update();
        void split(std::vector<uint8_t> p_ratio, math::axis p_axis, std::vector<surface*> p_surfs);

        std::vector<std::pair<std::vector<uint8_t>, math::axis>> splits;
        std::vector<std::variant<surface*, obj*>> layer;
        bool enabled, outline;
        math::vec2i size;
        math::vec2i pos;
        color bgColor;
        rect r;
    private:
    };

    class obj {
    public:
        virtual void render() = 0;
        virtual void update() = 0;

        virtual ~obj() = default;
    private:
    };

    class button: public obj {
    public:
        button(math::vec2i p_size, texture* p_textureNormal, texture* p_textureHovering, texture* p_texturePressed);

        void render() override;
        void update() override;

        math::vec2i size;

        texture* textureNormal;
        texture* textureHovering;
        texture* texturePressed;

        rect buttonRect;
    private:
    };
}