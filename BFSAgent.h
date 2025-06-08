#ifndef ASSIGN3_BFSAGENT_H
#define ASSIGN3_BFSAGENT_H

#include <iostream>
#include <mcpp/mcpp.h>
#include <vector>
#include <unordered_map>

// === Directional Moves ===
#define MOVE_XPLUS mcpp::Coordinate(1, 0, 0)
#define MOVE_XMINUS mcpp::Coordinate(-1, 0, 0)
#define MOVE_ZPLUS mcpp::Coordinate(0, 0, 1)
#define MOVE_ZMINUS mcpp::Coordinate(0, 0, -1)

#define MOVE_NORTH MOVE_ZMINUS
#define MOVE_SOUTH MOVE_ZPLUS
#define MOVE_EAST  MOVE_XPLUS
#define MOVE_WEST  MOVE_XMINUS

enum AgentOrientation {
    X_PLUS,
    Z_PLUS,
    X_MINUS,
    Z_MINUS
};

class Pathfinder {
public:
    Pathfinder(mcpp::Coordinate spawnPoint);
    ~Pathfinder();

    void navigateToExit();
    void setStartToPlayer();

    std::vector<mcpp::Coordinate> searchWithBFS();
    bool checkExitTile(const mcpp::Coordinate& tile);

    AgentOrientation turnLeft(AgentOrientation dir);
    AgentOrientation turnRight(AgentOrientation dir);
    AgentOrientation turnAround(AgentOrientation dir);

    mcpp::Coordinate computeNextMove(const mcpp::Coordinate& origin, AgentOrientation dir);
    AgentOrientation inferDirection(const mcpp::Coordinate& from, const mcpp::Coordinate& to);

    bool isOnAirBlock(mcpp::MinecraftConnection& mc, mcpp::Coordinate pos, AgentOrientation dir);

private:
    mcpp::MinecraftConnection mc;
    mcpp::Coordinate currentLocation;
    AgentOrientation currentOrientation;
};

#endif // ASSIGN3_BFSAGENT_H
