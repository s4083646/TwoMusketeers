#include <iostream>
#include <mcpp/mcpp.h>
#include "menuUtils.h"
#include "Maze.h"
#include "Agent.h"
#include "MazeReadWriteUtils.h"

#define NORMAL_MODE 0
#define TESTING_MODE 1

enum States {
    ST_Main,
    ST_GetMaze,
    ST_SolveMaze,
    ST_Creators,
    ST_Exit
};

int main(void) {
    mcpp::MinecraftConnection mc;
    mc.doCommand("time set day");

    States curState = ST_Main;

    int userChoice = 0;
    int mazeLength, mazeWidth;
    std::vector<std::string> maze;
    mcpp::Coordinate buildStart;
    Maze builtMaze;  // ← Needed to track changes for undo()

    while (curState != ST_Exit) {
        printStartText();
        printMainMenu();
        std::cin >> userChoice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        if (userChoice == 1) {
            bool noError = true;

            while (noError) {
                printGenerateMazeMenu();
                std::cin >> userChoice;

                if (userChoice == 1) {
                    std::vector<std::vector<char>> mazeVec;
                    mcpp::Coordinate base;
                    if (readMaze(mazeVec, base)) {
                        std::cout << "Maze read successfully" << std::endl;
                        printMaze(mazeVec);

                        maze.clear();
                        for (const auto& row : mazeVec) {
                            maze.emplace_back(row.begin(), row.end());
                        }

                        mazeLength = mazeVec[0].size();
                        mazeWidth = mazeVec.size();
                        buildStart = base;
                        noError = false;
                    } else {
                        std::cout << "Error Reading Maze. Try again." << std::endl;
                    }

                } else if (userChoice == 2) {
                    mcpp::Coordinate playerPos = mc.getPlayerPosition();
                    mcpp::Coordinate front = playerPos + mcpp::Coordinate(1, 0, 0);
                    buildStart = front;

                    if (!readLengthWidth((unsigned&)mazeLength, (unsigned&)mazeWidth)) continue;

                    builtMaze.generateRandomMaze(maze, mazeWidth, mazeLength, TESTING_MODE);
                    std::cout << "Maze generated successfully" << std::endl;

                    std::vector<std::vector<char>> mazeVecForPrint;
                    for (const std::string& row : maze) {
                        mazeVecForPrint.emplace_back(row.begin(), row.end());
                    }
                    printMaze(mazeVecForPrint);
                    noError = false;

                } else if (userChoice == 3) {
                    noError = false;
                } else {
                    std::cout << "Error. Please enter a number from 1 to 3" << std::endl;
                    std::cin.clear();
                    std::cin.ignore(1000, '\n');
                }
            }
        }

        else if (userChoice == 2) {
            if (maze.empty()) {
                std::cout << "Maze has not been created. Please generate a maze before attempting to build one." << std::endl;
                continue;
            }

            int mazeX = buildStart.x;
            int mazeY = buildStart.y;
            int mazeZ = buildStart.z;

            int x1 = mazeX;
            int z1 = mazeZ;
            int x2 = mazeX + mazeLength - 1;
            int z2 = mazeZ + mazeWidth - 1;

            std::string tpCommand = "tp @a " + std::to_string(mazeX) + " " + std::to_string(mazeY + 1) + " " + std::to_string(mazeZ);
            mc.doCommand(tpCommand);

            std::string clearMazeArea = "fill "
                + std::to_string(x1) + " " + std::to_string(mazeY) + " " + std::to_string(z1) + " "
                + std::to_string(x2) + " " + std::to_string(mazeY + 2) + " " + std::to_string(z2)
                + " minecraft:air";
            mc.doCommand(clearMazeArea);

            builtMaze.buildMaze(maze, mazeLength, mazeWidth, buildStart);
        }

        else if (userChoice == 3) {
            if (maze.empty()) {
                std::cout << "Maze has not been created. Please generate a maze before attempting to solve one." << std::endl;
                continue;
            }

            bool backCheck = true;
            while (backCheck) {
                printSolveMazeMenu();
                std::cin >> userChoice;

                if (userChoice == 1) {
                    builtMaze.teleportPlayerToRandomDot(maze, buildStart);
                } else if (userChoice == 2) {
                    Agent solveMaze(buildStart);
                    solveMaze.initializePlayerBlock();
                    solveMaze.guideToExit();
                } else if (userChoice == 3) {
                    backCheck = false;
                } else {
                    std::cout << "Invalid choice. Please enter a number from 1 to 3." << std::endl;
                }
            }
        }

        else if (userChoice == 4) {
            printTeamInfo();
        }

        else if (userChoice == 5) {
            printExitMessage();  // fixed name
            builtMaze.undoChanges();  // properly tracked undo
            curState = ST_Exit;
        }

        else {
            std::cout << "Input Error: Enter a number between 1 and 5 ...." << std::endl;
        }
    }

    return 0;
}
