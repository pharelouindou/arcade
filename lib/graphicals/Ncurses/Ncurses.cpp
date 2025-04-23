#include "Ncurses.hpp"
#include <ncurses.h>
#include <algorithm>

namespace arcade {

NcursesGraphical::NcursesGraphical() : 
    _mainWindow(nullptr), 
    _isRunning(false), 
    _width(0), 
    _height(0),
    _cellSize(20) {}

NcursesGraphical::~NcursesGraphical() {
    if (_isRunning) {
        close();
    }
}

void NcursesGraphical::init(int width, int height, const std::string &title) {
    (void)title;
    initscr();
    if (!stdscr) {
        throw std::runtime_error("Failed to initialize NCurses");
    }
    start_color();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    timeout(16);
    keypad(stdscr, TRUE);
    curs_set(0);
    _width = width;
    _height = height;
    _mainWindow = newwin(_height, _width, 0, 0);
    if (!_mainWindow) {
        endwin();
        throw std::runtime_error("Failed to create NCurses window");
    }
    keypad(_mainWindow, TRUE);
    initColors();
    clear();
    refresh();
    _isRunning = true;
}

void NcursesGraphical::close() {
    if (!_isRunning) return;
    
    _isRunning = false;
    
    if (_mainWindow) {
        werase(_mainWindow);
        wrefresh(_mainWindow);
        delwin(_mainWindow);
        _mainWindow = nullptr;
    }
    clear();
    refresh();
    endwin();
}

bool NcursesGraphical::isOpen() const {
    return _isRunning && _mainWindow != nullptr;
}

void NcursesGraphical::clear() {
    wclear(_mainWindow);
    werase(stdscr);
    refresh();
}

void NcursesGraphical::display() {
    box(_mainWindow, 0, 0);
    wrefresh(_mainWindow);
    refresh();
    napms(16);
    flushinp();
}

EventType NcursesGraphical::getEvent() {
    if (!_mainWindow || !_isRunning) {
        return EventType::QUIT;
    }

    int ch = wgetch(_mainWindow);
    if (ch == ERR) {
        return EventType::NONE;
    }

    switch (ch) {
        case 27:
        case 'q': return EventType::QUIT;
        case KEY_UP: return EventType::MOVE_UP;
        case KEY_DOWN: return EventType::MOVE_DOWN;
        case KEY_LEFT: return EventType::MOVE_LEFT;
        case KEY_RIGHT: return EventType::MOVE_RIGHT;
        case '\n':
        case KEY_ENTER: return EventType::ACTION;
        case ' ': return EventType::ACTION;
        case 'p': return EventType::PAUSE;
        case 'm': return EventType::MENU;
        case 'n': return EventType::NEXT_LIB;
        case 'b': return EventType::PREV_LIB;
        case ']': return EventType::NEXT_GAME;
        case '[': return EventType::PREV_GAME;
        default: return EventType::NONE;
    }
}

void NcursesGraphical::drawRectangle(const Position &pos, int width, int height, const Color &color) {
    short colorPair = getColorPair(color);
    wattron(_mainWindow, COLOR_PAIR(colorPair));
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            mvwaddch(_mainWindow, pos.y + y, pos.x + x, ' ');
        }
    }
    
    wattroff(_mainWindow, COLOR_PAIR(colorPair));
}

void NcursesGraphical::drawCircle(const Position &pos, int radius, const Color &color) {
    short colorPair = getColorPair(color);
    wattron(_mainWindow, COLOR_PAIR(colorPair));
    
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x*x + y*y <= radius*radius) {
                mvwaddch(_mainWindow, pos.y + y, pos.x + x, ' ');
            }
        }
    }
    
    wattroff(_mainWindow, COLOR_PAIR(colorPair));
}

void NcursesGraphical::drawSprite(const Sprite &sprite) {
    mvwaddch(_mainWindow, sprite.position.y, sprite.position.x, 'O');
}

void NcursesGraphical::drawText(const Text &text) {
    short colorPair = getColorPair(text.color);
    wattron(_mainWindow, COLOR_PAIR(colorPair));
    mvwprintw(_mainWindow, text.position.y / 16, text.position.x / 16, "%s", text.content.c_str());
    wattroff(_mainWindow, COLOR_PAIR(colorPair));
    wrefresh(_mainWindow);
}

void NcursesGraphical::drawCell(int x, int y, const Color &color) {
    if (!_mainWindow) return;
    
    short colorPair = getColorPair(color);
    wattron(_mainWindow, COLOR_PAIR(colorPair) | A_REVERSE);
    mvwaddch(_mainWindow, y, x, ' ');
    wattroff(_mainWindow, COLOR_PAIR(colorPair) | A_REVERSE);
    wrefresh(_mainWindow);
}

void NcursesGraphical::setCellSize(int size) {
    _cellSize = size;
}

std::string NcursesGraphical::getName() const {
    return "NCurses";
}

void NcursesGraphical::initColors() {
    if (has_colors()) {
        init_pair(1, COLOR_RED, COLOR_RED);
        init_pair(2, COLOR_GREEN, COLOR_GREEN);
        init_pair(3, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(4, COLOR_BLUE, COLOR_BLUE);
        init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);
        init_pair(6, COLOR_CYAN, COLOR_CYAN);
        init_pair(7, COLOR_WHITE, COLOR_WHITE);
    }
}

short NcursesGraphical::getColorPair(const Color& color) {
    if (color.r > 127 && color.g < 127 && color.b < 127) return 1;
    if (color.r < 127 && color.g > 127 && color.b < 127) return 2;
    if (color.r > 127 && color.g > 127 && color.b < 127) return 3;
    if (color.r < 127 && color.g < 127 && color.b > 127) return 4;
    if (color.r > 127 && color.g < 127 && color.b > 127) return 5;
    if (color.r < 127 && color.g > 127 && color.b > 127) return 6;
    if (color.r > 127 && color.g > 127 && color.b > 127) return 7;
    return 0;
}

extern "C" {
    arcade::IGraphical* create() {
        return new arcade::NcursesGraphical();
    }

    void destroy(arcade::IGraphical* obj) {
        delete obj;
    }
}

}
