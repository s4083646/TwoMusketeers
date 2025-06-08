#ifndef MAZE_HANDLER_H
#define MAZE_HANDLER_H

#include <vector>
#include <string>
#include <mcpp/mcpp.h>

class MazeHandler {
public:
    MazeHandler();
    ~MazeHandler();

    void createRandomLayout(std::vector<std::string>& layout, int height, int width, bool isTest);
    void renderMazeInWorld(const std::vector<std::string>& layout, int len, int wid, mcpp::Coordinate origin);
    void moveToRandomStart(const std::vector<std::string>& layout, mcpp::Coordinate origin);
    void moveToDeepestPoint(const std::vector<std::string>& layout, mcpp::Coordinate origin);
    void revertChanges();

private:
    struct ChangeRecord {
        mcpp::Coordinate location;
        mcpp::BlockType originalType;
    };

    void captureChange(const mcpp::Coordinate& location);

    static constexpr int MAX_LOG = 10000;
    ChangeRecord history[MAX_LOG];
    int changeIndex;

    int goalRow;
    mcpp::MinecraftConnection mc;
};

#endif // MAZE_HANDLER_H
