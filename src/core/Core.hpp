#ifndef CORE_HPP_
#define CORE_HPP_

#include "DLLoader.hpp"
#include "../interfaces/IGraphical.hpp"
#include "../interfaces/IGame.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <filesystem>

namespace arcade {

class Core {
private:
    std::unique_ptr<DLLoader<IGraphical>> _graphicalLoader;
    std::vector<std::shared_ptr<IGraphical>> _previousGraphicals;
    std::unique_ptr<DLLoader<IGame>> _gameLoader;
    std::vector<std::string> _graphicalLibs;
    std::vector<std::string> _gameLibs;
    size_t _currentGraphicalIndex;
    size_t _currentGameIndex;
    GameState _state;
    std::chrono::steady_clock::time_point _lastUpdateTime;
    std::vector<std::pair<std::string, int>> _highScores;
    std::shared_ptr<IGraphical> _currentGraphical;
    std::shared_ptr<IGame> _currentGame;
    void loadLibraries();
    void switchGraphical(int direction);
    void switchGame(int direction);
    void showMenu();
    void updateHighScores(const std::string &gameName, int score);
    void displayHighScores(IGraphical &graphical);

public:
    Core(const std::string &initialGraphical, const std::string &initialGame);
    ~Core();
    void init();
    void run();
    void stop();
};

}

#endif