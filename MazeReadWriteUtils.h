#ifndef MAZEREADWRITEUTILS_H
#define MAZEREADWRITEUTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <queue>
#include "mcpp/mcpp.h"

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

bool readLengthWidth(unsigned int& xlength, unsigned int& zlength) {
    std::cout << "Enter the length and width of maze: ";
    std::cin >> xlength >> zlength;

    if (xlength < 3 || zlength < 3 || xlength % 2 == 0 || zlength % 2 == 0) {
        std::cout << "Length and width must be odd numbers >= 3!" << std::endl;
        return false;
    }
    return true;
}

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

bool validateMaze(const std::vector<std::vector<char>>& M, int& entX, int& entZ, bool& hasLoops, bool& hasIsolated) {
    int Z = M.size();
    int X = M[0].size();
    int exitCount = 0;

    hasLoops = false;
    hasIsolated = false;
    entX = entZ = -1;

    // Check left wall for forbidden entrances
    for (int r = 0; r < Z; ++r) {
        if (M[r][0] == '.') {
            std::cout << "Invalid: entrance detected on left wall at row " << r << std::endl;
            return false;
        }
        if (M[r][X - 1] == 'e') {
            ++exitCount;
            entZ = r;
            entX = X - 1;
        }
    }

    if (exitCount != 1) {
        std::cout << "Error: Maze must contain exactly ONE exit 'e' on the right wall." << std::endl;
        return false;
    }

    int walkable = 0;
    for (int z = 0; z < Z; ++z)
        for (int x = 0; x < X; ++x)
            if (M[z][x] == '.' || M[z][x] == 'e') ++walkable;

    std::vector<std::vector<bool>> visited(Z, std::vector<bool>(X, false));
    std::queue<std::pair<int, int>> q;
    q.push({entZ, entX});
    visited[entZ][entX] = true;

    int reachable = 0;
    while (!q.empty()) {
        auto [z, x] = q.front(); q.pop();
        ++reachable;

        for (auto [dz, dx] : std::vector<std::pair<int, int>>{{1,0},{-1,0},{0,1},{0,-1}}) {
            int nz = z + dz, nx = x + dx;
            if (nz >= 0 && nz < Z && nx >= 0 && nx < X &&
                !visited[nz][nx] && (M[nz][nx] == '.' || M[nz][nx] == 'e')) {
                visited[nz][nx] = true;
                q.push({nz, nx});
            }
        }
    }

    if (reachable != walkable) {
        std::cout << "Error: Some path tiles are unreachable from exit 'e'." << std::endl;
        hasIsolated = true;
        return false;
    }

    return true;
}

#endif // MAZEREADWRITEUTILS_H
