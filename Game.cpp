#include <vector>
#include <time.h>
#include "Program.h"
#include "Game.h"
#include "GOLWindow.h"

//int to store the cell count at the start
int Game::startingCellCount = 0;
//int to store the grid size
int Game::GRID_SIZE = 0;
//double to store the generationCount
float Game::generationCount = 0;
//double to store the age of the current oldest cell
float Game::oldestCell = 1;
//2D Vector to store the age of each tile 0 = dead, >=1 = generational age of the cell
std::vector<std::vector<float>> Game::tiles;
//2D Vector to store the amount of neighbors each tile has
std::vector<std::vector<__int8>> Game::activeNeighbors;
//Bool to store whether the game has started or not
bool Game::hasStarted = false;
//Bool to store whether the game is playing or not
bool Game::isPlaying = false;

//Function to intialize the game
void Game::init(int cellCount, const int GRID_SIZE) {
	//Sets tiles and neighbor counts to 0
	tiles = std::vector<std::vector<float>>(GRID_SIZE, std::vector<float>(GRID_SIZE, 0));
	activeNeighbors = std::vector<std::vector<__int8>>(GRID_SIZE, std::vector<__int8>(GRID_SIZE, 0));

	//Assigns grid size and cell count to Game
	Game::GRID_SIZE = GRID_SIZE;
	Game::startingCellCount = cellCount;

	//Asks the user if they want random tile generation true = yes, false = no/other
	if (GOLWindow::ShowMessageBox(L"Game of Life", L"Do you want the cells to be randomly placed", MB_YESNO) == IDYES)
	{
		//Fills the grid to the cell count
		float percentFull = (float)cellCount / ((float)GRID_SIZE * (float)GRID_SIZE);
		for (int i = 0, k = 0; i < GRID_SIZE && k < cellCount; i += (int)(1 / percentFull)) {
			for (int j = 0; j < GRID_SIZE && k < cellCount; j += (int)(1 / percentFull), k++) {
				tiles[i][j] = 1;
			}
		}

		//Resets random
		srand(time(NULL));

		//Suffles the grid
		for (int i = 0; i < GRID_SIZE; i++) {
			for (int j = 0; j < GRID_SIZE; j++)
			{
				//Gets a random position
				int x = rand() % GRID_SIZE,
					y = rand() % GRID_SIZE;

				//Swaps the positions
				double holder = tiles[i][j];
				tiles[i][j] = tiles[x][y];
				tiles[x][y] = holder;
			}
		}
	}

	//Sets the game has started and sets the oldest cell
	Game::hasStarted = true;
	Game::oldestCell = 1;
}

//Function to step to the next generation
void Game::step() {
	//Sets neighbor counts and oldestCell to 0
	activeNeighbors = std::vector<std::vector<__int8>>(GRID_SIZE, std::vector<__int8>(GRID_SIZE, 0));
	oldestCell = 0;
	//Loops through each tile to update the neighbors
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			//Only updates the neighbors if the tile is active
			if (tiles[i][j])
			{
				UpdateNeighborCounts(i, j);
			}
		}
	}

	//Loops through each tile to check if it should be alive
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			//Triggers if the tile should be alive
			if ((tiles[i][j] > 0 && (activeNeighbors[i][j] == 2 || activeNeighbors[i][j] == 3)) || (!tiles[i][j] && activeNeighbors[i][j] == 3)) {
				//Increases the age of the tile
				tiles[i][j]++;

				//Checks if the cell is the oldest tile
				if (tiles[i][j] > oldestCell)
					oldestCell = tiles[i][j];
			}
			//Triggers if the tile should be dead
			else {
				//Sets the tile to dead
				tiles[i][j] = 0;
			}
		}
	}

	//Adds a generation
	generationCount++;
}

//Function to update neighboring tile counts
void Game::UpdateNeighborCounts(int x, int y) {
	//Left
	if (x >= 1)
		activeNeighbors[x - 1][y]++;
	//Up-Left
	if (x >= 1 && y >= 1)
		activeNeighbors[x - 1][y - 1]++;
	//Up
	if (y >= 1)
		activeNeighbors[x][y - 1]++;
	//Up-Right
	if (x < GRID_SIZE - 1 && y >= 1)
		activeNeighbors[x + 1][y - 1]++;
	//Right
	if (x < GRID_SIZE - 1)
		activeNeighbors[x + 1][y]++;
	//Down-Right
	if (x < GRID_SIZE - 1 && y < GRID_SIZE - 1)
		activeNeighbors[x + 1][y + 1]++;
	//Down
	if (y < GRID_SIZE - 1)
		activeNeighbors[x][y + 1]++;
	//Down-Left
	if (x >= 1 && y < GRID_SIZE - 1)
		activeNeighbors[x - 1][y + 1]++;
}