#include "Nibbler.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "../../../src/interfaces/MapLoader.hpp"

namespace arcade {

Nibbler::Nibbler()
    : _state(GameState::MENU), _score(0), _level(1), _cellSize(20),
      _gridWidth(30), _gridHeight(20), _timeAccumulator(0.0f), _moveInterval(0.15f) {
    srand(time(nullptr));
}

void Nibbler::init() {
    _nibbler.clear();
    _nibbler.push_back({Position(_gridWidth / 2, _gridHeight / 2)});
    _direction = Position(1, 0);
    _score = 0;
    _level = 1;
    loadMapForLevel(_level);
    spawnFood();
    _state = GameState::PLAYING;
    _timeAccumulator = 0.0f;
}

void Nibbler::loadLevel(int level) {
    _walls.clear();
    for (int x = 0; x < _gridWidth; ++x) {
        _walls.push_back({x, 0});
        _walls.push_back({x, _gridHeight - 1});
    }
    for (int y = 0; y < _gridHeight; ++y) {
        _walls.push_back({0, y});
        _walls.push_back({_gridWidth - 1, y});
    }
    for (int i = 0; i < level * 2; ++i) {
        int x = 2 + rand() % (_gridWidth - 4);
        int y = 2 + rand() % (_gridHeight - 4);
        _walls.push_back({x, y});
    }
}

void Nibbler::loadMapForLevel(int level) {
    try {
        std::string mapFile = "assets/maps/nibbler/level" + std::to_string(level) + ".txt";
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
        loadLevel(level);
    }
}

void Nibbler::update(float deltaTime) {
    if (_state != GameState::PLAYING) return;

    _timeAccumulator += deltaTime;
    if (_timeAccumulator >= _moveInterval) {
        _timeAccumulator -= _moveInterval;
        moveNibbler();

        if (checkCollision()) {
            _state = GameState::GAME_OVER;
            return;
        }

        if (_nibbler.front().position.x == _food.x && _nibbler.front().position.y == _food.y) {
            _score += 10 * _level;
            _nibbler.push_back({_nibbler.back().position});
            
            if (_score >= _level * 100) {
                _level++;
                loadLevel(_level);
                _moveInterval = std::max(0.05f, 0.15f - (_level * 0.01f));
            }
            spawnFood();
        }
    }
}

void Nibbler::render(IGraphical &graphical) {
    graphical.clear();
    graphical.setCellSize(_cellSize);

    for (size_t y = 0; y < _currentMap.size(); ++y) {
        for (size_t x = 0; x < _currentMap[y].size(); ++x) {
            switch (_currentMap[y][x]) {
                case '#':
                    graphical.drawCell(x, y, Color(128, 128, 128));
                    break;
                case '.':
                    graphical.drawCell(x, y, Color(50, 50, 50));
                    break;
            }
        }
    }

    for (const auto &segment : _nibbler) {
        graphical.drawCell(segment.position.x, segment.position.y, Color(0, 255, 0));
    }

    graphical.drawCell(_food.x, _food.y, Color(255, 0, 0));

    graphical.drawText(Text("Score: " + std::to_string(_score), Position(10, 10), Color(255, 255, 255), 16));
    graphical.drawText(Text("Level: " + std::to_string(_level), Position(10, 30), Color(255, 255, 255), 16));

    graphical.display();
}

void Nibbler::stop() {
    _state = GameState::MENU;
}

void Nibbler::restart() {
    init();
}

void Nibbler::handleEvent(EventType event) {
    if (_state != GameState::PLAYING) return;

    switch (event) {
        case EventType::MOVE_UP:
            if (_direction.y == 0) _direction = Position(0, -1);
            break;
        case EventType::MOVE_DOWN:
            if (_direction.y == 0) _direction = Position(0, 1);
            break;
        case EventType::MOVE_LEFT:
            if (_direction.x == 0) _direction = Position(-1, 0);
            break;
        case EventType::MOVE_RIGHT:
            if (_direction.x == 0) _direction = Position(1, 0);
            break;
        case EventType::PAUSE:
            _state = GameState::PAUSED;
            break;
        default:
            break;
    }
}

void Nibbler::moveNibbler() {
    Position newHead = _nibbler.front().position;
    newHead.x += _direction.x;
    newHead.y += _direction.y;

    _nibbler.push_front({newHead});
    _nibbler.pop_back();
}

bool Nibbler::checkCollision() {
    const Position &head = _nibbler.front().position;

    for (const auto &wall : _walls) {
        if (head.x == wall.x && head.y == wall.y) {
            return true;
        }
    }

    for (size_t i = 1; i < _nibbler.size(); ++i) {
        if (head.x == _nibbler[i].position.x && head.y == _nibbler[i].position.y) {
            return true;
        }
    }

    return false;
}

bool Nibbler::isCollision(const Position& pos) const {
    for (const auto& wall : _walls) {
        if (pos.x == wall.x && pos.y == wall.y) {
            return true;
        }
    }
    return false;
}

void Nibbler::spawnFood() {
    bool validPosition;
    int maxAttempts = 100;
    int attempts = 0;

    do {
        attempts++;
        int minX = 2;
        int maxX = _gridWidth - 3;
        int minY = 2;
        int maxY = _gridHeight - 3;

        _food.x = minX + rand() % (maxX - minX + 1);
        _food.y = minY + rand() % (maxY - minY + 1);
        
        validPosition = true;
        
        if (isCollision(_food)) {
            validPosition = false;
            continue;
        }
        
        for (const auto &segment : _nibbler) {
            if (segment.position.x == _food.x && segment.position.y == _food.y) {
                validPosition = false;
                break;
            }
        }

        if (attempts >= maxAttempts) {
            for (int x = 2; x < _gridWidth - 2; x++) {
                for (int y = 2; y < _gridHeight - 2; y++) {
                    Position testPos = {x, y};
                    if (!isCollision(testPos)) {
                        bool snakeCollision = false;
                        for (const auto &segment : _nibbler) {
                            if (segment.position.x == x && segment.position.y == y) {
                                snakeCollision = true;
                                break;
                            }
                        }
                        if (!snakeCollision) {
                            _food = testPos;
                            return;
                        }
                    }
                }
            }
            break;
        }
    } while (!validPosition);
}

GameState Nibbler::getState() const {
    return _state;
}

int Nibbler::getScore() const {
    return _score;
}

void Nibbler::setState(GameState state) {
    _state = state;
}

std::string Nibbler::getName() const {
    return "Nibbler";
}

std::string Nibbler::getDescription() const {
    return "Classic Nibbler game with walls and levels";
}

}

arcade::IGame *create()
{
    return new arcade::Nibbler();
}

void destroy(arcade::IGame *game)
{
    delete game;
}