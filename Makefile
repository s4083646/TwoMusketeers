.default: all

all: mazeRunner

clean:
	rm -f mazeRunner *.o

mazeRunner: Maze.o BFSAgent.o mazeRunner.o MazeReadWriteUtils.o
	g++ -Wall -Werror -std=c++17 -g -O -o $@ $^ -lmcpp

%.o: %.cpp
	g++ -Wall -Werror -std=c++17 -g -O -c $^