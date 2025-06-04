#ifndef ASSIGN_MAZE_H
#define ASSIGN_MAZE_H

#include <iostream>
#include <vector>
#include <mcpp/mcpp.h>

struct BlockChange {
    mcpp::Coordinate coord;
    mcpp::BlockType original;
};

class Maze {
public:
    Maze(mcpp::Coordinate basePoint, unsigned int xlen, unsigned int zlen, bool mode);
    Maze();
    ~Maze();

    void buildMaze(const std::vector<std::string>& maze, int length, int width, mcpp::Coordinate buildStart);
    void teleportPlayerToRandomDot(const std::vector<std::string>& maze, mcpp::Coordinate buildStart);
    void generateRandomMaze(std::vector<std::string>& maze, int length, int width);
    void recursiveBacktrack(std::vector<std::string>& maze, int length, int width, int x, int y);

    void saveBlockChange(const mcpp::Coordinate& coord);
    void undoChanges();

private:
    mcpp::MinecraftConnection mc;
    BlockChange changes[10000];
    int changeCount = 0;
};

#endif //ASSIGN_MAZE_H
