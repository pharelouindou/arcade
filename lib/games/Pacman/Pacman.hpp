#ifndef PACMAN_HPP
#define PACMAN_HPP

#include "../../../src/interfaces/IGame.hpp"
#include "../../../src/interfaces/IGraphical.hpp"
#include <vector>
#include <memory>

namespace arcade {

class Pacman : public IGame {
public:
    Pacman();
    ~Pacman() override = default;

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
    struct Entity {
        Position position;
        Color color;
        int speed;
    };

    Entity _pacman;
    std::vector<Entity> _ghosts;
    std::vector<Position> _dots;
    std::vector<Position> _walls;
    GameState _state;
    int _score;
    int _cellSize;
    int _gameWidth;
    int _gameHeight;
    float _moveTimer;
    const float _moveInterval;

    void moveGhost(Entity& ghost);
    bool isCollision(const Position& pos);
    void updatePacmanPosition(EventType direction);
    void checkCollisions();
    void spawnDots();
    void spawnWalls();
};

extern "C" {
    IGame* createGame();
    void destroyGame(IGame* game);
}

} 

#endif