#ifndef NIBBLER_HPP_
#define NIBBLER_HPP_

#include "../../../src/interfaces/IGame.hpp"
#include <deque>
#include <vector>

namespace arcade {

class Nibbler : public IGame {
public:
    Nibbler();
    ~Nibbler() override = default;

    void init() override;
    void stop() override;
    void restart() override;
    void update(float deltaTime) override;
    void render(IGraphical &graphical) override;
    void handleEvent(EventType event) override;

    GameState getState() const override;
    int getScore() const override;
    void setState(GameState state) override;
    std::string getName() const override;
    std::string getDescription() const override;

private:
    struct Segment {
        Position position;
    };

    std::deque<Segment> _nibbler;
    Position _food;
    Position _direction;
    std::vector<Position> _walls;
    GameState _state;
    int _score;
    int _level;
    int _cellSize;
    int _gridWidth;
    int _gridHeight;
    float _timeAccumulator;
    float _moveInterval;
    std::vector<std::string> _currentMap;
    void loadMapForLevel(int level);

    void loadLevel(int level);
    void spawnFood();
    void moveNibbler();
    bool checkCollision();
    bool isCollision(const Position& pos) const;
};

}

extern "C" {
    arcade::IGame* create();
    void destroy(arcade::IGame* game);
}

#endif
