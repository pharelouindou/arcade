#ifndef SFML_HPP_
#define SFML_HPP_

#include "../../../src/interfaces/IGraphical.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <map>

namespace arcade {

class SFMLGraphical : public IGraphical {
public:
    SFMLGraphical();
    ~SFMLGraphical() override;

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
    sf::RenderWindow _window;
    int _cellSize;
};

extern "C" {
    arcade::IGraphical* create() {
        return new arcade::SFMLGraphical();
    }

    void destroy(arcade::IGraphical* obj) {
        delete obj;
    }
}

}

#endif
