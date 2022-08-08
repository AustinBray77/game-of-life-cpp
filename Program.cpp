#include "Program.h"
#include "GOLWindow.h"
#include "GOLException.h"

//Entry function
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	//Creates the window
	GOLWindow::CreateWindowGOL(
		hInstance, L"GOLWINDOW1", L"Game of Life (Windows 32 - C++ Edition)", 
		WS_OVERLAPPEDWINDOW, nCmdShow, 0, 800, 800);

	//Try catch to catch errors
	try {
		MSG msg = { };
		//Message loop
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (GOLException ex) {
		MessageBox(GOLWindow::m_hwnd, L"Critical Error Process Terminated - Code: " + ex.exitcode, ex.data, MB_ICONEXCLAMATION);
		return ex.exitcode;
	}
	catch (...) {
		MessageBox(GOLWindow::m_hwnd, L"Unknown Critical Error Process Terminated", L"A critical error has occured in the process.\nPlease contact the developer if this issue continues.", MB_ICONEXCLAMATION);
	}

	//Returns the last hresult, >=0 = no error, <0 = error
	return GOLWindow::lastResult;
}