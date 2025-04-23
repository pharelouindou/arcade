#include "SDL2.hpp"
#include <stdexcept>
#include <iostream>

namespace arcade {

SDL2Graphical::SDL2Graphical() : 
    _window(nullptr),
    _renderer(nullptr),
    _font(nullptr),
    _isRunning(false),
    _width(0),
    _height(0),
    _cellSize(20) {}

SDL2Graphical::~SDL2Graphical() {
    if (_isRunning) {
        close();
    }
}

void SDL2Graphical::init(int width, int height, const std::string &title) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("SDL could not initialize!");
    }
    if (TTF_Init() < 0) {
        throw std::runtime_error("SDL_ttf could not initialize!");
    }

    _window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    if (!_window) {
        throw std::runtime_error("Window could not be created!");
    }
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!_renderer) {
        throw std::runtime_error("Renderer could not be created!");
    }
    _font = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", 24);
    if (!_font) {
        std::cerr << "Warning: Failed to load font from assets/fonts/DejaVuSans.ttf" << std::endl;
        _font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
        if (!_font) {
            throw std::runtime_error("Failed to load font!");
        }
    }
    _width = width;
    _height = height;
    _isRunning = true;
}

void SDL2Graphical::close() {
    if (!_isRunning) 
    return;
    
    _isRunning = false;    
    if (_font) {
        TTF_CloseFont(_font);
        _font = nullptr;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

bool SDL2Graphical::isOpen() const {
    return _isRunning;
}

void SDL2Graphical::clear() {
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
}

void SDL2Graphical::display() {
    SDL_RenderPresent(_renderer);
    SDL_Delay(1000/60);
}

EventType SDL2Graphical::getEvent() {
    SDL_Event event;
    
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return EventType::QUIT;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP: return EventType::MOVE_UP;
                    case SDLK_DOWN: return EventType::MOVE_DOWN;
                    case SDLK_LEFT: return EventType::MOVE_LEFT;
                    case SDLK_RIGHT: return EventType::MOVE_RIGHT;
                    case SDLK_RETURN: return EventType::ACTION;
                    case SDLK_SPACE: return EventType::ACTION;
                    case SDLK_p: return EventType::PAUSE;
                    case SDLK_m: return EventType::MENU;
                    case SDLK_n: return EventType::NEXT_LIB;
                    case SDLK_b: return EventType::PREV_LIB;
                    case SDLK_RIGHTBRACKET: return EventType::NEXT_GAME;
                    case SDLK_LEFTBRACKET: return EventType::PREV_GAME;
                    case SDLK_ESCAPE: return EventType::QUIT;
                }
        }
    }
    return EventType::NONE;
}

void SDL2Graphical::drawRectangle(const Position &pos, int width, int height, const Color &color) {
    SDL_Rect rect = {pos.x, pos.y, width, height};
    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(_renderer, &rect);
}

void SDL2Graphical::drawCircle(const Position &pos, int radius, const Color &color) {
    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(_renderer, pos.x + dx, pos.y + dy);
            }
        }
    }
}

void SDL2Graphical::drawSprite(const Sprite &sprite) {
    SDL_Rect rect = {sprite.position.x, sprite.position.y, _cellSize, _cellSize};
    SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(_renderer, &rect);
}

void SDL2Graphical::drawText(const Text &text) {
    SDL_Color sdlColor = {text.color.r, text.color.g, text.color.b, text.color.a};
    SDL_Surface* surface = TTF_RenderText_Blended(_font, text.content.c_str(), sdlColor);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
        if (texture) {
            SDL_Rect rect = {text.position.x, text.position.y, surface->w, surface->h};
            SDL_RenderCopy(_renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void SDL2Graphical::drawCell(int x, int y, const Color &color) {
    SDL_Rect rect = {x * _cellSize, y * _cellSize, _cellSize, _cellSize};
    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(_renderer, &rect);
}

void SDL2Graphical::setCellSize(int size) {
    _cellSize = size;
}

std::string SDL2Graphical::getName() const {
    return "SDL2";
}

void SDL2Graphical::drawMenuBackground() {
    for (int y = 0; y < _height; ++y) {
        float factor = static_cast<float>(y) / _height;
        SDL_SetRenderDrawColor(_renderer, 
            static_cast<Uint8>(50 * (1-factor)), 
            static_cast<Uint8>(100 * (1-factor)), 
            static_cast<Uint8>(150 * (1-factor)), 
            255);
        SDL_RenderDrawLine(_renderer, 0, y, _width, y);
    }
}

void SDL2Graphical::drawGameFrame() {
    SDL_SetRenderDrawColor(_renderer, 100, 100, 100, 255);
    SDL_Rect border = {10, 10, _width - 20, _height - 20};
    SDL_RenderDrawRect(_renderer, &border);
}

void SDL2Graphical::createCustomTexture(const std::string& text, const Color& color, SDL_Texture** texture, int* w, int* h) {
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderText_Blended(_font, text.c_str(), sdlColor);
    if (surface) {
        *texture = SDL_CreateTextureFromSurface(_renderer, surface);
        *w = surface->w;
        *h = surface->h;
        SDL_FreeSurface(surface);
    }
}

void SDL2Graphical::drawAnimatedText(const Text& text) {
    static float time = 0;
    time += 0.016f;

    SDL_Color sdlColor = {
        text.color.r,
        text.color.g,
        text.color.b,
        static_cast<Uint8>(128 + 127 * sin(time * 3))
    };

    SDL_Surface* surface = TTF_RenderText_Blended(_font, text.content.c_str(), sdlColor);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
        if (texture) {
            float scale = 1.0f + 0.1f * sin(time * 2);
            SDL_Rect rect = {
                text.position.x,
                text.position.y,
                static_cast<int>(surface->w * scale),
                static_cast<int>(surface->h * scale)
            };
            SDL_RenderCopy(_renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

extern "C" {
    arcade::IGraphical* create() {
        return new arcade::SDL2Graphical();
    }

    void destroy(arcade::IGraphical* obj) {
        delete obj;
    }
}

}
