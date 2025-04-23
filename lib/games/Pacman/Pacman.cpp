#include "Pacman.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>

namespace arcade {

Pacman::Pacman() : 
    _pacman{Position(10, 10), Color(255, 255, 0), 1},
    _state(GameState::MENU),
    _score(0),
    _cellSize(20),
    _moveTimer(0),
    _moveInterval(0.5f) {
    srand(time(nullptr));
}

void Pacman::init() {
    _gameWidth = 20;
    _gameHeight = 20;
    
    _pacman.position = Position(_gameWidth / 2, _gameHeight / 2);
    _pacman.color = Color(255, 255, 0);
    _pacman.speed = 1;
    
    _ghosts.clear();
    _ghosts.push_back({{2, 2}, Color(255, 0, 0), 1});
    _ghosts.push_back({{_gameWidth-3, 2}, Color(255, 192, 203), 1});
    _ghosts.push_back({{2, _gameHeight-3}, Color(0, 255, 255), 1});
    _ghosts.push_back({{_gameWidth-3, _gameHeight-3}, Color(255, 165, 0), 1});
    
    spawnDots();
    spawnWalls();
    _state = GameState::PLAYING;
    _score = 0;
}

void Pacman::stop() {
    _state = GameState::MENU;
}

void Pacman::restart() {
    init();
}

void Pacman::update(float deltaTime) {
    if (_state != GameState::PLAYING) return;
    
    _moveTimer += deltaTime;
    if (_moveTimer >= _moveInterval) {
        _moveTimer = 0;
        
        for (auto& ghost : _ghosts) {
            moveGhost(ghost);
        }
    }
    
    checkCollisions();
    if (_dots.empty()) {
        _state = GameState::WIN;
    }
}

void Pacman::render(IGraphical &graphical) {
    graphical.clear();
    graphical.setCellSize(_cellSize);
    
    for (const auto& wall : _walls) {
        graphical.drawCell(wall.x, wall.y, Color(100, 100, 100));
    }
    for (const auto& dot : _dots) {
        Position dotPos;
        dotPos.x = dot.x * _cellSize + _cellSize/2;
        dotPos.y = dot.y * _cellSize + _cellSize/2;
        
        Color dotColor(255, 215, 0);
        
        graphical.drawCircle(dotPos, _cellSize/6, dotColor);
    }
    
    for (const auto& ghost : _ghosts) {
        graphical.drawCell(ghost.position.x, ghost.position.y, ghost.color);
    }
    
    graphical.drawCell(_pacman.position.x, _pacman.position.y, _pacman.color);
    
    Text scoreText("Score: " + std::to_string(_score), Position(0, 0), Color(255, 255, 255), 16);
    graphical.drawText(scoreText);
    graphical.display();
}

void Pacman::handleEvent(EventType event) {
    switch (event) {
        case EventType::MOVE_UP:
            updatePacmanPosition(event);
            break;
        case EventType::MOVE_DOWN:
            updatePacmanPosition(event);
            break;
        case EventType::MOVE_LEFT:
            updatePacmanPosition(event);
            break;
        case EventType::MOVE_RIGHT:
            updatePacmanPosition(event);
            break;
        case EventType::PAUSE:
            _state = (_state == GameState::PLAYING) ? GameState::PAUSED : GameState::PLAYING;
            break;
        default:
            break;
    }
}

void Pacman::updatePacmanPosition(EventType direction) {
    Position newPos = _pacman.position;

    switch (direction) {
        case EventType::MOVE_UP:    newPos.y--; break;
        case EventType::MOVE_DOWN:  newPos.y++; break;
        case EventType::MOVE_LEFT:  newPos.x--; break;
        case EventType::MOVE_RIGHT: newPos.x++; break;
        default: return;
    }

    if (!isCollision(newPos) && 
        newPos.x >= 1 && newPos.x < _gameWidth - 1 &&
        newPos.y >= 1 && newPos.y < _gameHeight - 1) {
        _pacman.position = newPos;
    }
}

void Pacman::checkCollisions() {
    auto dotIt = std::remove_if(_dots.begin(), _dots.end(), 
        [this](const Position& dot) {
            return dot.x == _pacman.position.x && dot.y == _pacman.position.y;
        });
    if (dotIt != _dots.end()) {
        _dots.erase(dotIt, _dots.end());
        _score += 10;
    }
    for (const auto& ghost : _ghosts) {
        if (ghost.position.x == _pacman.position.x && 
            ghost.position.y == _pacman.position.y) {
            _state = GameState::GAME_OVER;
            break;
        }
    }
}

void Pacman::spawnDots() {
    _dots.clear();
    for (int x = 1; x < _gameWidth - 1; x++) {
        for (int y = 1; y < _gameHeight - 1; y++) {
            if (!isCollision({x, y})) {
                _dots.push_back({x, y});
            }
        }
    }
}

void Pacman::spawnWalls() {
    _walls.clear();
    
    for (int x = 0; x < _gameWidth; x++) {
        _walls.push_back({x, 0});
        _walls.push_back({x, _gameHeight - 1});
    }
    for (int y = 0; y < _gameHeight; y++) {
        _walls.push_back({0, y});
        _walls.push_back({_gameWidth - 1, y});
    }
    for (int x = 5; x < 15; x += 4) {
        for (int y = 5; y < 15; y += 4) {
            _walls.push_back({x, y});
        }
    }
}

void Pacman::moveGhost(Entity& ghost) {
    Position oldPos = ghost.position;
    int direction = rand() % 4;

    switch (direction) {
        case 0: ghost.position.y -= ghost.speed; break;
        case 1: ghost.position.x += ghost.speed; break;
        case 2: ghost.position.y += ghost.speed; break;
        case 3: ghost.position.x -= ghost.speed; break;
    }

    if (isCollision(ghost.position) || 
        ghost.position.x < 1 || ghost.position.x >= _gameWidth - 1 ||
        ghost.position.y < 1 || ghost.position.y >= _gameHeight - 1) {
        ghost.position = oldPos;
    }
}

bool Pacman::isCollision(const Position& pos) {
    return std::any_of(_walls.begin(), _walls.end(), 
        [&pos](const Position& wall) {
            return wall.x == pos.x && wall.y == pos.y;
        });
}

GameState Pacman::getState() const {
    return _state;
}

int Pacman::getScore() const {
    return _score;
}

void Pacman::setState(GameState state) {
    _state = state;
}

std::string Pacman::getName() const {
    return "Pacman";
}

std::string Pacman::getDescription() const {
    return "Classic Pacman game where you eat dots and avoid ghosts";
}

extern "C" {
    arcade::IGame* create() {
        return new arcade::Pacman();
    }
    
    void destroy(arcade::IGame* obj) {
        delete obj;
    }
}
}