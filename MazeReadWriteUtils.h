#ifndef MAZEREADWRITEUTILS_H
#define MAZEREADWRITEUTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <queue>
#include "mcpp/mcpp.h"

// === Base point reading ===
bool ReadBasePoint(mcpp::Coordinate& start) {
    std::string input;
    mcpp::MinecraftConnection mc;

    std::cout << "Stand in Minecraft and type 'done': ";
    std::cin >> input;
    if (input != "done") {
        std::cout << "Error: type 'done' to set base point." << std::endl;
        return false;
    }

    start = mc.getPlayerPosition() + mcpp::Coordinate(1, 0, 1);
    return true;
}

// === Length & Width ===
bool readLengthWidth(unsigned int& xlength, unsigned int& zlength) {
    std::cout << "Enter the length and width of maze: ";
    std::cin >> xlength >> zlength;

    if (xlength < 3 || zlength < 3 || xlength % 2 == 0 || zlength % 2 == 0) {
        std::cout << "Length and width must be odd numbers >= 3!" << std::endl;
        return false;
    }
    return true;
}

// === Maze Reading ===
bool readMaze(std::vector<std::vector<char>>& maze, mcpp::Coordinate& basePoint) {
    unsigned int xlength = 0, zlength = 0;

    if (!ReadBasePoint(basePoint) || !readLengthWidth(xlength, zlength))
        return false;

    std::cout << "Enter the maze structure row-by-row (x . e):\n";

    maze.resize(zlength);
    for (unsigned int z = 0; z < zlength; ++z) {
        maze[z].resize(xlength);
        for (unsigned int x = 0; x < xlength; ++x) {
            char c;
            std::cin >> c;
            if (c != 'x' && c != '.' && c != 'e') {
                std::cout << "Invalid character: " << c << ". Allowed: x . e" << std::endl;
                return false;
            }
            maze[z][x] = c;
        }
    }

    return true;
}

// === Maze Validation (unchanged) ===
// [...keep your validation logic here...]

// === Maze Printing Functions ===
inline void printMaze(const std::vector<std::vector<char>>& mazeVec) {
    std::cout << "**Printing Maze Structure**\n";
    for (const auto& row : mazeVec) {
        for (char c : row) {
            std::cout << c;
        }
        std::cout << "\n";
    }
    std::cout << "**End Printing Maze**\n";
}

inline void printMaze(const std::vector<std::string>& maze) {
    std::cout << "**Printing Maze Structure**\n";
    for (const auto& row : maze) {
        std::cout << row << "\n";
    }
    std::cout << "**End Printing Maze**\n";
}

#endif // MAZEREADWRITEUTILS_H
