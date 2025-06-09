#include "Maze.h"
#include <random>
#include <ctime>
#include <queue>
#include <tuple>
#include <iostream>
#include <thread>
#include <chrono>

MazeHandler::MazeHandler() : changeIndex(0), goalRow(-1) {}
MazeHandler::~MazeHandler() {}

void MazeHandler::captureChange(const mcpp::Coordinate& location) {
    if (changeIndex >= MAX_LOG) return;
    history[changeIndex].location = location;
    history[changeIndex].originalType = mc.getBlock(location);
    ++changeIndex;
}

void MazeHandler::revertChanges() {
    for (int i = changeIndex - 1; i >= 0; --i) {
        mc.setBlock(history[i].location, history[i].originalType);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    changeIndex = 0;
}

void MazeHandler::createRandomLayout(std::vector<std::string>& layout, int height, int width, bool isTest) {
    layout.assign(height, std::string(width, 'x'));

    std::mt19937 engine(isTest ? 0 : std::random_device{}());
    auto shuffle = [&](auto& vec) {
        std::shuffle(vec.begin(), vec.end(), engine);
    };

    auto dig = [&](int r, int c, auto&& digSelf) -> void {
        static const int dr[] = { -2, 2, 0, 0 };
        static const int dc[] = { 0, 0, -2, 2 };
        layout[r][c] = '.';
        std::vector<int> dirs = { 0, 1, 2, 3 };
        shuffle(dirs);

        for (int d : dirs) {
            int nr = r + dr[d], nc = c + dc[d];
            if (nr > 0 && nr < height - 1 && nc > 0 && nc < width - 1 && layout[nr][nc] == 'x') {
                layout[r + dr[d] / 2][c + dc[d] / 2] = '.';
                digSelf(nr, nc, digSelf);
            }
        }
    };

    dig(1, 1, dig);

    for (int r = 0; r < height; ++r) {
        layout[r][0] = 'x';
        layout[r][width - 1] = 'x';
    }
    for (int c = 0; c < width; ++c) {
        layout[0][c] = 'x';
        layout[height - 1][c] = 'x';
    }

    std::vector<int> options;
    for (int r = 1; r < height - 1; ++r) {
        if (layout[r][width - 2] == '.') {
            options.push_back(r);
        }
    }

    if (!options.empty()) {
        goalRow = options[engine() % options.size()];
        layout[goalRow][width - 1] = '.';
    } else {
        goalRow = -1;
    }
}

void MazeHandler::renderMazeInWorld(const std::vector<std::string>& layout, int len, int wid, mcpp::Coordinate origin) {
    int mazeHeight = 3;
    int y = origin.y - 1;

    revertChanges();

    int margin = 1;
    for (int zOff = -margin; zOff < wid + margin; ++zOff) {
        for (int xOff = -margin; xOff < len + margin; ++xOff) {
            int x = origin.x + xOff;
            int z = origin.z + zOff;

            mcpp::Coordinate below(x, y - 2, z);
            captureChange(below);
            mc.setBlock(below, mcpp::Blocks::AIR);

            for (int h = 0; h < 4; ++h) {
                mcpp::Coordinate above(x, y + h, z);
                captureChange(above);
                mc.setBlock(above, mcpp::Blocks::AIR);
            }
        }
    }

    for (int row = 0; row < wid; ++row) {
        for (int col = 0; col < len; ++col) {
            char type = layout[row][col];
            int x = origin.x + col;
            int z = origin.z + row;

            mcpp::Coordinate ground(x, y - 1, z);
            captureChange(ground);

            if (type == 'x') {
                mc.setBlock(ground, mcpp::Blocks::ACACIA_WOOD_PLANK);
            } else {
                mc.setBlock(ground, mcpp::Blocks::GRASS);
            }

            for (int h = 0; h < mazeHeight; ++h) {
                mcpp::Coordinate pos(x, y + h, z);
                captureChange(pos);

                if (type == 'x') {
                    mc.setBlock(pos, mcpp::Blocks::ACACIA_WOOD_PLANK);
                } else {
                    mc.setBlock(pos, mcpp::Blocks::AIR);
                }
            }
        }
    }

    // ✅ UNIVERSAL EXIT DETECTION & CARPET PLACEMENT
    bool blueCarpetPlaced = false;

    for (int row = 0; row < wid; ++row) {
        for (int col = 0; col < len; ++col) {
            char type = layout[row][col];

            // Only check border tiles
            if (!(row == 0 || row == wid - 1 || col == 0 || col == len - 1)) continue;

            if (type == '.' && !blueCarpetPlaced) {
                int x = origin.x + col;
                int z = origin.z + row;

                mcpp::Coordinate carpet;

                if (col == 0)         carpet = mcpp::Coordinate(x - 1, y, z); // left edge
                else if (col == len - 1) carpet = mcpp::Coordinate(x + 1, y, z); // right edge
                else if (row == 0)       carpet = mcpp::Coordinate(x, y, z - 1); // top edge
                else if (row == wid - 1) carpet = mcpp::Coordinate(x, y, z + 1); // bottom edge

                captureChange(carpet);
                mc.setBlock(carpet, mcpp::Blocks::BLUE_CARPET);

                std::cout << "[✔] Blue carpet placed at: (" << carpet.x << ", " << carpet.y << ", " << carpet.z << ")" << std::endl;
                blueCarpetPlaced = true;
            }
        }
    }

    if (!blueCarpetPlaced) {
        std::cout << "[❌] Warning: No valid exit found to place blue carpet!" << std::endl;
    }
}



void MazeHandler::moveToRandomStart(const std::vector<std::string>& layout, mcpp::Coordinate origin) {
    std::vector<mcpp::Coordinate> candidates;
    int h = layout.size();
    int w = layout[0].length();

    for (int z = 0; z < h; ++z) {
        for (int x = 0; x < w; ++x) {
            if (layout[z][x] == '.') {
                candidates.emplace_back(origin.x + x, origin.y + 1, origin.z + z);
            }
        }
    }

    if (candidates.empty()) {
        std::cout << "No valid tiles to teleport." << std::endl;
        return;
    }

    std::mt19937 engine(std::random_device{}());
    mcpp::Coordinate pick = candidates[engine() % candidates.size()];

    mc.doCommand("tp @a " + std::to_string(pick.x) + " " + std::to_string(pick.y) + " " + std::to_string(pick.z));
}

void MazeHandler::moveToDeepestPoint(const std::vector<std::string>& layout, mcpp::Coordinate origin) {
    int h = layout.size();
    int w = layout[0].size();

    // Step 1: find an outer-border '.' tile (the exit)
    std::queue<std::pair<int, int>> bfs;
    std::vector<std::vector<int>> dist(h, std::vector<int>(w, -1));

    for (int z = 0; z < h; ++z) {
        for (int x = 0; x < w; ++x) {
            bool onEdge = (z == 0 || z == h - 1 || x == 0 || x == w - 1);
            if (onEdge && layout[z][x] == '.') {
                bfs.push({z, x});
                dist[z][x] = 0;
            }
        }
    }

    if (bfs.empty()) {
        std::cout << "[❌] No outer exit found in maze for deepest point search.\n";
        return;
    }

    // Step 2: BFS to find furthest reachable point
    std::pair<int, int> deepest = bfs.front();
    static const int dz[4] = {0, 0, 1, -1};
    static const int dx[4] = {1, -1, 0, 0};

    while (!bfs.empty()) {
        auto [z, x] = bfs.front(); bfs.pop();
        deepest = {z, x};

        for (int d = 0; d < 4; ++d) {
            int nz = z + dz[d];
            int nx = x + dx[d];

            if (nz >= 0 && nz < h && nx >= 0 && nx < w && layout[nz][nx] == '.' && dist[nz][nx] == -1) {
                dist[nz][nx] = dist[z][x] + 1;
                bfs.push({nz, nx});
            }
        }
    }

    int targetX = origin.x + deepest.second;
    int targetZ = origin.z + deepest.first;
    int targetY = origin.y + 1;

    mc.doCommand("tp @a " + std::to_string(targetX) + " " + std::to_string(targetY) + " " + std::to_string(targetZ));
}


void MazeHandler::setGoalRowFromMaze(const std::vector<std::string>& layout) {
    int h = layout.size();
    if (h == 0) {
        goalRow = -1;
        return;
    }
    int w = layout[0].size();
    goalRow = -1;

    for (int z = 1; z < h - 1; ++z) {
        if (layout[z][w - 1] == '.' || layout[z][w - 2] == '.') {
            goalRow = z;
            return;
        }
    }
}
