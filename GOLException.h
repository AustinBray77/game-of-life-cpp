#pragma once

//Class for custom exception
class GOLException {
public:
	//Stores the information of the exception
	const wchar_t* data;
	//Stores the code to exit with
	int exitcode;

	//Basic Constructor
	GOLException(const wchar_t* data, int exitcode) {
		this->data = data;
		this->exitcode = exitcode;
	}
};