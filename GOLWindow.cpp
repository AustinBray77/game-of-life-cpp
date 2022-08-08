#include "Program.h"
#include "GOLWindow.h"
#include "FileHandler.h"
#include "Game.h"
#include <iostream>
#include <math.h>
#include <string>

#define MINZOOM 0
#define MAXZOOM 99
#define ZOOMSPEED 0.01

//Stores render factory
ID2D1Factory* GOLWindow::pFactory = NULL;
//Stores render target
ID2D1HwndRenderTarget* GOLWindow::pRenderTarget = NULL;
//Stores colored brush
ID2D1SolidColorBrush* GOLWindow::lifeBrush = NULL;
//std::vector<std::vector<D2D1_RECT_F>> GOLWindow::rectangles = std::vector<std::vector<D2D1_RECT_F>>(0);

//Stores whether the user is drawing tiles or not
bool GOLWindow::isDrawing = false;
//Stores whether the user is erasing tiles or not
bool GOLWindow::isErasing = false;
//Stores whether the user paused the game or not
bool GOLWindow::isPaused = false;
//Stores the update speed for the game (ms)
int GOLWindow::UPDATESPEED = 10;
//Stores the zoom factor of the camera
int GOLWindow::zoomfactor = 0;
//Stores the camera's position for rendering
int GOLWindow::position[2] = { -1, -1 };

//Stores the hwnd
HWND GOLWindow::m_hwnd = NULL;
//Stores the last hresult for error checking
HRESULT GOLWindow::lastResult = 0;

