#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <ctime>

// Fully removed typing effect
void printTextWithDelay(const std::string& text, int) {
    std::cout << text << std::endl;
}

void printStartText(void) {
    std::cout << "\nWelcome to MineCraft MazeRunner!" << std::endl;
    std::cout << "--------------------------------" << std::endl;
}

void printMainMenu(void) {
    std::cout << "\n------------- MAIN MENU -------------" << std::endl;
    std::cout << "1) Generate Maze" << std::endl;
    std::cout << "2) Build Maze in MineCraft" << std::endl;
    std::cout << "3) Solve Maze" << std::endl;
    std::cout << "4) Show Team Information" << std::endl;
    std::cout << "5) Exit" << std::endl;
    std::cout << "\nEnter Menu item to continue: " << std::endl;
}

void printGenerateMazeMenu(void) {
    std::cout << std::endl;
    std::cout << "------------- GENERATE MAZE -------------" << std::endl;
    std::cout << "1) Read Maze from terminal" << std::endl;
    std::cout << "2) Generate Random Maze" << std::endl;
    std::cout << "3) Back" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter Menu item to continue: " << std::endl;
}

void printSolveMazeMenu(void) {
    std::cout << std::endl;
    std::cout << "------------- SOLVE MAZE -------------" << std::endl;
    std::cout << "1) Solve Manually" << std::endl;
    std::cout << "2) Show Escape Route" << std::endl;
    std::cout << "3) Back" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter Menu item to continue: " << std::endl;
}

void printTeamInfo(void) {
    std::cout << std::endl;
    std::cout << "Team members:" << std::endl;
    std::cout << "\t [1] Marko Cukanic (s4008040@student.rmit.edu.au)" << std::endl;
    std::cout << "\t [2] Vincent Dao (s4007708@student.rmit.edu.au)" << std::endl;
    std::cout << "\t [3] Thomas Saleh (s4006031@student.rmit.edu.au)" << std::endl;
    std::cout << std::endl;
}

void printExitMessage(void) {
    std::cout << std::endl;
    std::cout << "The End!" << std::endl;
    std::cout << std::endl;
}

void readBasePoint(int& xCor, int& yCor, int& zCor) {
    std::string input;
    bool validInput = false;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter the basePoint of maze in the format 'X Y Z': " << std::endl;

    while (!validInput) {
        std::getline(std::cin, input);

        std::istringstream ss(input);
        if (ss >> xCor >> yCor >> zCor && ss.eof()) {
            std::cout << "Press Enter again" << std::endl;
            validInput = true;
        } else {
            std::cout << "Invalid input. Please enter the format 'X Y Z'." << std::endl;
        }
    }
}

void readLengthAndWidth(int& length, int& width) {
    std::string input;
    bool validInput = false;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter the length and width of the maze 'L W': " << std::endl;

    while (!validInput) {
        std::getline(std::cin, input);

        std::stringstream ss(input);
        if (ss >> length && ss.get() == ' ' && ss >> width && ss.eof()) {
            if (length > 0 && width > 0) {
                validInput = true;
            } else {
                std::cout << "Both inputs must be positive integers." << std::endl;
            }
        } else {
            std::cout << "Invalid input. Please enter the format [L W] with positive integers." << std::endl;
        }
    }
}

void readMazeStructure(std::vector<std::string>& maze, int length, int width) {
    std::cout << "Enter the maze structure:" << std::endl;
    for (int i = 0; i < length; ++i) {
        std::string row;
        std::cin >> row;
        if (static_cast<int>(row.size()) != width) {
            std::cout << "Invalid input. Row should have exactly " << width << " characters." << std::endl;
            i--;
        } else {
            maze.push_back(row);
        }
    }
}

void printMazeInfo(const std::vector<std::string>& maze, int baseX, int baseY, int baseZ) {
    std::cout << "Maze read successfully" << std::endl;
    std::cout << "**Printing Maze**" << std::endl;
    std::cout << "Base Point: (" << baseX << ", " << baseY << ", " << baseZ << ")" << std::endl;
    std::cout << "Structure:" << std::endl;
    for (const std::string& row : maze) {
        std::cout << row << std::endl;
    }
    std::cout << "**End Printing Maze**" << std::endl;
}