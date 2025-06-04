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
    std::cout << "\t [1] Dhiya Batrisya Binti Norfaiz (s4083646@student.rmit.edu.au)" << std::endl;
    std::cout << "\t [2] Sahar Zainullah (s3903957@student.rmit.edu.au)" << std::endl;
    std::cout << "\t [3] Shahira Jasmine Binti Yahaya (s411155@student.rmit.edu.au)" << std::endl;
    std::cout << std::endl;
}

void printExitMessage(void) {
    std::cout << std::endl;
    std::cout << "The End!" << std::endl;
    std::cout << std::endl;
}

