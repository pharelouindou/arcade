/*
** EPITECH PROJECT, 2025
** add
** File description:
** arcade
*/

#include "Core.hpp"
#include <iostream>
#include <filesystem>

void displayUsage(const std::string &programName) {
    std::cout << "USAGE: " << programName << " path_to_graphical_lib" << std::endl;
    std::cout << "       path_to_graphical_lib: Path to a graphical library (.so file)" << std::endl;
}

void displayAvailableLibs() {
    std::cout << "Available libraries:" << std::endl;
    for (const auto &entry : std::filesystem::directory_iterator("./lib/")) {
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::cout << "  " << entry.path().string() << std::endl;
        }
    }
}

bool isGraphicalLibrary(const std::string &path) {
    return path.find("_ncurses.so") != std::string::npos ||
           path.find("_sfml.so") != std::string::npos ||
           path.find("_sdl2.so") != std::string::npos;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        displayUsage(argv[0]);
        displayAvailableLibs();
        return 84;
    }
    std::string graphicalLibPath = argv[1];
    if (!std::filesystem::exists(graphicalLibPath)) {
        std::cerr << "Error: Library file does not exist: " << graphicalLibPath << std::endl;
        displayAvailableLibs();
        return 84;
    }
    if (!isGraphicalLibrary(graphicalLibPath)) {
        std::cerr << "Error: '" << graphicalLibPath << "' not a graphical library" << std::endl;
        return 84;
    }
    try {
        arcade::Core core(graphicalLibPath, "");
        core.init();
        core.run();
    } catch (const std::exception &e) {
        return 84;
    }
    return 0;
}