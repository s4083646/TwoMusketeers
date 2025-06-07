#include "Maze.h"
#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <algorithm>
#include <queue>
#include <tuple>

Maze::Maze() {}

Maze::Maze(mcpp::Coordinate basePoint, unsigned int xlen, unsigned int zlen, bool mode) {}

Maze::~Maze() {}

void Maze::saveBlockChange(const mcpp::Coordinate& coord) {
    if (changeCount >= 10000) return;
    changes[changeCount].coord = coord;
    changes[changeCount].original = mc.getBlock(coord);
    ++changeCount;
}

void Maze::undoChanges() {
    int delayMs = 20;
    for (int i = changeCount - 1; i >= 0; --i) {
        mc.setBlock(changes[i].coord, changes[i].original);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    changeCount = 0;
}

void Maze::buildMaze(const std::vector<std::string>& mazeRaw, int length, int width, mcpp::Coordinate buildStart) {
    int mazeHeight = 3;
    int y = buildStart.y - 1;

    // Copy maze and restore entrance marker
    std::vector<std::string> maze = mazeRaw;
    for (int z = 0; z < width; ++z) {
        for (int x = 0; x < length; ++x) {
            if (maze[z][x] == 'e') maze[z][x] = '.';  // restore entrance to path
        }
    }

    // Step 1: Flatten terrain
    for (int z = -1; z <= width; ++z) {
        for (int x = -1; x <= length; ++x) {
            mcpp::Coordinate ground = buildStart + mcpp::Coordinate(x, 0, z);
            mcpp::Coordinate floor(ground.x, y - 1, ground.z);

            saveBlockChange(floor);
            mc.setBlock(floor, mcpp::Blocks::GRASS);

            for (int h = 0; h < mazeHeight; ++h) {
                mcpp::Coordinate clear(ground.x, y + h, ground.z);
                saveBlockChange(clear);
                mc.setBlock(clear, mcpp::Blocks::AIR);
            }
        }
    }

    // Step 2: Build maze walls
    for (int z = 0; z < width; ++z) {
        for (int x = 0; x < length; ++x) {
            mcpp::Coordinate pos = buildStart + mcpp::Coordinate(x, 0, z);
            if (maze[z][x] == 'x') {
                for (int h = 0; h < mazeHeight; ++h) {
                    mcpp::Coordinate wall(pos.x, y + h, pos.z);
                    saveBlockChange(wall);
                    mc.setBlock(wall, mcpp::Blocks::ACACIA_WOOD_PLANK);
                }
            }
        }
    }

    // Step 3: Place BLUE_CARPET at the first outer '.' (but NOT 'e')
    for (int z = 0; z < width; ++z) {
        for (int x = 0; x < length; ++x) {
            if (maze[z][x] == '.' &&
                (z == 0 || z == width - 1 || x == 0 || x == length - 1)) {
                mcpp::Coordinate exit(buildStart.x + x, y, buildStart.z + z);
                saveBlockChange(exit);
                mc.setBlock(exit, mcpp::Blocks::BLUE_CARPET);
                return;
            }
        }
    }
}

void Maze::teleportPlayerToRandomDot(const std::vector<std::string>& maze, mcpp::Coordinate basePoint) {
    int rows = maze.size();
    int cols = maze[0].size();

    int startRow = -1, startCol = -1;
    for (int r = 0; r < rows && startRow == -1; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (maze[r][c] == '.' || maze[r][c] == 'e') {
                startRow = r;
                startCol = c;
                break;
            }
        }
    }

    if (startRow == -1) {
        std::cout << "No '.' found in maze." << std::endl;
        return;
    }

    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<std::tuple<int, int, int>> q;
    q.push({startRow, startCol, 0});
    visited[startRow][startCol] = true;

    int furthestRow = startRow;
    int furthestCol = startCol;
    int maxDist = 0;

    const int dRow[] = {-1, 1, 0, 0};
    const int dCol[] = {0, 0, -1, 1};

    while (!q.empty()) {
        auto [r, c, dist] = q.front(); q.pop();

        if ((maze[r][c] == '.' || maze[r][c] == 'e') && dist > maxDist) {
            maxDist = dist;
            furthestRow = r;
            furthestCol = c;
        }

        for (int d = 0; d < 4; ++d) {
            int nr = r + dRow[d];
            int nc = c + dCol[d];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                (maze[nr][nc] == '.' || maze[nr][nc] == 'e') && !visited[nr][nc]) {
                visited[nr][nc] = true;
                q.push({nr, nc, dist + 1});
            }
        }
    }

    int worldX = basePoint.x + furthestCol;
    int worldY = basePoint.y;
    int worldZ = basePoint.z + furthestRow;

    std::string tpCommand = "tp @a " + std::to_string(worldX) + " "
                                    + std::to_string(worldY) + " "
                                    + std::to_string(worldZ);
    mc.doCommand(tpCommand);

    std::cout << "Teleported to furthest point: (" << worldX << ", " << worldY << ", " << worldZ << ")\n";
}

