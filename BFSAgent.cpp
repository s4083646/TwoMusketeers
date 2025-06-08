#include "BFSAgent.h"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

// Hash support for mcpp::Coordinate
namespace std {
    template <>
    struct hash<mcpp::Coordinate> {
        std::size_t operator()(const mcpp::Coordinate& coord) const noexcept {
            return std::hash<int>()(coord.x) ^ std::hash<int>()(coord.y << 1) ^ std::hash<int>()(coord.z << 2);
        }
    };
}

Agent::Agent(mcpp::Coordinate startLoc)
    : mc(), currentLocation(startLoc), currentOrientation(Z_MINUS) {}

Agent::~Agent() {}

void Agent::initializePlayerBlock() {
    currentLocation = mc.getPlayerPosition();
    currentOrientation = Z_MINUS;
}

void Agent::guideToExit() {
    currentLocation = mc.getPlayerPosition();
    currentOrientation = Z_MINUS;

    std::vector<mcpp::Coordinate> path = solveWithBFS();

    if (path.empty()) {
        std::cout << "Sorry, no path, you are trapped!" << std::endl;
        return;
    }

    mcpp::Coordinate previous = currentLocation;
    int groundY = currentLocation.y;

    for (size_t step = 0; step < path.size(); ++step) {
        mcpp::Coordinate current = path[step];

        // Try to locate ground just below
        int yBelow = current.y - 1;
        while (mc.getBlock(mcpp::Coordinate(current.x, yBelow, current.z)) == mcpp::Blocks::AIR && yBelow > 1) {
            yBelow--;
        }
        groundY = yBelow + 1;

        mcpp::Coordinate carpetPos(current.x, groundY, current.z);

        // Remove previous carpet
        if (step != 0) {
            mc.setBlock(previous, mcpp::Blocks::AIR);
        }

        // Place current carpet
        mc.setBlock(carpetPos, mcpp::Blocks::LIME_CARPET);

        std::cout << "Step[" << step + 1 << "]: (" << carpetPos.x << ", " << carpetPos.y << ", " << carpetPos.z << ")\n";

        previous = carpetPos;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Escape route visualized.\n";
}

std::vector<mcpp::Coordinate> Agent::solveWithBFS() {
    std::queue<mcpp::Coordinate> q;
    std::unordered_map<mcpp::Coordinate, mcpp::Coordinate> cameFrom;
    std::unordered_set<mcpp::Coordinate> visited;

    q.push(currentLocation);
    visited.insert(currentLocation);

    mcpp::Coordinate end;
    bool found = false;

    while (!q.empty()) {
        mcpp::Coordinate curr = q.front();
        q.pop();

        std::vector<mcpp::Coordinate> neighbors = {
            curr + MOVE_NORTH,
            curr + MOVE_SOUTH,
            curr + MOVE_EAST,
            curr + MOVE_WEST
        };

        for (const auto& next : neighbors) {
            if (visited.count(next)) continue;
            if (mc.getBlock(next) != mcpp::Blocks::AIR && mc.getBlock(next) != mcpp::Blocks::BLUE_CARPET) continue;

            cameFrom[next] = curr;
            visited.insert(next);
            q.push(next);

            if (isExit(next)) {
                end = next;
                found = true;
                break;
            }
        }

        if (found) break;
    }

    std::vector<mcpp::Coordinate> path;
    if (!found) return path;

    for (mcpp::Coordinate at = end; at != currentLocation; at = cameFrom[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

bool Agent::isExit(const mcpp::Coordinate& loc) {
    // FIXED: detect if it's a blue carpet block
    return mc.getBlock(loc) == mcpp::Blocks::BLUE_CARPET;
}

AgentOrientation Agent::turnLeft(AgentOrientation orientation) {
    switch (orientation) {
        case X_PLUS: return Z_MINUS;
        case Z_PLUS: return X_PLUS;
        case X_MINUS: return Z_PLUS;
        case Z_MINUS: return X_MINUS;
    }
    return orientation;
}

AgentOrientation Agent::turnRight(AgentOrientation orientation) {
    switch (orientation) {
        case X_PLUS: return Z_PLUS;
        case Z_PLUS: return X_MINUS;
        case X_MINUS: return Z_MINUS;
        case Z_MINUS: return X_PLUS;
    }
    return orientation;
}

AgentOrientation Agent::turnBack(AgentOrientation orientation) {
    switch (orientation) {
        case X_PLUS: return X_MINUS;
        case Z_PLUS: return Z_MINUS;
        case X_MINUS: return X_PLUS;
        case Z_MINUS: return Z_PLUS;
    }
    return orientation;
}

AgentOrientation Agent::getNewOrientation(const mcpp::Coordinate& currentLocation, const mcpp::Coordinate& nextLocation) {
    int dx = nextLocation.x - currentLocation.x;
    int dz = nextLocation.z - currentLocation.z;

    if (dx == 1) return X_PLUS;
    if (dx == -1) return X_MINUS;
    if (dz == 1) return Z_PLUS;
    if (dz == -1) return Z_MINUS;
    return currentOrientation;
}

mcpp::Coordinate Agent::getNextLocation(const mcpp::Coordinate& currentLocation, AgentOrientation orientation) {
    switch (orientation) {
        case X_PLUS:  return currentLocation + MOVE_EAST;
        case Z_PLUS:  return currentLocation + MOVE_SOUTH;
        case X_MINUS: return currentLocation + MOVE_WEST;
        case Z_MINUS: return currentLocation + MOVE_NORTH;
    }
    return currentLocation;
}

bool Agent::CheckIfExit(mcpp::MinecraftConnection& mc, mcpp::Coordinate currentLocation, AgentOrientation currentOrientation) {
    return mc.getBlock(currentLocation) == mcpp::Blocks::AIR;
}