//Function to interpret windows messages
LRESULT CALLBACK GOLWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	m_hwnd = hwnd;
	switch (uMsg)
	{
	//Called on window creation
	case WM_CREATE:
	{
		//Attempts to create the render factory, returns -1 if it fails
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
			return -1;

		//New file handler to handly .config
		FileHandler f = FileHandler(FileHandler::FHGetCurrentDirectory() + L"\\.config");

		int params[2] = { 0, 0 };

		auto lines = f.GetLines();

		//Loops through each line in .config and interprets them
		for (auto s : lines) {
			//Try catch for if a non number is attempted to be converted to a number
			try
			{
				//Triggers if line has CellAmount-, assigns the number to params[0]
				if (s.find("CellAmount-") == 0) {
					std::string num = s.substr(11);
					params[0] = std::stoi(num);
				}
				//Triggers if line has GridSize-, assigns the number to params[1]
				else if (s.find("GridSize-") == 0) {
					std::string num = s.substr(9);
					params[1] = std::stoi(num);
				}
				//Triggers if line has UpdateSpeed-, assigns the number to UPDATESPEED
				else if (s.find("UpdateSpeed-") == 0) {
					std::string num = s.substr(12);
					UPDATESPEED = std::stoi(num);
				}
			}
			catch (...) {}
		}

		//Resets random
		srand(time(NULL));

		//Assigns params[0], params[1], UPDATESPEED to be inbounds
		params[1] = params[1] <= 0 ? 
			128 : params[1] > MAX_GRID ? 
				MAX_GRID : params[1];
		params[0] = params[0] <= 0 || params[0] > params[1]*params[1] ? 
			params[1] * params[1] / (rand() % 5 + 2) : params[0];
		UPDATESPEED = UPDATESPEED <= 9 ? 10 : UPDATESPEED;

		//Initializes the game with params[0] and params[1]
		Game::init(params[0], params[1]);

		//Resets camera position
		position[0] = (Game::GRID_SIZE / 2) + 1;
		position[1] = (Game::GRID_SIZE / 2) + 1;

		return 0;
	}

	//Called when the user attempts to close the window
	case WM_CLOSE:
		//Asks the user if they want to close the window, destorys the window if they say yes
		if (MessageBox(m_hwnd, L"Do you really want to quit Game Of Life?", L"Game Of Life", MB_OKCANCEL) == IDOK) {
			DestroyWindow(m_hwnd);
		}
		return 0;

	//Called when the window is to be destroyed
	case WM_DESTROY:
		//DiscardsGraphicsResources
		DiscardGraphicsResources();
		//Releases the render factory
		SafeRelease(&pFactory);
		//Exits with code 0
		PostQuitMessage(0);
		return 0;

	//Called when the timer updates
	case WM_TIMER:
		//Updates the game
		Game::step();
		//Updates the frame
		OnPaint();
		return 0;
	
	//Called when the window is supposed to paint
	case WM_PAINT:
		//Updates the frame
		OnPaint();
		return 0;

	//Called when the window is resized
	case WM_SIZE:
		//Updates the window size
		Resize();
		return 0;

	//Called when a key is pushed
	case WM_KEYDOWN:
		switch (wParam) {
		//P key
		case 0x50:
			//Switches pause
			isPaused = !isPaused;

			//Triggers if paused and stops the update timer
			if (isPaused)
			{
				KillTimer(m_hwnd, 0);
			}
			//Triggers if not paused and restarts the update timer
			else {
				SetTimer(m_hwnd, 0, UPDATESPEED, (TIMERPROC)NULL);
			}
			return 0;
		//R key
		case 0x52:
			//Stops update timer
			KillTimer(m_hwnd, 0);

			//Initializes game
			Game::init(Game::startingCellCount, Game::GRID_SIZE);

			//Updates the frame
			OnPaint();

			//Sets to not playing
			Game::isPlaying = false;

			return 0;
		//S key
		case 0x53:
			//Starts update timer
			SetTimer(m_hwnd, 0, UPDATESPEED, (TIMERPROC)NULL);
			//Sets to playing
			Game::isPlaying = true;
			return 0;
		}
		return 0;

	//Called when the left mouse button is held down
	case WM_LBUTTONDOWN: {
		//Returns if game is playing
		if (Game::isPlaying)
			break;

		isDrawing = true;

		//Gets mouse x and y positinon
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//Gets grid pos from mouse pos, and sets that tile to active
		int* gridPos = MouseToGridPos(xPos, yPos);
		Game::tiles[gridPos[0]][gridPos[1]] = 1;

		//Updates the frame
		OnPaint();

		return 0;
	}

	//Called when the left mouse button is lifted				
	case WM_LBUTTONUP:
		isDrawing = false;
		return 0;

	//Called when the right mouse button is held down
	case WM_RBUTTONDOWN: {
		//Returns if game is playing
		if (Game::isPlaying)
			break;

		isErasing = true;

		//Gets mouse x and y positinon
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//Gets grid pos from mouse pos, and sets that tile to inactive
		int* gridPos = MouseToGridPos(xPos, yPos);
		Game::tiles[gridPos[0]][gridPos[1]] = 0;

		//Updates the frame
		OnPaint();

		return 0;
	}

	//Called when the right mouse button is lifted	
	case WM_RBUTTONUP:
		isErasing = false;
		return 0;

	//Called when the mouse is moved
	case WM_MOUSEMOVE: {
		//Returns if not drawing or erasing
		if (!isDrawing && !isErasing)
			break;

		//Gets mouse x and y positinon
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		//Gets grid pos from mouse pos, and sets that tile to active if drawing and inactive if erasing
		int* gridPos = MouseToGridPos(xPos, yPos);
		Game::tiles[gridPos[0]][gridPos[1]] = isDrawing ? 1 : 0;

		//Updates the frame
		OnPaint();

		return 0;
	}

	/*case WM_MOUSEWHEEL: {
		float mwDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		zoomfactor += mwDelta * ZOOMSPEED;

		zoomfactor = zoomfactor < MINZOOM ?
			MINZOOM : zoomfactor > MAXZOOM ?
				MAXZOOM : zoomfactor;

		CalculateLayout();
		OnPaint();

		return 0;
	}*/

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//Function to create the window
HRESULT GOLWindow::CreateWindowGOL(
	HINSTANCE hInstance, 
	LPCWSTR CLASS_NAME, 
	LPCWSTR TITLE, 
	DWORD dw_Style, 
	INT nCmdShow, 
	DWORD dwExStyle, 
	int width, 
	int height, 
	HWND parent, 
	HMENU menu, 
	LPVOID lpParam) {

	//Declares new window class
	WNDCLASS wc = { };

	//Window Protocol, Hinstance, and Class Name for the class
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	//Registers the class
	RegisterClass(&wc);

	//Creates a hwnd with the given parameters
	HWND hwnd = CreateWindowEx(
		dwExStyle,
		CLASS_NAME,
		TITLE,
		dw_Style,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		parent,
		menu,
		hInstance,
		lpParam
	);

	//Stores hwnd in m_hwnd
	m_hwnd = hwnd;

	//Triggers if creation was successful
	if (hwnd != NULL)
	{
		//Shows the window and returns
		ShowWindow(hwnd, nCmdShow);
		return 0;
	}

	//Returns -1 as hwnd creation was unsuccessful
	return -1;
}

//Function to create the graphics resources
HRESULT GOLWindow::CreateGraphicsResources() {
	HRESULT hr = S_OK;
	//Triggers if the render target has not been created yet
	if (pRenderTarget == NULL) {  
		//Gets rectangle for client area
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		//Gets size of rectangle
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		//Uses factory to create render target
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget
		);

		//Triggers if creation was successful
		if (SUCCEEDED(hr)) {
			//Creates brush for drawing
			const D2D1_COLOR_F green = D2D1::ColorF(0, 1.0f, 0);
			pRenderTarget->CreateSolidColorBrush(green, &lifeBrush);

			//CalculateLayout();
		}
	}
	return hr;
}

