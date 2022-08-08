#pragma once

#define MAX_GRID 24000

//Class to store game data
class Game {
public:
	//int to store the cell count at the start
	static int startingCellCount;
	//int to store the grid size
	static int GRID_SIZE;
	//double to store the generationCount
	static float generationCount;
	//double to store the age of the current oldest cell
	static float oldestCell;
	//2D Vector to store the age of each tile 0 = dead, >=1 = generational age of the cell
	static std::vector<std::vector<float>> tiles;
	//2D Vector to store the amount of neighbors each tile has
	static std::vector<std::vector<__int8>> activeNeighbors;
	//Bool to store whether the game has started or not
	static bool hasStarted;
	//Bool to store whether the game is playing or not
	static bool isPlaying;
	//Function to intialize the game
	static void init(int cellCount, const int GRID_SIZE = 32);
	//Function to step to the next generation
	static void step();
	//Function to update neighboring tile counts
	static void UpdateNeighborCounts(int x, int y);
};