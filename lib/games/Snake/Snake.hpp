#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../../../src/interfaces/IGame.hpp"
#include "../../../src/interfaces/IGraphical.hpp"
#include <deque>
#include <vector>
#include <memory>

namespace arcade {

class Snake : public IGame {
public:
    Snake();
    ~Snake() override = default;

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

    std::deque<Segment> _snake;
    Position _food;
    Position _direction;
    GameState _state;
    int _score;
    int _cellSize;
    int _gridWidth;
    int _gridHeight;
    float _timeAccumulator;
    float _moveInterval;
    std::vector<std::string> _currentMap;
    std::vector<Position> _walls;
    void loadMapForLevel(int level);
    void initializeWalls();
    bool isWall(int x, int y) const;
    void spawnFood();
    void moveSnake();
    bool checkCollision();
};

} // namespace arcade

extern "C" {
    arcade::IGame* create();
    void destroy(arcade::IGame* game);
}

#endif
