#ifndef NCURSES_HPP_
#define NCURSES_HPP_

#include "../../../src/interfaces/IGraphical.hpp"
#include <ncurses.h>
#include <string>
#include <map>
#include <iostream>

namespace arcade {

class NcursesGraphical : public IGraphical {
public:
    NcursesGraphical();
    ~NcursesGraphical() override;

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
    WINDOW* _mainWindow;
    bool _isRunning;
    int _width;
    int _height;
    int _cellSize;

    void initColors();
    short getColorPair(const Color& color);
};

extern "C" {
    arcade::IGraphical* create();
    void destroy(arcade::IGraphical* obj);
}

} // namespace arcade

#endif // NCURSES_HPP_