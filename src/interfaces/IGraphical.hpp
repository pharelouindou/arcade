#ifndef IGRAPHICAL_HPP_
#define IGRAPHICAL_HPP_

#include <string>
#include <vector>
#include <memory>

namespace arcade {

enum class EventType {
    NONE,
    QUIT,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    ACTION,
    PAUSE,
    MENU,
    NEXT_LIB,
    PREV_LIB,
    NEXT_GAME,
    PREV_GAME
};

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    Color(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255)
        : r(r), g(g), b(b), a(a) {}
};

struct Position {
    int x;
    int y;

    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Sprite {
    std::string path;
    Position position;
    
    Sprite(const std::string &path = "", const Position &position = Position())
        : path(path), position(position) {}
};

struct Text {
    std::string content;
    Position position;
    Color color;
    unsigned int size;
    
    Text(const std::string &content = "", const Position &position = Position(), 
         const Color &color = Color(255, 255, 255), unsigned int size = 16)
        : content(content), position(position), color(color), size(size) {}
};

class IGraphical {
public:
    virtual ~IGraphical() = default;

    virtual void init(int width, int height, const std::string &title) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual void clear() = 0;
    virtual void display() = 0;
    
    virtual EventType getEvent() = 0;
    
    virtual void drawRectangle(const Position &pos, int width, int height, const Color &color) = 0;
    virtual void drawCircle(const Position &pos, int radius, const Color &color) = 0;
    virtual void drawSprite(const Sprite &sprite) = 0;
    virtual void drawText(const Text &text) = 0;
    
    virtual void drawCell(int x, int y, const Color &color) = 0;
    virtual void setCellSize(int size) = 0;
    
    virtual std::string getName() const = 0;
};

}

#endif