//Function to discard the graphics resources
void GOLWindow::DiscardGraphicsResources() {
	//Releases render target and brush
	SafeRelease(&pRenderTarget);
	SafeRelease(&lifeBrush);
}

//void GOLWindow::CalculateLayout() {
//	if (pRenderTarget == NULL)
//		return;
//
//	D2D1_SIZE_F size = pRenderTarget->GetSize();
//
//	rectangles = std::vector<std::vector<D2D1_RECT_F>>(Game::GRID_SIZE, std::vector<D2D1_RECT_F>(Game::GRID_SIZE));
//
//	for (int i = 0; i < Game::GRID_SIZE; i++) {
//		for (int j = 0; j < Game::GRID_SIZE; j++) {
//			int *topLeft = GridToMousePos(i, j), *bottomRight = GridToMousePos(i + 1, j + 1);
//			rectangles[i][j] = D2D1::RectF(topLeft[0], topLeft[1], bottomRight[0], bottomRight[1]);
//		}
//	}
//}

//Function to paint the window
void GOLWindow::OnPaint() {
	//Creates graphics resources
	HRESULT hr = CreateGraphicsResources();
	//Triggers if creation was successful
	if (SUCCEEDED(hr)) {
		//Gets device context and initializes the paint struct
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);

		//Begins render target drawing cycle
		pRenderTarget->BeginDraw();

		//Fills the screen with black
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		//Loops through each tile
		for (int i = 0; i < Game::GRID_SIZE; i++) {
			for (int j = 0; j < Game::GRID_SIZE; j++) {
				//Triggers if tile is active
				if (Game::tiles[i][j])
				{
					//Sets brush color to color between green and red depending on lifespan, green = young red = old
					lifeBrush->SetColor(D2D1::ColorF(Game::tiles[i][j] / Game::oldestCell, 1 - Game::tiles[i][j] / Game::oldestCell, 0.0f));

					//Gets corners for tiles
					int* topLeft = GridToMousePos(i, j), * bottomRight = GridToMousePos(i + 1, j + 1);

					//Draws the tile
					pRenderTarget->FillRectangle(D2D1::RectF(topLeft[0], topLeft[1], bottomRight[0], bottomRight[1]), lifeBrush);

					//Deallocates pointers
					delete[] topLeft;
					delete[] bottomRight;
				}
			}
		}

		//Ends render target drawing cycle
		hr = pRenderTarget->EndDraw();

		//Triggers if end draw failed
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			//Discards the graphic resources
			DiscardGraphicsResources();
		}

		//Ends paintstruct paint
		EndPaint(m_hwnd, &ps);
	}
	//Triggers if creation failed
	else {
		//Sets last result to the hresult
		lastResult = hr;
		//Quits with hresult exit code
		PostQuitMessage(hr);
	}
}

//Function to resize the window
void GOLWindow::Resize() {
	//Returns if render target is null
	if (pRenderTarget == NULL)
		return;

	//Gets rectangle for client region
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	//Gets size of client region
	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

	//Resizes the render target
	pRenderTarget->Resize(size);
	//CalculateLayout();
	//Invalidates the previous rect
	InvalidateRect(m_hwnd, NULL, FALSE);
}
//Function to show a message box
int GOLWindow::ShowMessageBox(LPCWSTR title, LPCWSTR text, UINT type) {
	//Returns the message box
	return MessageBox(m_hwnd, text, title, type);
}

//Function to convert mouse position to grid position
int* GOLWindow::MouseToGridPos(int x, int y) {
	//Gets render target size
	D2D1_SIZE_F size = pRenderTarget->GetSize();

	//Calculates the ratio of grid count to screen size
	const float xFactor = (size.width / Game::GRID_SIZE) * (zoomfactor + 1);
	const float yFactor = (size.height / Game::GRID_SIZE) * (zoomfactor + 1);

	//Returns grid pos / ratio - zoomfactor(this doesn't matter right now as it will always be 0)
	return new int[2]{ (int)round(x / xFactor) - (int)(position[0] * zoomfactor * xFactor), (int)round(y / yFactor) - (int)(position[1] * zoomfactor * yFactor) };
}

//Function to convert grid position to mouse position
int* GOLWindow::GridToMousePos(int x, int y) {
	//Gets render target size
	D2D1_SIZE_F size = pRenderTarget->GetSize();

	//Calculates the ratio of grid count to screen size
	const float xFactor = (size.width / Game::GRID_SIZE) * (zoomfactor + 1);
	const float yFactor = (size.height / Game::GRID_SIZE) * (zoomfactor + 1);

	//Returns grid pos * ratio - zoomfactor(this doesn't matter right now as it will always be 0)
	return new int[2]{ (int)(x * xFactor) - (int)(position[0] * zoomfactor * xFactor), (int)(y * yFactor) - (int)(position[1] * zoomfactor * yFactor) };
}