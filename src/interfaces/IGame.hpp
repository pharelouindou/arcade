#ifndef IGAME_HPP_
#define IGAME_HPP_

#include "IGraphical.hpp"
#include <string>
#include <memory>

namespace arcade {

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    WIN
};

class IGame {
public:
    virtual ~IGame() = default;
    
    virtual void init() = 0;
    virtual void stop() = 0;
    virtual void restart() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(IGraphical &graphical) = 0;
    virtual void handleEvent(EventType event) = 0;
    virtual GameState getState() const = 0;
    virtual int getScore() const = 0;
    virtual void setState(GameState state) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

}

#endif