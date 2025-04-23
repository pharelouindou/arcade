#ifndef MAP_LOADER_HPP_
#define MAP_LOADER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace arcade {

class MapLoader {
public:
    static std::vector<std::string> loadMap(const std::string& filename) {
        std::vector<std::string> map;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            file.open("assets/maps/" + filename);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open map file: " + filename);
            }
        }
        
        std::string line;
        while (std::getline(file, line)) {
            map.push_back(line);
        }
        
        if (map.empty()) {
            throw std::runtime_error("Map file is empty: " + filename);
        }
        
        return map;
    }

    static std::string getDefaultMap(const std::string& gameName) {
        if (gameName == "Pacman") {
            return std::string(
                "####################\n"
                "#........##........#\n"
                "#.####...##...####.#\n"
                "#.#  #...##...#  #.#\n"
                "#.####...##...####.#\n"
                "#..................#\n"
                "#.####.######.####.#\n"
                "#....#...##...#....#\n"
                "####.#...##...#.####\n"
                "#....#...##...#....#\n"
                "#.####.######.####.#\n"
                "#..................#\n"
                "#.####...##...####.#\n"
                "#.#  #...##...#  #.#\n"
                "#.####...##...####.#\n"
                "#........##........#\n"
                "####################\n"
            );
        }
        return std::string(
            "####################\n"
            "#                  #\n"
            "#   ##########     #\n"
            "#                  #\n"
            "#     ##########   #\n"
            "#                  #\n"
            "#   ##########     #\n"
            "#                  #\n"
            "#     ##########   #\n"
            "#                  #\n"
            "####################\n"
        );
    }
};

} 

#endif
