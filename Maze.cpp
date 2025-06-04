#include "Maze.h"
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
    // Optional: fade slower for bigger mazes
    int delayMs = 20;

    for (int i = changeCount - 1; i >= 0; --i) {
        mc.setBlock(changes[i].coord, changes[i].original);

        // Add staggered delay
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }

    changeCount = 0;
}


void Maze::buildMaze(const std::vector<std::string>& maze, int length, int width, mcpp::Coordinate buildStart) {
    int mazeHeight = 3;
    int border = 1;
    int y = buildStart.y - 1;

    // Step 1: Flatten terrain to baseY
    for (int z = -border; z < width + border; z++) {
        for (int x = -border; x < length + border; x++) {
            mcpp::Coordinate ground = buildStart + mcpp::Coordinate(x, 0, z);
            mcpp::Coordinate floor = mcpp::Coordinate(ground.x, y - 1, ground.z);

            // Save and set the ground base
            saveBlockChange(floor);
            mc.setBlock(floor, mcpp::Blocks::GRASS);

            // Clear 3 blocks of air above
            for (int h = 0; h < mazeHeight; ++h) {
                mcpp::Coordinate clear = mcpp::Coordinate(ground.x, y + h, ground.z);
                saveBlockChange(clear);
                mc.setBlock(clear, mcpp::Blocks::AIR);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // Step 2: Build maze walls and clear paths
    for (int z = 0; z < width; z++) {
        for (int x = 0; x < length; x++) {
            mcpp::Coordinate cell = buildStart + mcpp::Coordinate(x, 0, z);

            if (maze[z][x] == 'x') {
                // Wall: place 3 acacia blocks
                for (int h = 0; h < mazeHeight; ++h) {
                    mcpp::Coordinate wall = mcpp::Coordinate(cell.x, y + h, cell.z);
                    saveBlockChange(wall);
                    mc.setBlock(wall, mcpp::Blocks::ACACIA_WOOD_PLANK);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            } else {
                // Path: ensure air
                for (int h = 0; h < mazeHeight; ++h) {
                    mcpp::Coordinate path = mcpp::Coordinate(cell.x, y + h, cell.z);
                    saveBlockChange(path);
                    mc.setBlock(path, mcpp::Blocks::AIR);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
    }

    // Step 3: Place BLUE_CARPET at exit (first border '.' found)
    for (int z = 0; z < width; z++) {
        for (int x = 0; x < length; x++) {
            if (maze[z][x] == '.' &&
                (z == 0 || z == width - 1 || x == 0 || x == length - 1)) {
                mcpp::Coordinate exitTile = buildStart + mcpp::Coordinate(x, 0, z);
                mcpp::Coordinate carpet = mcpp::Coordinate(exitTile.x, y, exitTile.z);
                saveBlockChange(carpet);
                mc.setBlock(carpet, mcpp::Blocks::BLUE_CARPET);
                return;  // Only mark one exit
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
            if (maze[r][c] == '.') {
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

        if (maze[r][c] == '.' && dist > maxDist) {
            maxDist = dist;
            furthestRow = r;
            furthestCol = c;
        }

        for (int d = 0; d < 4; ++d) {
            int nr = r + dRow[d];
            int nc = c + dCol[d];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                maze[nr][nc] == '.' && !visited[nr][nc]) {
                visited[nr][nc] = true;
                q.push({nr, nc, dist + 1});
            }
        }
    }

    int worldX = basePoint.x + furthestCol;
    int worldY = basePoint.y;
    int worldZ = basePoint.z + furthestRow;

    std::string tpCommand = "tp @a " + std::to_string(worldX + 0.5) + " "
                                     + std::to_string(worldY) + " "
                                     + std::to_string(worldZ+ 0.5);
    mc.doCommand(tpCommand);

    std::cout << "Teleported to furthest point: (" << worldX << ", " << worldY << ", " << worldZ << ")\n";
}

void Maze::generateRandomMaze(std::vector<std::string>& maze, int length, int width) {
    for (int row = 0; row < length; row++) {
        std::string mazeRow(width, 'x');
        maze.push_back(mazeRow);
    }

    int side = std::rand() % 4;
    int entranceX = 0, entranceY = 0;

    switch (side) {
        case 0: entranceX = 1 + 2 * (std::rand() % ((width - 1) / 2)); entranceY = 0; break;
        case 1: entranceX = 1 + 2 * (std::rand() % ((width - 1) / 2)); entranceY = length - 1; break;
        case 2: entranceX = width - 1; entranceY = 1 + 2 * (std::rand() % ((length - 1) / 2)); break;
        case 3: entranceX = 0; entranceY = 1 + 2 * (std::rand() % ((length - 1) / 2)); break;
    }

    maze[entranceY][entranceX] = '.';

    int startX, startY;
    do {
        startX = 1 + 2 * (std::rand() % ((width - 1) / 2));
        startY = 1 + 2 * (std::rand() % ((length - 1) / 2));
    } while (startX % 2 == 0 || startY % 2 == 0);

    recursiveBacktrack(maze, length, width, startX, startY);
}

void Maze::recursiveBacktrack(std::vector<std::string>& maze, int length, int width, int x, int y) {
    const int dx[] = {0, 0, -2, 2};
    const int dy[] = {2, -2, 0, 0};
    const int directions = 4;

    maze[y][x] = '.';

    int dir[directions] = {0, 1, 2, 3};
    std::mt19937 gen(std::random_device{}());
    std::shuffle(dir, dir + directions, gen);

    for (int i = 0; i < directions; i++) {
        int nx = x + dx[dir[i]];
        int ny = y + dy[dir[i]];

        if (nx >= 1 && nx < width - 1 && ny >= 1 && ny < length - 1 && maze[ny][nx] == 'x') {
            int mx = (x + nx) / 2;
            int my = (y + ny) / 2;
            maze[my][mx] = '.';
            recursiveBacktrack(maze, length, width, nx, ny);
        }
    }
}
