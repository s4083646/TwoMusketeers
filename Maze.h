#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <string>
#include <mcpp/mcpp.h>

class Maze {
public:
    Maze();
    Maze(mcpp::Coordinate basePoint, unsigned int xlen, unsigned int zlen, bool mode);
    ~Maze();

    void buildMaze(const std::vector<std::string>& maze, int length, int width, mcpp::Coordinate buildStart);
    void teleportPlayerToRandomDot(const std::vector<std::string>& maze, mcpp::Coordinate basePoint);
    void generateRandomMaze(std::vector<std::string>& maze, int height, int width, bool testMode);
    void undoChanges();

private:
    void divideRegion(std::vector<std::string>& maze, int top, int bottom, int left, int right, bool testMode, bool horizontal);
    void saveBlockChange(const mcpp::Coordinate& coord);

    struct BlockChange {
        mcpp::Coordinate coord;
        mcpp::BlockType original;
    };

    BlockChange changes[10000];  // Limited to 10,000 changes
    int changeCount = 0;

    mcpp::MinecraftConnection mc;
};

#endif // MAZE_H
