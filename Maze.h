#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <string>
#include <mcpp/mcpp.h>

class Maze {
public:
    Maze();
    ~Maze();

    void generateRandomMaze(std::vector<std::string>& maze, int rows, int cols, bool testMode);
    void buildMaze(const std::vector<std::string>& maze, int length, int width, mcpp::Coordinate buildStart);
    void teleportPlayerToRandomDot(const std::vector<std::string>& maze, mcpp::Coordinate buildStart);
    void teleportPlayerToFurthestDot(const std::vector<std::string>& maze, mcpp::Coordinate buildStart);  // <-- NEW
    void undoChanges();

private:
    struct BlockChange {
        mcpp::Coordinate coord;
        mcpp::BlockType original;
    };

    void saveBlockChange(const mcpp::Coordinate& coord);

    static constexpr int MAX_CHANGES = 10000;
    BlockChange changes[MAX_CHANGES];
    int changeCount;

    int exitRow;
    mcpp::MinecraftConnection mc;
};

#endif // MAZE_H
