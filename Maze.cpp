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

    for (int r = 0; r < rows; ++r) {
        maze[r][0] = 'x';
        maze[r][cols - 1] = 'x';
    }
    for (int c = 0; c < cols; ++c) {
        maze[0][c] = 'x';
        maze[rows - 1][c] = 'x';
    }

    std::vector<int> candidates;
    for (int r = 1; r < rows - 1; ++r) {
        if (maze[r][cols - 2] == '.') {
            candidates.push_back(r);
        }
    }

    if (!candidates.empty()) {
        exitRow = candidates[rng() % candidates.size()];
        maze[exitRow][cols - 1] = '.';
    }
}

void Maze::buildMaze(const std::vector<std::string>& maze, int length, int width, mcpp::Coordinate buildStart) {
    int mazeHeight = 3;
    int y = buildStart.y - 1;

    // Clear any previous maze first
    undoChanges();

    int border = 1;
    for (int row = -border; row < width + border; ++row) {
        for (int col = -border; col < length + border; ++col) {
            int x = buildStart.x + col;
            int z = buildStart.z + row;

            // 🟫 Floor block at y - 1
            mcpp::Coordinate floor(x, y - 2, z);
            saveBlockChange(floor);
            mc.setBlock(floor, mcpp::Blocks::AIR);

            // 🧹 Clear 3 blocks above (y to y+2)
            for (int h = 0; h < 64; ++h) {
                mcpp::Coordinate air(x, y + h, z);
                saveBlockChange(air);
                mc.setBlock(air, mcpp::Blocks::AIR);
            }
        }
    }

    // Build maze structure
    for (int row = 0; row < width; ++row) {
        for (int col = 0; col < length; ++col) {
            char cell = maze[row][col];
            int x = buildStart.x + col;
            int z = buildStart.z + row;

            mcpp::Coordinate floor(x, y - 1, z);
            saveBlockChange(floor);

            if (cell == 'x') {
                mc.setBlock(floor, mcpp::Blocks::ACACIA_WOOD_PLANK);
            } else {
                mc.setBlock(floor, mcpp::Blocks::GRASS);
            }

            for (int h = 0; h < mazeHeight; ++h) {
                mcpp::Coordinate pos(x, y + h, z);
                saveBlockChange(pos);

                if (cell == 'x') {
                    mc.setBlock(pos, mcpp::Blocks::ACACIA_WOOD_PLANK);
                } else {
                    mc.setBlock(pos, mcpp::Blocks::AIR);
                }

                // 🎯 Blue carpet at exit
                if (cell == '.' && col == length - 1 && row == exitRow && h == 0) {
                    mcpp::Coordinate carpet(x + 1, y, z);
                    saveBlockChange(carpet);
                    mc.setBlock(carpet, mcpp::Blocks::BLUE_CARPET);
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

void Maze::teleportPlayerToFurthestDot(const std::vector<std::string>& maze, mcpp::Coordinate buildStart) {
    int width = maze.size();
    int length = maze[0].size();

    // Find the exit cell (should be last column at row = exitRow)
    int startZ = exitRow;
    int startX = length - 1;

    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<bool>> visited(width, std::vector<bool>(length, false));
    q.push({startZ, startX});
    visited[startZ][startX] = true;

    int furthestX = startX;
    int furthestZ = startZ;

    while (!q.empty()) {
        auto [z, x] = q.front();
        q.pop();

        furthestX = x;
        furthestZ = z;

        static const int dz[4] = {0, 0, 1, -1};
        static const int dx[4] = {1, -1, 0, 0};

        for (int d = 0; d < 4; ++d) {
            int nz = z + dz[d];
            int nx = x + dx[d];
            if (nz >= 0 && nz < width && nx >= 0 && nx < length &&
                !visited[nz][nx] && maze[nz][nx] == '.') {
                visited[nz][nx] = true;
                q.push({nz, nx});
            }
        }
    }

    mcpp::Coordinate furthestCoord = mcpp::Coordinate(
        buildStart.x + furthestX,
        buildStart.y + 1,
        buildStart.z + furthestZ
    );

    std::string tpCmd = "tp @a " + std::to_string(furthestCoord.x) + " " +
                        std::to_string(furthestCoord.y) + " " +
                        std::to_string(furthestCoord.z);
    mc.doCommand(tpCmd);
}
