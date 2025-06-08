#include <iostream>
#include "mcpp/mcpp.h"
#include <deque>


bool ReadBasePoint(mcpp::Coordinate& start){
    std::string input="";
    bool success = true;

    mcpp::MinecraftConnection mc;

    std::cout << "In Minecraft, navigate to where you need the maze" << std::endl;
    std::cout << "to be built and type - done: " << std::endl;
    
    std::cin >> input;
    if(std::cin.fail()){
        success = false;
    }

    if(success){
        start = mc.getPlayerPosition() + mcpp::Coordinate(1,0,1);
    }else{
        std::cout << "Basepoint is not correct." << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
    
    return success;
   
    
}

bool readLengthWidth(unsigned int& xlength, unsigned int& zlength){

    bool success = true;

    std::cout << "Enter the length and width of maze: " << std::endl;
    std::cin >> xlength;
    std::cin >> zlength;

    if (xlength == 0 || zlength == 0){
        success = false;
        std::cout <<
            "Length and width arguments are not correct!" << std::endl;
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }

    if (xlength%2 == 0 || zlength%2 == 0){
        success = false;
        std::cout <<
            "Length and width should to be odd numbers!" << std::endl;
    }

    return success;

}

bool readMaze(std::vector< std::vector<char> >& maze, mcpp::Coordinate& basePoint){

    unsigned int xlength = 0;
    unsigned int zlength = 0;

    bool success = ReadBasePoint(basePoint);

    if(success){
        success = readLengthWidth(xlength, zlength);
    }

    if(success){

        std::cout << 
        "Enter the maze structure: " 
        << std::endl;

        unsigned int charsRead = 0;
        char readC;

        maze.resize(zlength);
        for (unsigned int z = 0; z < zlength; z++){
            maze[z].resize(xlength);
            for (unsigned int x = 0; x < xlength; x++){
                if(std::cin.good()){
                    std::cin >> readC;
                    if(readC == 'x' || readC == '.'){
                        maze[z][x] = readC;
                        ++charsRead;
                    }
                }
            }
            
        }
        if (charsRead != (xlength * zlength)) {
            success = false;
            std::string errorString = "Structure not correct - only read ";
            errorString += std::to_string(charsRead);
            errorString += " characters instead of ";
            std::cout << errorString;
            std::cout << zlength * zlength << std::endl;
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
    }

    return success;
    
}

void printMaze(std::vector< std::vector<char> >& maze){
    std::cout << "**Printing Maze Structure**" << std::endl;
    for (unsigned int z = 0; z < maze.size(); z++){
        for (unsigned int x = 0; x < maze[0].size(); x++){
            if(maze[z][x] == 'x'){
                std::cout << 'x';
            }else{
                std::cout << '.';
            }
            
        }
        std::cout << std::endl;
            
    }
    std::cout << "**End Printing Maze**" << std::endl;
    std::cout << std::endl;

}




