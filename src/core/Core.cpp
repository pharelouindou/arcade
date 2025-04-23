#include "Core.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <filesystem>
#include "DLLoader.hpp"

namespace arcade
{

    Core::Core(const std::string &initialGraphical, const std::string &initialGame)
        : _currentGraphicalIndex(0), _currentGameIndex(0), _state(GameState::MENU)
    {
        loadLibraries();

        auto graphicalIt = std::find(_graphicalLibs.begin(), _graphicalLibs.end(), initialGraphical);
        if (graphicalIt != _graphicalLibs.end())
        {
            _currentGraphicalIndex = std::distance(_graphicalLibs.begin(), graphicalIt);
        }

        auto gameIt = std::find(_gameLibs.begin(), _gameLibs.end(), initialGame);
        if (gameIt != _gameLibs.end())
        {
            _currentGameIndex = std::distance(_gameLibs.begin(), gameIt);
        }
    }

    Core::~Core()
    {
        stop();
    }

    void Core::loadLibraries()
    {
        _graphicalLibs.clear();
        _gameLibs.clear();

        for (const auto &entry : std::filesystem::recursive_directory_iterator("./lib/"))
        {
            if (!entry.is_regular_file() || entry.path().extension() != ".so")
                continue;

            std::string filename = entry.path().filename().string();
            std::string filepath = entry.path().string();

            if (filename.find("arcade_ncurses") != std::string::npos ||
                filename.find("arcade_sfml") != std::string::npos ||
                filename.find("arcade_sdl2") != std::string::npos)
            {
                _graphicalLibs.push_back(filepath);
            }
            else if (filename.find("arcade_snake") != std::string::npos ||
                     filename.find("arcade_pacman") != std::string::npos ||
                     filename.find("arcade_nibbler") != std::string::npos)
            {
                _gameLibs.push_back(filepath);
            }
        }

        std::sort(_graphicalLibs.begin(), _graphicalLibs.end());
        std::sort(_gameLibs.begin(), _gameLibs.end());
    }