void Maze::generateRandomMaze(std::vector<std::string>& maze, int height, int width, bool testMode) {
    if (width < 3 || height < 3 || width % 2 == 0 || height % 2 == 0) {
        std::cout << "Maze dimensions must be odd numbers and at least 3x3.\n";
        return;
    }

    // Initialize maze: fully open inside, walls around
    maze.clear();
    for (int r = 0; r < height; ++r) {
        std::string row(width, '.');
        if (r == 0 || r == height - 1) row.assign(width, 'x');
        else {
            row[0] = 'x';
            row[width - 1] = 'x';
        }
        maze.push_back(row);
    }

    // Begin recursive division inside the inner area
    divideRegion(maze, 1, height - 2, 1, width - 2, testMode, /*horizontal=*/true);

    // Add entrance on the left wall
    for (int r = 1; r < height - 1; r += 2) {
        if (maze[r][1] == '.') {
            maze[r][0] = 'e';
            break;
        }
    }

    // Add exit on the right wall (opposite side)
    for (int r = height - 2; r >= 1; r -= 2) {
        if (maze[r][width - 2] == '.') {
            maze[r][width - 1] = '.';
            break;
        }
    }
}

   void Maze::divideRegion(std::vector<std::string>& maze, int top, int bottom, int left, int right, bool testMode, bool horizontal) {
    if (bottom - top < 2 || right - left < 2) return;

    std::mt19937 rng(std::random_device{}());

    if (horizontal) {
        int wallRow = testMode
            ? top + ((bottom - top) / 2)
            : top + 2 * (rng() % ((bottom - top) / 2));
        if (wallRow % 2 == 0) wallRow++;

        for (int c = left; c <= right; ++c) {
            maze[wallRow][c] = 'x';
        }

        int passageCol = testMode
            ? left + ((right - left) / 2)
            : left + 2 * (rng() % ((right - left + 1) / 2));
        if (passageCol % 2 == 0) passageCol++;
        maze[wallRow][passageCol] = '.';

        divideRegion(maze, top, wallRow - 1, left, right, testMode, !horizontal);
        divideRegion(maze, wallRow + 1, bottom, left, right, testMode, !horizontal);
    } else {
        int wallCol = testMode
            ? left + ((right - left) / 2)
            : left + 2 * (rng() % ((right - left) / 2));
        if (wallCol % 2 == 0) wallCol++;

        for (int r = top; r <= bottom; ++r) {
            maze[r][wallCol] = 'x';
        }

        int passageRow = testMode
            ? top + ((bottom - top) / 2)
            : top + 2 * (rng() % ((bottom - top + 1) / 2));
        if (passageRow % 2 == 0) passageRow++;
        maze[passageRow][wallCol] = '.';

        divideRegion(maze, top, bottom, left, wallCol - 1, testMode, !horizontal);
        divideRegion(maze, top, bottom, wallCol + 1, right, testMode, !horizontal);
    }
}
