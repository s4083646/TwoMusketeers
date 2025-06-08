#ifndef ASSIGN3_AGENT_H
#define ASSIGN3_AGENT_H

#include <iostream>
#include <mcpp/mcpp.h>
#include <vector>
#include <unordered_map>

// --- Movement Macros ---
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

class Agent {
public:
    Agent(mcpp::Coordinate startLoc);
    ~Agent();

    void guideToExit();
    void initializePlayerBlock();

    std::vector<mcpp::Coordinate> solveWithBFS();
    bool isExit(const mcpp::Coordinate& loc);

    AgentOrientation turnLeft(AgentOrientation orientation);
    AgentOrientation turnRight(AgentOrientation orientation);
    AgentOrientation turnBack(AgentOrientation orientation);

    mcpp::Coordinate getNextLocation(const mcpp::Coordinate& currentLocation, AgentOrientation orientation);
    AgentOrientation getNewOrientation(const mcpp::Coordinate& currentLocation, const mcpp::Coordinate& nextLocation);

    bool CheckIfExit(mcpp::MinecraftConnection& mc, mcpp::Coordinate currentLocation, AgentOrientation currentOrientation);

private:
    mcpp::MinecraftConnection mc;
    mcpp::Coordinate currentLocation;
    AgentOrientation currentOrientation;
};

#endif // ASSIGN3_AGENT_H