    void Core::init()
    {
        try
        {
            if (!_graphicalLibs.empty())
            {
                _graphicalLoader = std::make_unique<DLLoader<IGraphical>>(_graphicalLibs[_currentGraphicalIndex]);
                auto graphical = _graphicalLoader->getInstance();
                if (!graphical)
                {
                    throw std::runtime_error("Failed to get graphical instance");
                }
                graphical->init(800, 600, "Arcade");
                if (!graphical->isOpen())
                {
                    throw std::runtime_error("Graphical library failed to open window");
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error loading graphical library: " << e.what() << std::endl;
            stop();
            return;
        }
    }

    void Core::stop()
    {
        if (_gameLoader)
        {
            try
            {
                auto game = _gameLoader->getInstance();
                if (game)
                {
                    game->stop();
                }
            }
            catch (...)
            {
            }
            _gameLoader.reset();
        }

        std::ofstream highScoresFile("highscores.txt");
        if (highScoresFile.is_open())
        {
            for (const auto &entry : _highScores)
            {
                highScoresFile << entry.first << " " << entry.second << std::endl;
            }
            highScoresFile.close();
        }

        if (_graphicalLoader)
        {
            try
            {
                auto graphical = _graphicalLoader->getInstance();
                if (graphical && graphical->isOpen())
                {
                    graphical->close();
                }
            }
            catch (const std::exception &e)
            {
            }
            _graphicalLoader.reset();
        }
    }

    void Core::switchGraphical(int direction)
    {
        if (_graphicalLibs.empty())
        {
            std::cerr << "No graphical libraries available" << std::endl;
            return;
        }

        std::cout << "Starting graphical switch..." << std::endl;

        size_t newIndex = (_currentGraphicalIndex + direction + _graphicalLibs.size()) % _graphicalLibs.size();
        std::string newLibPath = _graphicalLibs[newIndex];

        std::cout << "Switching from lib " << _currentGraphicalIndex << " to lib " << newIndex << ": " << newLibPath << std::endl;

        size_t oldIndex = _currentGraphicalIndex;
        GameState previousState = _state;
        std::unique_ptr<DLLoader<IGame>> gameLoaderBackup;

        if (_gameLoader)
        {
            try
            {
                auto game = _gameLoader->getInstance();
                if (game && game->getState() != GameState::MENU)
                {
                    updateHighScores(game->getName(), game->getScore());
                    game->stop();
                    gameLoaderBackup = std::move(_gameLoader);
                }
                else
                {
                    game->stop();
                    _gameLoader.reset();
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error stopping game: " << e.what() << std::endl;
                _gameLoader.reset();
            }
        }

        std::unique_ptr<DLLoader<IGraphical>> oldGraphicalLoader;

        if (_graphicalLoader)
        {
            try
            {
                auto oldGraphical = _graphicalLoader->getInstance();
                if (oldGraphical && oldGraphical->isOpen())
                {
                    std::cout << "Closing " << oldGraphical->getName() << std::endl;
                    oldGraphical->close();
                }
                oldGraphicalLoader = std::move(_graphicalLoader);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error closing old library: " << e.what() << std::endl;
                _graphicalLoader.reset();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        try
        {
            std::cout << "Loading new graphical library: " << newLibPath << std::endl;
            _graphicalLoader = std::make_unique<DLLoader<IGraphical>>(newLibPath);
            auto newGraphical = _graphicalLoader->getInstance();

            if (!newGraphical)
            {
                throw std::runtime_error("Failed to get new graphical instance");
            }

            std::cout << "Successfully created instance: " << newGraphical->getName() << std::endl;

            int width = static_cast<int>(800 * 1.28);
            int height = static_cast<int>(600 * 1.28);
            std::string title = "Arcade";

            std::cout << "Initializing new graphical library..." << std::endl;
            newGraphical->init(width, height, title);

            for (int attempts = 0; attempts < 5; attempts++)
            {
                if (newGraphical->isOpen())
                {
                    break;
                }
                std::cout << "Waiting for window to open, attempt " << (attempts + 1) << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

            if (!newGraphical->isOpen())
            {
                throw std::runtime_error("New graphical window failed to open");
            }

            _currentGraphicalIndex = newIndex;
            std::cout << "Graphical library switched successfully" << std::endl;

            newGraphical->clear();
            newGraphical->drawText(Text("Switching libraries...", Position(350, 300), Color(255, 255, 255), 24));
            newGraphical->display();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            if (!newGraphical->isOpen())
            {
                throw std::runtime_error("Window closed unexpectedly after initial display");
            }

            if (previousState != GameState::MENU && gameLoaderBackup)
            {
                try
                {
                    _gameLoader = std::move(gameLoaderBackup);
                    auto game = _gameLoader->getInstance();
                    if (game)
                    {
                        game->init();
                        game->setState(previousState);
                        _state = previousState;
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error restoring game: " << e.what() << std::endl;
                    _state = GameState::MENU;
                }
            }
            else
            {
                _state = GameState::MENU;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Critical error during graphical switch: " << e.what() << std::endl;

            if (oldGraphicalLoader)
            {
                try
                {
                    std::cout << "Attempting to restore previous graphical library..." << std::endl;
                    _graphicalLoader = std::move(oldGraphicalLoader);
                    auto oldGraphical = _graphicalLoader->getInstance();
                    if (!oldGraphical->isOpen())
                    {
                        oldGraphical->init(static_cast<int>(800 * 1.28), static_cast<int>(600 * 1.28), "Arcade (Recovery)");
                    }
                    _currentGraphicalIndex = oldIndex;
                    _state = GameState::MENU;
                    return;
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Failed to restore previous library: " << e.what() << std::endl;
                }
            }

            try
            {
                std::cout << "Attempting to load alternative graphical library..." << std::endl;
                for (size_t i = 0; i < _graphicalLibs.size(); ++i)
                {
                    if (i != _currentGraphicalIndex)
                    {
                        try {
                            _graphicalLoader = std::make_unique<DLLoader<IGraphical>>(_graphicalLibs[i]);
                            auto fallback = _graphicalLoader->getInstance();
                            fallback->init(800, 600, "Arcade (Fallback)");
                            _currentGraphicalIndex = i;
                            _state = GameState::MENU;
                            return;
                        } catch (...) {
                            continue;
                        }
                    }
                }
                throw std::runtime_error("No working graphical library found");
            }
            catch (const std::exception &e)
            {
                std::cerr << "All fallback attempts failed: " << e.what() << std::endl;
            }
        }
    }

    void Core::switchGame(int direction)
    {
        if (_gameLibs.empty())
            return;

        if (_gameLoader)
        {
            try
            {
                auto game = _gameLoader->getInstance();
                if (game)
                {
                    if (game->getState() != GameState::MENU)
                    {
                        updateHighScores(game->getName(), game->getScore());
                    }
                    game->stop();
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error stopping current game: " << e.what() << std::endl;
            }
            _gameLoader.reset();
        }

        _currentGameIndex = (_currentGameIndex + direction + _gameLibs.size()) % _gameLibs.size();

        try
        {
            std::cout << "Loading game: " << _gameLibs[_currentGameIndex] << std::endl;
            _gameLoader = std::make_unique<DLLoader<IGame>>(_gameLibs[_currentGameIndex]);
            auto game = _gameLoader->getInstance();

            if (!game)
            {
                throw std::runtime_error("Failed to get game instance");
            }

            std::cout << "Initializing game..." << std::endl;
            game->init();
            game->setState(GameState::PLAYING);
            _state = GameState::PLAYING;
            std::cout << "Game switched successfully" << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error switching game: " << e.what() << std::endl;
            _state = GameState::MENU;
        }
    }

    void Core::showMenu()
    {
        if (!_graphicalLoader)
            return;

        auto graphical = _graphicalLoader->getInstance();
        if (!graphical)
            return;

        graphical->clear();

        graphical->drawText(Text("ARCADE", Position(350, 50), Color(255, 255, 0), 48));

        graphical->drawText(Text("Graphical Libraries (Left/Right to switch):", Position(100, 150), Color(255, 255, 255), 18));
        for (size_t i = 0; i < _graphicalLibs.size(); ++i) {
            std::string name = std::filesystem::path(_graphicalLibs[i]).filename().string();
            Color color(255, 255, 255);
            if (i == _currentGraphicalIndex) {
                color = Color(0, 255, 0);
            }
            graphical->drawText(Text(name, Position(100 + i * 250, 200), color, 16));
        }
        graphical->drawText(Text("Games (Up/Down to switch):", Position(100, 300), Color(255, 255, 255), 18));
        for (size_t i = 0; i < _gameLibs.size(); ++i) {
            std::string name = std::filesystem::path(_gameLibs[i]).filename().string();
            Color color = (i == _currentGameIndex) ? Color(0, 255, 0) : Color(255, 255, 255);
            graphical->drawText(Text(name, Position(100, 350 + i * 50), color, 16));
        }
        graphical->drawText(Text("Instructions:", Position(100, 500), Color(255, 255, 255), 18));
        graphical->drawText(Text("Use Arrow Keys to navigate", Position(100, 530), Color(255, 255, 255), 16));
        graphical->drawText(Text("Press ENTER to start the selected game", Position(100, 560), Color(255, 255, 255), 16));
        graphical->drawText(Text("Press ESC to quit", Position(100, 590), Color(255, 255, 255), 16));
        if (!_highScores.empty()) {
            displayHighScores(*graphical);
        }
        graphical->display();
        EventType event = graphical->getEvent();
        switch (event) {
        case EventType::MOVE_UP:
            if (!_gameLibs.empty()) {
                _currentGameIndex = (_currentGameIndex + _gameLibs.size() - 1) % _gameLibs.size();
            }
            break;
        case EventType::MOVE_DOWN:
            if (!_gameLibs.empty()) {
                _currentGameIndex = (_currentGameIndex + 1) % _gameLibs.size();
            }
            break;
        case EventType::MOVE_LEFT:
            std::cout << "Switching to previous graphical library." << std::endl;
            switchGraphical(-1);
            break;
        case EventType::MOVE_RIGHT:
            std::cout << "Switching to next graphical library." << std::endl;
            switchGraphical(1);
            break;
        case EventType::ACTION:
            if (!_gameLibs.empty()) {
                try
                {
                    if (_gameLoader && _gameLibs[_currentGameIndex] == _gameLoader->getPath())
                    {
                        auto game = _gameLoader->getInstance();
                        game->restart();
                        game->setState(GameState::PLAYING);
                        _state = GameState::PLAYING;
                    }
                    else
                    {
                        std::cout << "Loading game: " << _gameLibs[_currentGameIndex] << std::endl;
                        _gameLoader = std::make_unique<DLLoader<IGame>>(_gameLibs[_currentGameIndex]);
                        auto game = _gameLoader->getInstance();
                        if (!game)
                        {
                            std::cerr << "Failed to get game instance" << std::endl;
                            return;
                        }
                        std::cout << "Game loaded successfully" << std::endl;
                        game->init();
                        game->setState(GameState::PLAYING);
                        _state = GameState::PLAYING;
                    }
                } catch (const std::exception &e){
                    std::cerr << "Error starting game: " << e.what() << std::endl;
                }
            }
            break;
        case EventType::QUIT:
            if (graphical->isOpen())
            {
                graphical->close();
            }
            break;
        default:
            break;
        }
    }

    void Core::run()
    {
        if (!_graphicalLoader) {
            std::cerr << "No graphical library loaded" << std::endl;
            return;
        }
        auto graphical = _graphicalLoader->getInstance();
        if (!graphical) {
            std::cerr << "Failed to get graphical instance" << std::endl;
            return;
        }
        _lastUpdateTime = std::chrono::steady_clock::now();
        _state = GameState::MENU;
        while (graphical->isOpen()) {
            auto currentTime = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - _lastUpdateTime).count();
            deltaTime = std::min(deltaTime, 1.0f / 30.0f);
            _lastUpdateTime = currentTime;
            EventType event = graphical->getEvent();
            if (event == EventType::QUIT) {
                if (graphical) {
                    graphical->close();
                }
                break;
            }
            if (event == EventType::NEXT_LIB) {
                std::cout << "Switching to next graphical library via hotkey." << std::endl;
                switchGraphical(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            } else if (event == EventType::PREV_LIB) {
                std::cout << "Switching to previous graphical library via hotkey." << std::endl;
                switchGraphical(-1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            } else if (event == EventType::NEXT_GAME) {
                switchGame(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            } else if (event == EventType::PREV_GAME) {
                switchGame(-1);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            } else if (event == EventType::MENU) {
                _state = GameState::MENU;
                if (_gameLoader) {
                    try
                    {
                        auto game = _gameLoader->getInstance();
                        if (game) {
                            game->stop();
                        }
                    }
                    catch (...){
                    }
                }
                continue;
            }
            if (_state == GameState::MENU) {
                showMenu();
            } else {
                if (_gameLoader) {
                    auto game = _gameLoader->getInstance();
                    if (game) {
                        if (event == EventType::PAUSE) {
                            GameState currentState = game->getState();
                            game->setState(currentState == GameState::PLAYING ? GameState::PAUSED : GameState::PLAYING);
                        } else{
                            game->handleEvent(event);
                        }
                        game->update(deltaTime);
                        if (!graphical->isOpen()) {
                            break;
                        }
                        game->render(*graphical);
                        graphical->drawText(Text("Score: " + std::to_string(game->getScore()),
                                                 Position(50, 50), Color(255, 255, 0), 16));

                        if (game->getState() == GameState::GAME_OVER || game->getState() == GameState::WIN)
                        {
                            graphical->drawText(Text(game->getState() == GameState::GAME_OVER ? "GAME OVER" : "YOU WIN",
                                                     Position(350, 250), Color(255, 0, 0), 32));
                            graphical->drawText(Text("Final Score: " + std::to_string(game->getScore()),
                                                     Position(350, 300), Color(255, 255, 255), 24));
                            graphical->drawText(Text("Press ENTER to return to menu",
                                                     Position(350, 350), Color(255, 255, 255), 18));
                            graphical->drawText(Text("Press R to restart game",
                                                     Position(350, 380), Color(255, 255, 255), 18));
                            graphical->display();

                            updateHighScores(game->getName(), game->getScore());

                            bool waitingForInput = true;
                            while (waitingForInput && graphical->isOpen()){
                                EventType endEvent = graphical->getEvent();
                                if (endEvent == EventType::ACTION || endEvent == EventType::MENU){
                                    _state = GameState::MENU;
                                    waitingForInput = false;
                                }
                                else if (endEvent == EventType::QUIT) {
                                    return;
                                } else if (endEvent == EventType::PAUSE){
                                    try
                                    {
                                        game->restart();
                                        game->setState(GameState::PLAYING);
                                        waitingForInput = false;
                                    }
                                    catch (const std::exception &e)
                                    {
                                        std::cerr << "Error restarting game: " << e.what() << std::endl;
                                    }
                                }
                                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                            }
                        }
                    } else {
                        std::cerr << "Game instance is null" << std::endl;
                        _state = GameState::MENU;
                    }
                } else {
                    std::cerr << "Game loader is null" << std::endl;
                    _state = GameState::MENU;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    void Core::updateHighScores(const std::string &gameName, int score) {
        auto it = std::find_if(_highScores.begin(), _highScores.end(),
                               [&gameName](const auto &entry)
                               { return entry.first == gameName; });

        if (it != _highScores.end()) {
            if (score > it->second) {
                it->second = score;
            }
        } else {
            _highScores.emplace_back(gameName, score);
        }

        std::sort(_highScores.begin(), _highScores.end(),
                  [](const auto &a, const auto &b)
                  { return a.second > b.second; });

        if (_highScores.size() > 10) {
            _highScores.resize(10);
        }
    }

    void Core::displayHighScores(IGraphical &graphical)
    {
        graphical.drawText(Text("High Scores:", Position(500, 300), Color(255, 255, 255), 18));

        for (size_t i = 0; i < _highScores.size() && i < 5; ++i) {
            std::string name = _highScores[i].first;
            int score = _highScores[i].second;
            graphical.drawText(Text(name + ": " + std::to_string(score), Position(500, 330 + i * 30), Color(255, 255, 255), 16));
        }
    }
}
