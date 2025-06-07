#include "Maze.h"
#include <random>
#include <ctime>
#include <queue>
#include <tuple>
#include <iostream>
#include <thread>
#include <chrono>

Maze::Maze() : changeCount(0), exitRow(-1) {}

Maze::~Maze() {}

void Maze::saveBlockChange(const mcpp::Coordinate& coord) {
    if (changeCount >= 10000) return;
    changes[changeCount].coord = coord;
    changes[changeCount].original = mc.getBlock(coord);
    ++changeCount;
}

void Maze::undoChanges() {
    for (int i = changeCount - 1; i >= 0; --i) {
        mc.setBlock(changes[i].coord, changes[i].original);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    changeCount = 0;
}

void Maze::generateRandomMaze(std::vector<std::string>& maze, int rows, int cols, bool testMode) {
    maze.assign(rows, std::string(cols, 'x'));

    std::mt19937 rng(testMode ? 0 : std::random_device{}());
    auto shuffle = [&](auto& vec) {
        std::shuffle(vec.begin(), vec.end(), rng);
    };

    auto carve = [&](int r, int c, auto&& carve_ref) -> void {
        static const int dr[] = { -2, 2, 0, 0 };
        static const int dc[] = { 0, 0, -2, 2 };
        maze[r][c] = '.';
        std::vector<int> dirs = { 0, 1, 2, 3 };
        shuffle(dirs);

        for (int d : dirs) {
            int nr = r + dr[d], nc = c + dc[d];
            if (nr > 0 && nr < rows - 1 && nc > 0 && nc < cols - 1 && maze[nr][nc] == 'x') {
                maze[r + dr[d] / 2][c + dc[d] / 2] = '.';
                carve_ref(nr, nc, carve_ref);
            }
        }
    };

    carve(1, 1, carve);

    // Ensure outer border is closed
    for (int r = 0; r < rows; ++r) {
        maze[r][0] = 'x';
        maze[r][cols - 1] = 'x';
    }
    for (int c = 0; c < cols; ++c) {
        maze[0][c] = 'x';
        maze[rows - 1][c] = 'x';
    }

    // Add one walkable exit (no 'e', just a . with carpet)
    std::vector<int> candidates;
    for (int r = 1; r < rows - 1; ++r) {
        if (maze[r][cols - 2] == '.') {
            candidates.push_back(r);
        }
    }

    if (!candidates.empty()) {
        exitRow = candidates[rng() % candidates.size()];
        maze[exitRow][cols - 1] = '.'; // Normal dot, will add blue carpet later
    }
}

void Maze::buildMaze(const std::vector<std::string>& maze, int length, int width, mcpp::Coordinate buildStart) {
    int mazeHeight = 3;
    int y = buildStart.y;

    for (int row = 0; row < width; ++row) {
        for (int col = 0; col < length; ++col) {
            mcpp::Coordinate ground = buildStart + mcpp::Coordinate(col, 0, row);
            mcpp::Coordinate floor = mcpp::Coordinate(ground.x, y - 1, ground.z);

            saveBlockChange(floor);
            char cell = maze[row][col];

            if (cell == 'x') {
                mc.setBlock(floor, mcpp::Blocks::ACACIA_WOOD_PLANK);  // Wall floor
            } else {
                mc.setBlock(floor, mcpp::Blocks::GRASS);        // Path floor
            }

            for (int h = 0; h < mazeHeight; ++h) {
                mcpp::Coordinate pos = mcpp::Coordinate(ground.x, y + h, ground.z);
                saveBlockChange(pos);

                if (cell == 'x') {
                    mc.setBlock(pos, mcpp::Blocks::ACACIA_WOOD_PLANK);
                } else {
                    mc.setBlock(pos, mcpp::Blocks::AIR);
                }

                // Place blue carpet at the exit tile (only once, base level)
                if (cell == '.' && col == length - 1 && row == exitRow && h == 0) {
                    mc.setBlock(ground, mcpp::Blocks::BLUE_CARPET);
                }
            }
        }
    }
}

void Maze::teleportPlayerToRandomDot(const std::vector<std::string>& maze, mcpp::Coordinate buildStart) {
    std::vector<mcpp::Coordinate> walkableTiles;
    int width = maze.size();
    int length = maze[0].length();

    for (int z = 0; z < width; ++z) {
        for (int x = 0; x < length; ++x) {
            if (maze[z][x] == '.') {
                walkableTiles.emplace_back(buildStart.x + x, buildStart.y + 1, buildStart.z + z);
            }
        }
    }

    if (walkableTiles.empty()) {
        std::cout << "No walkable tiles found to teleport to." << std::endl;
        return;
    }

    std::mt19937 rng(std::random_device{}());
    mcpp::Coordinate chosen = walkableTiles[rng() % walkableTiles.size()];

    mc.doCommand("tp @a " + std::to_string(chosen.x) + " " +
                             std::to_string(chosen.y) + " " +
                             std::to_string(chosen.z));
}
