#ifndef MAZE_READ_WRITE_UTILS_H
#define MAZE_READ_WRITE_UTILS_H

#include <iostream>
#include <vector>
#include <queue>
#include <mcpp/mcpp.h>

// === Utility function declarations ===
bool validateMaze(std::vector<std::vector<char>>& maze);
void fixMaze(std::vector<std::vector<char>>& maze);
bool floodFill(std::vector<std::vector<char>>& maze, int x, int z, char target, char marker);
int countEntrances(const std::vector<std::vector<char>>& maze);

// === Input Functions ===
bool ReadBasePoint(mcpp::Coordinate& start) {
    std::string input = "";
    bool success = true;

    mcpp::MinecraftConnection mc;
    std::cout << "In Minecraft, navigate to where you need the maze\nto be built and type - done: " << std::endl;

    std::cin >> input;
    if (std::cin.fail()) {
        success = false;
    }

    if (success) {
        start = mc.getPlayerPosition() + mcpp::Coordinate(1, 0, 1);
    } else {
        std::cout << "Basepoint is not correct." << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    return success;
}

bool readLengthWidth(unsigned int& xlength, unsigned int& zlength) {
    bool success = true;

    std::cout << "Enter the length and width of maze: " << std::endl;
    std::cin >> xlength >> zlength;

    if (xlength == 0 || zlength == 0 || xlength % 2 == 0 || zlength % 2 == 0) {
        success = false;
        std::cout << "Length and width should be non-zero odd numbers!" << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    return success;
}

bool readMaze(std::vector<std::vector<char>>& maze, mcpp::Coordinate& basePoint) {
    unsigned int xlength = 0, zlength = 0;
    bool success = ReadBasePoint(basePoint);

    if (success) {
        success = readLengthWidth(xlength, zlength);
    }

    if (success) {
        std::cout << "Enter the maze structure: " << std::endl;

        maze.resize(zlength);
        unsigned int charsRead = 0;
        char readC;

        for (unsigned int z = 0; z < zlength; z++) {
            maze[z].resize(xlength);
            for (unsigned int x = 0; x < xlength; x++) {
                std::cin >> readC;
                if (readC == 'x' || readC == '.') {
                    maze[z][x] = readC;
                    ++charsRead;
                }
            }
        }

        if (charsRead != (xlength * zlength)) {
            std::cout << "Structure not correct - only read " << charsRead << " characters." << std::endl;
            success = false;
        }
    }

    if (success) {
        if (!validateMaze(maze)) {
            std::cout << "Errors detected. Would you like to automatically fix them? (y/n): ";
            char option;
            std::cin >> option;
            if (option == 'y' || option == 'Y') {
                fixMaze(maze);
                std::cout << "Maze fixed." << std::endl;
            } else {
                std::cout << "Maze rejected. Please enter a new maze." << std::endl;
                return false;
            }
        }
    }

    return success;
}

void printMaze(std::vector<std::vector<char>>& maze) {
    std::cout << "**Printing Maze Structure**" << std::endl;
    for (const auto& row : maze) {
        for (char c : row) std::cout << c;
        std::cout << std::endl;
    }
    std::cout << "**End Printing Maze**" << std::endl << std::endl;
}

// === Validation Logic ===
bool floodFill(std::vector<std::vector<char>>& maze, int x, int z, char target, char marker) {
    if (maze[z][x] != target) return false;

    int width = maze[0].size(), height = maze.size();
    std::queue<std::pair<int, int>> q;
    q.push({x, z});
    maze[z][x] = marker;

    while (!q.empty()) {
        auto [cx, cz] = q.front(); q.pop();
        for (int dx : {-1, 1, 0, 0}) {
            for (int dz : {0, 0, -1, 1}) {
                if (abs(dx) + abs(dz) != 1) continue;
                int nx = cx + dx, nz = cz + dz;
                if (nx >= 0 && nx < width && nz >= 0 && nz < height && maze[nz][nx] == target) {
                    maze[nz][nx] = marker;
                    q.push({nx, nz});
                }
            }
        }
    }
    return true;
}

bool validateMaze(std::vector<std::vector<char>>& maze) {
    int height = maze.size(), width = maze[0].size();
    std::vector<std::vector<char>> temp = maze;

    // Find entrance
    int entranceCount = countEntrances(maze);
    if (entranceCount != 1) return false;

    // Flood fill for isolation
    bool found = false;
    for (int z = 0; z < height && !found; ++z)
        for (int x = 0; x < width && !found; ++x)
            if (temp[z][x] == '.') {
                floodFill(temp, x, z, '.', 'o');
                found = true;
            }

    for (int z = 0; z < height; ++z)
        for (int x = 0; x < width; ++x)
            if (temp[z][x] == '.') return false; // isolated area

    // Loop check by flood-filling walls
    temp = maze;
    found = false;
    for (int z = 0; z < height && !found; ++z)
        for (int x = 0; x < width && !found; ++x)
            if (temp[z][x] == 'x') {
                floodFill(temp, x, z, 'x', '#');
                found = true;
            }

    for (int z = 0; z < height; ++z)
        for (int x = 0; x < width; ++x)
            if (temp[z][x] == 'x') return false; // disjoint wall structure

    return true;
}

void fixMaze(std::vector<std::vector<char>>& maze) {
    int height = maze.size(), width = maze[0].size();

    // Fix isolation: flood from first path, open nearby walls to connect others
    std::vector<std::vector<char>> temp = maze;
    floodFill(temp, 1, 1, '.', 'o');
    for (int z = 1; z < height - 1; ++z) {
        for (int x = 1; x < width - 1; ++x) {
            if (temp[z][x] == '.') {
                for (int dx : {-1, 1, 0, 0}) {
                    for (int dz : {0, 0, -1, 1}) {
                        if (abs(dx) + abs(dz) != 1) continue;
                        int nx = x + dx, nz = z + dz;
                        if (temp[nz][nx] == 'o') {
                            maze[(z + nz) / 2][(x + nx) / 2] = '.';
                            floodFill(temp, x, z, '.', 'o');
                            break;
                        }
                    }
                }
            }
        }
    }

    // Fix entrance
    int entrances = countEntrances(maze);
    if (entrances != 1) {
        // Close all, then open one
        for (int i = 0; i < width; ++i) {
            if (maze[0][i] == '.') maze[0][i] = 'x';
            if (maze[height - 1][i] == '.') maze[height - 1][i] = 'x';
        }
        for (int i = 0; i < height; ++i) {
            if (maze[i][0] == '.') maze[i][0] = 'x';
            if (maze[i][width - 1] == '.') maze[i][width - 1] = 'x';
        }
        // Open one randomly (here, simply first found)
        for (int z = 1; z < height - 1; ++z) {
            if (maze[z][0] == 'x' && maze[z][1] == '.') {
                maze[z][0] = '.';
                break;
            }
        }
    }
}

int countEntrances(const std::vector<std::vector<char>>& maze) {
    int entrances = 0;
    int height = maze.size(), width = maze[0].size();
    for (int i = 0; i < width; ++i) {
        if (maze[0][i] == '.') entrances++;
        if (maze[height - 1][i] == '.') entrances++;
    }
    for (int i = 0; i < height; ++i) {
        if (maze[i][0] == '.') entrances++;
        if (maze[i][width - 1] == '.') entrances++;
    }
    return entrances;
}

#endif // MAZE_READ_WRITE_UTILS_H
