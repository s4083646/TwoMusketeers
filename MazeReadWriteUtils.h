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
bool ReadBasePoint(mcpp::Coordinate& start);
bool readLengthWidth(unsigned int& xlength, unsigned int& zlength);
bool readMaze(std::vector<std::vector<char>>& maze, mcpp::Coordinate& basePoint);
void printMaze(std::vector<std::vector<char>>& maze);

#endif // MAZE_READ_WRITE_UTILS_H
