#include "SFML.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <filesystem>
namespace fs = std::filesystem;
namespace arcade {

SFMLGraphical::SFMLGraphical() : _cellSize(20) {}

SFMLGraphical::~SFMLGraphical() {
    close();
}

void SFMLGraphical::init(int width, int height, const std::string &title) {
    _window.create(sf::VideoMode(width, height), title);
    _window.setVerticalSyncEnabled(true);
    _window.setFramerateLimit(60);
}

void SFMLGraphical::close() {
    if (_window.isOpen()) {
        _window.close();
    }
}

bool SFMLGraphical::isOpen() const {
    return _window.isOpen();
}

void SFMLGraphical::clear() {
    _window.clear(sf::Color::Black);
}

void SFMLGraphical::display() {
    _window.display();
}

EventType SFMLGraphical::getEvent() {
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return EventType::QUIT;
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Escape: return EventType::QUIT;
                case sf::Keyboard::Up: return EventType::MOVE_UP;
                case sf::Keyboard::Down: return EventType::MOVE_DOWN;
                case sf::Keyboard::Left: return EventType::MOVE_LEFT;
                case sf::Keyboard::Right: return EventType::MOVE_RIGHT;
                case sf::Keyboard::Enter: return EventType::ACTION;
                default: return EventType::NONE;
            }
        }
    }
    return EventType::NONE;
}

void SFMLGraphical::drawRectangle(const Position &pos, int width, int height, const Color &color) {
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition(pos.x, pos.y);
    rectangle.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
    _window.draw(rectangle);
}

void SFMLGraphical::drawCircle(const Position &pos, int radius, const Color &color) {
    sf::CircleShape circle(radius);
    circle.setPosition(pos.x, pos.y);
    circle.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
    _window.draw(circle);
}

void SFMLGraphical::drawSprite(const Sprite &sprite) {
    sf::Texture texture;
    if (!texture.loadFromFile(sprite.path)) {
        std::cerr << "Failed to load sprite: " << sprite.path << std::endl;
        return;
    }
    sf::Sprite sfSprite(texture);
    sfSprite.setPosition(sprite.position.x, sprite.position.y);
    _window.draw(sfSprite);
}

void SFMLGraphical::drawText(const Text &text) {
    static sf::Font font;
    static bool fontLoaded = false;

    if (!fontLoaded) {
        const std::vector<std::string> fontPaths = {
            "assets/fonts/DejaVuSans.ttf",
            "../assets/fonts/DejaVuSans.ttf",
            "lib/graphicals/SFML/font3.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
        };

        for (const auto &path : fontPaths) {
            if (font.loadFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }

        if (!fontLoaded) {
            std::cerr << "Error: Could not load font from any location" << std::endl;
            return;
        }
    }

    sf::Text sfText(text.content, font, text.size);
    sfText.setPosition(text.position.x, text.position.y);
    sfText.setFillColor(sf::Color(text.color.r, text.color.g, text.color.b, text.color.a));
    _window.draw(sfText);
}

void SFMLGraphical::drawCell(int x, int y, const Color &color) {
    drawRectangle(Position(x * _cellSize, y * _cellSize), _cellSize, _cellSize, color);
}

void SFMLGraphical::setCellSize(int size) {
    _cellSize = size;
}

std::string SFMLGraphical::getName() const {
    return "SFML";
}

}
