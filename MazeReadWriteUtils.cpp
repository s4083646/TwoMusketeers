#include "MazeReadWriteUtils.h"
#include <limits>
#include <tuple>
#include <vector>
#include <iostream>
#include <queue>
#include <set>
#include <cmath>

bool ReadBasePoint(mcpp::Coordinate& start) {
    std::string input;
    std::cout << "In Minecraft, navigate to where you need the maze\n"
                 "to be built and type - done: ";
    std::cin >> input;
    if (input != "done") return false;

    mcpp::MinecraftConnection mc;
    start = mc.getPlayerPosition() + mcpp::Coordinate(1, 0, 1);
    return true;
}

bool readLengthWidth(unsigned int& xlength, unsigned int& zlength) {
    std::cout << "Enter the length and width of maze: ";
    std::cin >> xlength >> zlength;

    if (xlength == 0 || zlength == 0 || xlength % 2 == 0 || zlength % 2 == 0) {
        std::cout << "Length and width should be non-zero odd numbers!" << std::endl;
        return false;
    }
    return true;
}

bool readMaze(std::vector<std::vector<char>>& maze, mcpp::Coordinate& basePoint) {
    unsigned int xlength = 0, zlength = 0;
    if (!ReadBasePoint(basePoint)) return false;
    if (!readLengthWidth(xlength, zlength)) return false;

    std::cout << "Enter the maze structure: \n";
    std::cin.ignore();
    maze.clear();

    for (unsigned int z = 0; z < zlength; ++z) {
        std::string line;
        std::getline(std::cin, line);
        if (line.length() != xlength) {
            std::cout << "Error: Line " << z+1 << " must be " << xlength << " characters." << std::endl;
            return false;
        }

        std::vector<char> row;
        for (char c : line) {
            if (c != 'x' && c != '.') {
                std::cout << "Error: Invalid character '" << c << "'. Use 'x' and '.' only." << std::endl;
                return false;
            }
            row.push_back(c);
        }
        maze.push_back(row);
    }

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
    } else {
        fixMaze(maze);
    }

    return true;
}

void printMaze(std::vector<std::vector<char>>& maze) {
    std::cout << "**Printing Maze Structure**\n";
    for (const auto& row : maze) {
        for (char c : row) std::cout << c;
        std::cout << std::endl;
    }
    std::cout << "**End Printing Maze**\n\n";
}

bool floodFill(std::vector<std::vector<char>>& maze, int x, int z, char target, char marker) {
    if (maze[z][x] != target) return false;

    int width = maze[0].size(), height = maze.size();
    std::queue<std::pair<int, int>> q;
    q.push({x, z});
    maze[z][x] = marker;

    while (!q.empty()) {
        auto [cx, cz] = q.front(); q.pop();
        for (auto [dx, dz] : std::vector<std::pair<int, int>>{{-1,0},{1,0},{0,-1},{0,1}}) {
            int nx = cx + dx, nz = cz + dz;
            if (nx >= 0 && nx < width && nz >= 0 && nz < height && maze[nz][nx] == target) {
                maze[nz][nx] = marker;
                q.push({nx, nz});
            }
        }
    }
    return true;
}

int countEntrances(const std::vector<std::vector<char>>& maze) {
    int entrances = 0, height = maze.size(), width = maze[0].size();
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

bool validateMaze(std::vector<std::vector<char>>& maze) {
    std::vector<std::vector<char>> temp = maze;

    if (countEntrances(maze) != 1) return false;

    if (!floodFill(temp, 1, 1, '.', 'o')) return false;
    for (const auto& row : temp)
        for (char c : row)
            if (c == '.') return false;

    temp = maze;
    floodFill(temp, 0, 0, 'x', '#');
    for (const auto& row : temp)
        for (char c : row)
            if (c == 'x') return false;

    return true;
}

void fixMaze(std::vector<std::vector<char>>& maze) {
    int height = maze.size(), width = maze[0].size();
    std::vector<std::vector<char>> visited(height, std::vector<char>(width, 0));

    floodFill(maze, 1, 1, '.', 'o');

    bool changed = true;
    while (changed) {
        changed = false;
        for (int z = 1; z < height - 1; ++z) {
            for (int x = 1; x < width - 1; ++x) {
                if (maze[z][x] == '.') {
                    std::vector<std::vector<char>> temp = maze;
                    if (!floodFill(temp, x, z, '.', 'x')) continue;

                    int minDist = 10000, bestX = -1, bestZ = -1;
                    for (int tz = 1; tz < height - 1; ++tz) {
                        for (int tx = 1; tx < width - 1; ++tx) {
                            if (maze[tz][tx] == 'o') {
                                int dist = abs(tx - x) + abs(tz - z);
                                dist += (tx != x && tz != z) ? 1 : 0;
                                if (dist < minDist) {
                                    minDist = dist;
                                    bestX = tx;
                                    bestZ = tz;
                                }
                            }
                        }
                    }

                    if (bestX != -1 && bestZ != -1) {
                        int cx = x, cz = z;
                        while (cx != bestX || cz != bestZ) {
                            maze[cz][cx] = '.';
                            if (std::abs(cx - bestX) >= std::abs(cz - bestZ)) {
                                cx += (bestX > cx) ? 1 : -1;
                            } else {
                                cz += (bestZ > cz) ? 1 : -1;
                            }
                        }
                        maze[bestZ][bestX] = '.';
                        floodFill(maze, 1, 1, '.', 'o');
                        changed = true;
                    }
                }
            }
        }
    }

    for (auto& row : maze)
        for (auto& cell : row)
            if (cell == 'o') cell = '.';

    if (countEntrances(maze) != 1) {
        for (int i = 0; i < width; ++i) {
            if (maze[0][i] == '.') maze[0][i] = 'x';
            if (maze[height - 1][i] == '.') maze[height - 1][i] = 'x';
        }
        for (int i = 0; i < height; ++i) {
            if (maze[i][0] == '.') maze[i][0] = 'x';
            if (maze[i][width - 1] == '.') maze[i][width - 1] = 'x';
        }
        for (int i = 1; i < height - 1; ++i) {
            if (maze[i][1] == '.') {
                maze[i][0] = '.';
                break;
            }
        }
    }

    bool hasExit = false;
    for (int z = 1; z < height - 1; ++z) {
        if (maze[z][width - 1] == '.' || maze[z][width - 2] == '.') {
            hasExit = true;
            break;
        }
    }

    if (!hasExit) {
        for (int z = height - 2; z > 0; --z) {
            if (maze[z][width - 2] == '.') {
                maze[z][width - 1] = '.';
                std::cout << "[FixMaze] Forced exit at [" << z << "][" << width - 1 << "]\n";
                break;
            }
        }
    }
}
