#include "Snake.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "../../../src/interfaces/MapLoader.hpp"

namespace arcade {

Snake::Snake() 
    : _state(GameState::MENU), _score(0), _cellSize(20), _gridWidth(20), _gridHeight(20), _timeAccumulator(0.0f), _moveInterval(0.2f) {
    srand(time(nullptr));
}

void Snake::init() {
    _snake.clear();
    _snake.push_back({Position(_gridWidth / 2, _gridHeight / 2)});
    _direction = Position(0, 0);
    loadMapForLevel(1);
    spawnFood();
    _state = GameState::PLAYING;
    _score = 0;
    _timeAccumulator = 0.0f;
    _moveInterval = 0.2f;
}

void Snake::stop() {
    _state = GameState::MENU;
}

void Snake::restart() {
    init();
}

void Snake::update(float deltaTime) {
    if (_state != GameState::PLAYING || (_direction.x == 0 && _direction.y == 0)) {
        return;
    }

    _timeAccumulator += deltaTime;
    if (_timeAccumulator >= _moveInterval) {
        _timeAccumulator -= _moveInterval;
        moveSnake();

        if (checkCollision()) {
            _state = GameState::GAME_OVER;
            return;
        }

        if (_snake.front().position.x == _food.x && _snake.front().position.y == _food.y) {
            _score += 10;
            _snake.push_back({_snake.back().position});
            spawnFood();
        }
    }
}

void Snake::render(IGraphical &graphical) {
    graphical.clear();
    graphical.setCellSize(_cellSize);

    for (const auto& wall : _walls) {
        graphical.drawCell(wall.x, wall.y, Color(128, 128, 128));
    }

    for (const auto &segment : _snake) {
        graphical.drawCell(segment.position.x, segment.position.y, Color(0, 255, 0));
    }

    graphical.drawCell(_food.x, _food.y, Color(255, 0, 0));
    graphical.drawText(Text("Score: " + std::to_string(_score), Position(10, 10), Color(255, 255, 255), 16));
    graphical.display();
}

void Snake::handleEvent(EventType event) {
    if (_state != GameState::PLAYING) return;

    Position newDir = _direction;
    switch (event) {
        case EventType::MOVE_UP:
            if (_direction.y != 1) newDir = Position(0, -1);
            break;
        case EventType::MOVE_DOWN:
            if (_direction.y != -1) newDir = Position(0, 1);
            break;
        case EventType::MOVE_LEFT:
            if (_direction.x != 1) newDir = Position(-1, 0);
            break;
        case EventType::MOVE_RIGHT:
            if (_direction.x != -1) newDir = Position(1, 0);
            break;
        case EventType::PAUSE:
            _state = GameState::PAUSED;
            break;
        default:
            break;
    }
    
    if (newDir.x != 0 || newDir.y != 0) {
        _direction = newDir;
    }
}

void Snake::spawnFood() {
    bool validPosition;
    do {
        _food.x = 1 + rand() % (_gridWidth - 2);
        _food.y = 1 + rand() % (_gridHeight - 2);
        
        validPosition = true;
        
        if (isWall(_food.x, _food.y)) {
            validPosition = false;
            continue;
        }
        
        for (const auto &segment : _snake) {
            if (segment.position.x == _food.x && segment.position.y == _food.y) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

void Snake::moveSnake() {
    Position newHead = _snake.front().position;
    newHead.x += _direction.x;
    newHead.y += _direction.y;

    _snake.push_front({newHead});
    _snake.pop_back();
}

bool Snake::checkCollision() {
    const auto &head = _snake.front().position;

    if (isWall(head.x, head.y)) {
        return true;
    }

    for (size_t i = 1; i < _snake.size(); ++i) {
        if (_snake[i].position.x == head.x && _snake[i].position.y == head.y) {
            return true;
        }
    }

    return false;
}

GameState Snake::getState() const {
    return _state;
}

int Snake::getScore() const {
    return _score;
}

void Snake::setState(GameState state) {
    _state = state;
}

std::string Snake::getName() const {
    return "Snake";
}

std::string Snake::getDescription() const {
    return "Classic Snake game where you eat food and grow longer.";
}

void Snake::loadMapForLevel(int level) {
    try {
        std::string mapFile = "assets/maps/snake/level" + std::to_string(level) + ".txt";
        _currentMap = MapLoader::loadMap(mapFile);
        
        _walls.clear();
        for (size_t y = 0; y < _currentMap.size(); ++y) {
            for (size_t x = 0; x < _currentMap[y].size(); ++x) {
                if (_currentMap[y][x] == '#') {
                    _walls.push_back({static_cast<int>(x), static_cast<int>(y)});
                }
            }
        }
    } catch (const std::exception& e) {
        initializeWalls();
    }
}

void Snake::initializeWalls() {
    _walls.clear();
    for (int x = 0; x < _gridWidth; ++x) {
        _walls.push_back({x, 0});
        _walls.push_back({x, _gridHeight - 1});
    }
    for (int y = 0; y < _gridHeight; ++y) {
        _walls.push_back({0, y});
        _walls.push_back({_gridWidth - 1, y});
    }
}

bool Snake::isWall(int x, int y) const {
    return std::find_if(_walls.begin(), _walls.end(),
        [x, y](const Position& wall) {
            return wall.x == x && wall.y == y;
        }) != _walls.end();
}

extern "C" {
    arcade::IGame* create() {
        return new arcade::Snake();
    }

    void destroy(arcade::IGame* game) {
        delete game;
    }
}
}

