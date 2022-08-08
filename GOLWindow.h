#pragma once

#include "Program.h"

//Function to release pointers
template <class T> void SafeRelease(T** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = NULL;
	}
}

//Class to store window drawing components
class GOLWindow {
private:
	//Stores render factory
	static ID2D1Factory* pFactory;
	//Stores render target
	static ID2D1HwndRenderTarget* pRenderTarget;
	//Stores colored brush
	static ID2D1SolidColorBrush *lifeBrush;
	//static std::vector<std::vector<D2D1_RECT_F>> rectangles;

	//Stores whether the user is drawing tiles or not
	static bool isDrawing; 
	//Stores whether the user is erasing tiles or not
	static bool isErasing; 
	//Stores whether the user paused the game or not
	static bool isPaused;
	//Stores the update speed for the game (ms)
	static int UPDATESPEED; 
	//Stores the zoom factor of the camera
	static int zoomfactor;
	//Stores the camera's position for rendering
	static int position[2];

	//Function to create the graphics resources
	static HRESULT CreateGraphicsResources();
	//Function to discard the graphics resources
	static void DiscardGraphicsResources();
	//static void CalculateLayout();
	//Function to paint the window
	static void OnPaint();
	//Function to resize the window
	static void Resize();
public:
	//Stores the hwnd
	static HWND m_hwnd;
	//Stores the last hresult for error checking
	static HRESULT lastResult;
	//Function to interpret windows messages
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//Function to create the window
    static HRESULT CreateWindowGOL(
		HINSTANCE hInstance, 
		LPCWSTR CLASS_NAME, 
		LPCWSTR TITLE, 
		DWORD dw_Style, 
		INT nCmdShow, 
		DWORD dwExStyle, 
		int width = CW_USEDEFAULT, 
		int height = CW_USEDEFAULT, 
		HWND parent = 0, 
		HMENU menu = 0, 
		LPVOID lpParam = NULL);
	//Function to show a message box
	static int ShowMessageBox(LPCWSTR title, LPCWSTR caption, UINT type = MB_OK);
	//Function to convert mouse position to grid position
	static int* MouseToGridPos(int x, int y);
	//Function to convert grid position to mouse position
	static int* GridToMousePos(int x, int y);
};