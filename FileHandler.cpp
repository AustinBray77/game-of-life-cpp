#include "FileHandler.h"
#include <fstream>
#include <string>
#include <vector>

//Returns lines from file set in FileHandler
std::vector<std::string> FileHandler::GetLines() {
	//Opens file from filePath into ifstream file
	std::ifstream file;
	file.open(filePath);
	
	std::vector<std::string> lines;

	//Loops through each line and adds them to lines
	std::string line;
	while (std::getline(file, line)) {
		lines.push_back(line);
	}

	return lines;
}