#pragma once

#include "Program.h"
#include <string>
#include <vector>

//Class to handle file management
class FileHandler {
public:
	//String to store the file path
	std::wstring filePath = L"";

	//Basic constructor
	FileHandler(std::wstring filePath) {
		this->filePath = filePath;
	}

	//Returns lines from file set in FileHandler
	std::vector<std::string> GetLines();
public:
	//Function to get the current directory of the exe file
	static std::wstring FHGetCurrentDirectory() {
		//Gets path from windows
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);

		//Cuts out the file name from the path and returns it
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		return std::wstring(buffer).substr(0, pos);
	}
};