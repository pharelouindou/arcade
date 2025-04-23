#ifndef SDL2_HPP_
#define SDL2_HPP_

#include "../../../src/interfaces/IGraphical.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>
#include <map>

namespace arcade {

class SDL2Graphical : public IGraphical {
public:
    SDL2Graphical();
    ~SDL2Graphical() override;

    void init(int width, int height, const std::string &title) override;
    void close() override;
    bool isOpen() const override;
    void clear() override;
    void display() override;
    
    EventType getEvent() override;
    
    void drawRectangle(const Position &pos, int width, int height, const Color &color) override;
    void drawCircle(const Position &pos, int radius, const Color &color) override;
    void drawSprite(const Sprite &sprite) override;
    void drawText(const Text &text) override;
    
    void drawCell(int x, int y, const Color &color) override;
    void setCellSize(int size) override;
    
    std::string getName() const override;

private:
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    TTF_Font* _font;
    bool _isRunning;
    int _width;
    int _height;
    int _cellSize;

    // Méthodes spécifiques à SDL2
    void drawMenuBackground();
    void drawGameFrame();
    void createCustomTexture(const std::string& text, const Color& color, SDL_Texture** texture, int* w, int* h);
    void drawAnimatedText(const Text& text);
};

}

extern "C" {
    arcade::IGraphical* create();
    void destroy(arcade::IGraphical* obj);
}

#endif
