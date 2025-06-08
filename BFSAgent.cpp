#include "BFSAgent.h"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

// Updated hash specialization for Coordinate
namespace std {
    template <>
    struct hash<mcpp::Coordinate> {
        std::size_t operator()(const mcpp::Coordinate& p) const noexcept {
            return hash<int>()(p.x) ^ hash<int>()(p.y << 1) ^ hash<int>()(p.z << 2);
        }
    };
}

Pathfinder::Pathfinder(mcpp::Coordinate startingPoint)
    : mc(), currentLocation(startingPoint), currentOrientation(Z_MINUS) {}

Pathfinder::~Pathfinder() {}

void Pathfinder::setStartToPlayer() {
    currentLocation = mc.getPlayerPosition();
    currentOrientation = Z_MINUS;
}

void Pathfinder::navigateToExit() {
    currentLocation = mc.getPlayerPosition();
    currentOrientation = Z_MINUS;

    auto path = searchWithBFS();

    if (path.empty()) {
        std::cout << "No available escape path found." << std::endl;
        return;
    }

    mcpp::Coordinate lastPosition = currentLocation;
    int yCoord = currentLocation.y;

    for (size_t i = 0; i < path.size(); ++i) {
        mcpp::Coordinate step = path[i];

        int below = step.y - 1;
        while (mc.getBlock({step.x, below, step.z}) == mcpp::Blocks::AIR && below > 1) {
            below--;
        }
        yCoord = below + 1;

        mcpp::Coordinate place(step.x, yCoord, step.z);

        if (i != 0) {
            mc.setBlock(lastPosition, mcpp::Blocks::AIR);
        }

        mc.setBlock(place, mcpp::Blocks::LIME_CARPET);

        std::cout << "Walk Step " << i + 1 << ": (" << place.x << ", " << place.y << ", " << place.z << ")\n";

        lastPosition = place;
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }

    std::cout << "Path completed." << std::endl;
}

std::vector<mcpp::Coordinate> Pathfinder::searchWithBFS() {
    std::queue<mcpp::Coordinate> open;
    std::unordered_map<mcpp::Coordinate, mcpp::Coordinate> pathMap;
    std::unordered_set<mcpp::Coordinate> visited;

    open.push(currentLocation);
    visited.insert(currentLocation);

    mcpp::Coordinate found;
    bool done = false;

    while (!open.empty()) {
        mcpp::Coordinate node = open.front();
        open.pop();

        std::vector<mcpp::Coordinate> nextMoves = {
            node + MOVE_NORTH,
            node + MOVE_SOUTH,
            node + MOVE_EAST,
            node + MOVE_WEST
        };

        for (const auto& neighbor : nextMoves) {
            if (visited.count(neighbor)) continue;
            auto tile = mc.getBlock(neighbor);
            if (tile != mcpp::Blocks::AIR && tile != mcpp::Blocks::BLUE_CARPET) continue;

            pathMap[neighbor] = node;
            visited.insert(neighbor);
            open.push(neighbor);

            if (checkExitTile(neighbor)) {
                found = neighbor;
                done = true;
                break;
            }
        }
        if (done) break;
    }

    std::vector<mcpp::Coordinate> trace;
    if (!done) return trace;

    for (auto step = found; step != currentLocation; step = pathMap[step]) {
        trace.push_back(step);
    }
    std::reverse(trace.begin(), trace.end());
    return trace;
}

bool Pathfinder::checkExitTile(const mcpp::Coordinate& pos) {
    return mc.getBlock(pos) == mcpp::Blocks::BLUE_CARPET;
}

AgentOrientation Pathfinder::turnLeft(AgentOrientation orient) {
    switch (orient) {
        case X_PLUS: return Z_MINUS;
        case Z_PLUS: return X_PLUS;
        case X_MINUS: return Z_PLUS;
        case Z_MINUS: return X_MINUS;
    }
    return orient;
}

AgentOrientation Pathfinder::turnRight(AgentOrientation orient) {
    switch (orient) {
        case X_PLUS: return Z_PLUS;
        case Z_PLUS: return X_MINUS;
        case X_MINUS: return Z_MINUS;
        case Z_MINUS: return X_PLUS;
    }
    return orient;
}

AgentOrientation Pathfinder::turnAround(AgentOrientation orient) {
    switch (orient) {
        case X_PLUS: return X_MINUS;
        case Z_PLUS: return Z_MINUS;
        case X_MINUS: return X_PLUS;
        case Z_MINUS: return Z_PLUS;
    }
    return orient;
}

AgentOrientation Pathfinder::inferDirection(const mcpp::Coordinate& start, const mcpp::Coordinate& end) {
    int diffX = end.x - start.x;
    int diffZ = end.z - start.z;

    if (diffX == 1) return X_PLUS;
    if (diffX == -1) return X_MINUS;
    if (diffZ == 1) return Z_PLUS;
    if (diffZ == -1) return Z_MINUS;
    return currentOrientation;
}

mcpp::Coordinate Pathfinder::computeNextMove(const mcpp::Coordinate& from, AgentOrientation orient) {
    switch (orient) {
        case X_PLUS:  return from + MOVE_EAST;
        case Z_PLUS:  return from + MOVE_SOUTH;
        case X_MINUS: return from + MOVE_WEST;
        case Z_MINUS: return from + MOVE_NORTH;
    }
    return from;
}

bool Pathfinder::isOnAirBlock(mcpp::MinecraftConnection& mc, mcpp::Coordinate location, AgentOrientation /*unused*/) {
    return mc.getBlock(location) == mcpp::Blocks::AIR;
}
