// // RecursiveMazeGenerator.cpp
// #include <iostream>
// #include <vector>
// #include <cstdlib>
// #include <ctime>

// using namespace std;

// class RecursiveMazeGenerator {
// private:
//     vector<vector<char>> maze;
//     int width, height;

//     void initializeMaze() {
//         maze.clear();
//         for (int i = 0; i < height; ++i) {
//             vector<char> row;
//             for (int j = 0; j < width; ++j) {
//                 if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
//                     row.push_back('x'); // Outer walls
//                 else
//                     row.push_back('.'); // Empty inside
//             }
//             maze.push_back(row);
//         }
//     }

//     void divide(int x, int y, int w, int h, bool horizontal) {
//         if (w < 3 || h < 3) return;

//         if (horizontal) {
//             int wallY = y + 1;
//             while (wallY % 2 == 0) wallY++;
//             if (wallY >= y + h - 1) return;
//             int passageX = x + 1;
//             while (passageX % 2 != 0) passageX++;
//             if (passageX >= x + w - 1) passageX = x + w - 2;

//             for (int i = x; i < x + w; ++i) {
//                 maze[wallY][i] = 'x';
//             }
//             maze[wallY][passageX] = '.';

//             divide(x, y, w, wallY - y, !horizontal);
//             divide(x, wallY + 1, w, y + h - wallY - 1, !horizontal);
//         } else {
//             int wallX = x + 1;
//             while (wallX % 2 == 0) wallX++;
//             if (wallX >= x + w - 1) return;
//             int passageY = y + 1;
//             while (passageY % 2 != 0) passageY++;
//             if (passageY >= y + h - 1) passageY = y + h - 2;

//             for (int i = y; i < y + h; ++i) {
//                 maze[i][wallX] = 'x';
//             }
//             maze[passageY][wallX] = '.';

//             divide(x, y, wallX - x, h, !horizontal);
//             divide(wallX + 1, y, x + w - wallX - 1, h, !horizontal);
//         }
//     }

//     void addEntranceAndExit() {
//         for (int i = 1; i < height - 1; ++i) {
//             if (maze[i][1] == '.') {
//                 maze[i][0] = '.';
//                 break;
//             }
//         }
//         for (int i = height - 2; i > 0; --i) {
//             if (maze[i][width - 2] == '.') {
//                 maze[i][width - 1] = '.';
//                 break;
//             }
//         }
//     }

// public:
//     std::vector<std::vector<char>> getMaze() const { return maze; }
//     RecursiveMazeGenerator(int h, int w) : width(w), height(h) {
//         srand(time(0));
//         initializeMaze();
//         divide(1, 1, width - 2, height - 2, true);
//         addEntranceAndExit();
//     }

//     void printMaze() {
//         for (const auto& row : maze) {
//             for (char cell : row) cout << cell;
//             cout << endl;
//         }
//     }
// };
