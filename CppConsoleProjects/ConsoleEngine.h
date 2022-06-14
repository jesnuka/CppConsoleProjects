#pragma once

// Based on Command Line Game Engine by @Javidx9 at OneLoneCoder.com 
/*
	License
	~~~~~~~
	One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details.
	Original works located at:
		https://www.github.com/onelonecoder
		https://www.onelonecoder.com
		https://www.youtube.com/javidx9

	GNU GPLv3
		https://github.com/OneLoneCoder/videos/blob/master/LICENSE
*/

#define _USE_MATH_DEFINES
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>
#include <condition_variable>
#include <list>
using namespace std;


enum Color
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

// For drawing full colored pixels, instead of character
enum Pixel
{
	PIXEL_FULL = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591
};

class ConsoleSprite
{
public:
	ConsoleSprite()
	{

	}

	ConsoleSprite(int width, int height)
	{
		CreateSprite(width, height);
	}

	// For loading files
	ConsoleSprite(std::wstring sFile)
	{
		if (!Load(sFile))
			CreateSprite(8, 8);
	}

	int sWidth = 0;
	int sHeight = 0;


private:
	wchar_t *symbols = nullptr;
	wchar_t *colours = nullptr;

	void CreateSprite(int width, int height)
	{
		// Initialize variables

		sWidth = width;
		sHeight = height;

		symbols = new wchar_t[width*height];
		colours = new wchar_t[width*height];

		for (int i = 0; i < width * height; i++)
		{
			symbols[i] = L' ';
			colours[i] = FG_BLACK;
		}
	}

public:

	// These are used to set or get symbol or colour at position xy of the sprite
	void SetSymbol(int x, int y, wchar_t c)
	{

		if (x < 0 || x >= sWidth || y < 0 || y >= sHeight)
			return;
		else
			symbols[y * sWidth + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x < 0 || x >= sWidth || y < 0 || y >= sHeight)
			return;
		else
			colours[y * sWidth + x] = c;
	}

	short GetSymbol(int x, int y)
	{
		if (x < 0 || x >= sWidth || y < 0 || y >= sHeight)
			return L' ';
		else
			return symbols[y * sWidth + x];
	}

	short GetColour(int x, int y)
	{
		if (x < 0 || x >= sWidth || y < 0 || y >= sHeight)
			return FG_BLACK;
		else
			return colours[y * sWidth + x];
	}

	short SampleSymbol(float x, float y)
	{
		int sx = (int)(x * (float)sWidth);
		int sy = (int)(y * (float)sHeight - 1.0f);
		if (sx < 0 || sx >= sWidth || sy < 0 || sy >= sHeight)
			return L' ';
		else
			return symbols[sy * sWidth + sx];
	}

	short SampleColour(float x, float y)
	{
		int sx = (int)(x * (float)sWidth);
		int sy = (int)(y * (float)sHeight - 1.0f);
		if (sx < 0 || sx >= sWidth || sy < 0 || sy >= sHeight)
			return FG_BLACK;
		else
			return colours[sy * sWidth + sx];
	}

	// Save sprite as file
	bool Save(std::wstring sFile)
	{
		FILE* f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite(&sWidth, sizeof(int), 1, f);
		fwrite(&sHeight, sizeof(int), 1, f);
		fwrite(colours, sizeof(short), sWidth * sHeight, f);
		fwrite(symbols, sizeof(short), sWidth * sHeight, f);

		fclose(f);

		return true;
	}

	// Load sprite from file
	bool Load(std::wstring sFile)
	{
		delete[] symbols;
		delete[] colours;
		sWidth = 0;
		sHeight = 0;

		FILE* f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		std::fread(&sWidth, sizeof(int), 1, f);
		std::fread(&sHeight, sizeof(int), 1, f);

		CreateSprite(sWidth, sHeight);

		std::fread(colours, sizeof(short), sWidth * sHeight, f);
		std::fread(symbols, sizeof(short), sWidth * sHeight, f);

		std::fclose(f);
		return true;
	}
};

class ConsoleEngine
{
public:

	ConsoleEngine()
	{
		screenWidth = 80;
		screenHeight = 30;

		console = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleInput = GetStdHandle(STD_INPUT_HANDLE);

		memset(keyNewState, 0, 256 * sizeof(short));
		memset(keyOldState, 0, 256 * sizeof(short));
		memset(keys, 0, 256 * sizeof(short));
		mousePositionX = 0;
		mousePositionY = 0;

		appName = L"Demo";

	}

	int ConstructConsole(int width, int height, int fontWidth, int fontHeight)
	{

		if (console == INVALID_HANDLE_VALUE)
			return Error(L"Bad Handle");

		screenWidth = width;
		screenHeight = height;

		// Resize console window to  minimum to shrink ScreenBuffer
		rectWindow = {0, 0, 1, 1};
		SetConsoleWindowInfo(console, TRUE, &rectWindow);

		// Set ScreenBuffer size
		COORD coord = {(short)screenWidth, (short)screenHeight};
		if (!SetConsoleScreenBufferSize(console, coord))
			Error(L"SetConsoleScreenBufferSize");

		// Now, assign the ScreenBuffer to the console
		if (!SetConsoleActiveScreenBuffer(console))
			return Error(L"SetConsoleActiveScreenBuffer");

		// Set the font settings
		CONSOLE_FONT_INFOEX consoleFontIndex;
		consoleFontIndex.cbSize = sizeof(consoleFontIndex);
		consoleFontIndex.nFont = 0;
		consoleFontIndex.dwFontSize.X = fontWidth;
		consoleFontIndex.dwFontSize.Y = fontHeight;
		consoleFontIndex.FontFamily = FF_DONTCARE;
		consoleFontIndex.FontWeight = FW_NORMAL;

		wcscpy_s(consoleFontIndex.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(console, FALSE, &consoleFontIndex))
			return Error(L"SetCurrentConsoleFontEx");

		// Get screen buffer information and window maximum size
		CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
		if (!GetConsoleScreenBufferInfo(console,&consoleScreenBufferInfo))
			return Error(L"GetConsoleScreenBufferInfo");
		if (screenWidth > consoleScreenBufferInfo.dwMaximumWindowSize.X)
			return Error(L"Screen Height or Font Too large");
		if (screenHeight > consoleScreenBufferInfo.dwMaximumWindowSize.Y)
			return Error(L"Screen Height or Font Too large");


		// Set the console window size
		rectWindow = { 0,0, (short)(screenWidth - 1), (short)(screenHeight - 1) };
		if (!SetConsoleWindowInfo(console, TRUE, &rectWindow))
			return  Error(L"SetConsoleWindowInfo");

		// Allow mouse input by setting flags
		if (!SetConsoleMode(consoleInput, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			Error(L"SetConsoleMode");

		// Allocate screen buffer memory
		bufferScreen = new CHAR_INFO[screenWidth * screenHeight];
		memset(bufferScreen, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);


		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);

	}

	// Draw a character and color on coordinates
	virtual void Draw(int x, int y, wchar_t c = PIXEL_FULL, short color = FG_WHITE)
	{
		// Draw on coordinates of screen buffer, if in bounds
		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
		{
			bufferScreen[y * screenWidth + x].Char.UnicodeChar = c;
			bufferScreen[y * screenWidth + x].Attributes = color;

		}
	}

	// Draw a string / text on the screen at coordinates
	virtual void DrawString(int x, int y, wstring str, short color = FG_WHITE, bool alpha = false)
	{
		for (int i = 0; i < str.size(); i++)
		{
			// If alpha enabled, don't draw on empty space
			if (alpha)
			{
				if (str[i] != L' ')
				{
					bufferScreen[y * screenWidth + x + i].Char.UnicodeChar = str[i];
					bufferScreen[y * screenWidth + x + i].Attributes = color;
				}
			}
			else
			{
				bufferScreen[y * screenWidth + x + i].Char.UnicodeChar = str[i];
				bufferScreen[y * screenWidth + x + i].Attributes = color;
			}
		}
	}

	// Draw sprite on coordinates
	void DrawSprite(int x, int y, ConsoleSprite* sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->sWidth; i++)
		{
			for (int j = 0; j < sprite->sHeight; j++)
			{
				if (sprite->GetSymbol(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetSymbol(i, j), sprite->GetColour(i, j));
			}
		}
	}

	// Draw part of a sprite on coordinates
	void DrawPartialSprite(int x, int y, ConsoleSprite* sprite, int ox, int oy, int w, int h, bool mirroredX = false, bool mirroredY = false)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				if (sprite->GetSymbol(i + ox, j + oy) != L' ')
				{
					int coordX = (x + i);
					int coordY = (y + j);
					if (mirroredX)
						coordX = w + (x - i);
					if (mirroredY)
						coordY = h + (y - j);

					Draw(coordX, coordY, sprite->GetSymbol(i + ox, j + oy), sprite->GetColour(i + ox, j + oy));
				}
			}
		}
		

	}

	// Draw line from start to end
	void Line(int xStart, int yStart, int xEnd, int yEnd, wchar_t c = PIXEL_FULL, short color = FG_WHITE)
	{
		// Use Bresenham's Line Algorithm
		int x = 0;
		int y = 0;
		int dx = 0;
		int dxA = 0;
		int dy = 0;
		int dyA = 0;
		int px = 0;
		int py = 0;
		int xe = 0;
		int ye = 0;
		int i = 0;

		dx = xEnd - xStart;
		dy = yEnd - yStart;
		dxA = abs(dx);
		dyA = abs(dy);
		px = 2 * dyA - dxA;
		py = 2 * dxA - dyA;

		if (dyA <= dxA)
		{
			if (dx >= 0)
			{
				x = xStart;
				y = yStart;
				xe = xEnd;
			}
			else
			{
				x = xEnd;
				y = yEnd;
				xe = xStart;
			}

			Draw(x,y,c,color);

			for (i = 0; x < xe; i++)
			{
				x += 1;
				if (px < 0)
					px += (2 * dyA);

				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						y += 1;
					else
						y -= 1;

					px += 2 * (dyA - dxA);
				}

				Draw(x,y,c,color);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = xStart;
				y = yStart;
				ye = yEnd;
			}
			else
			{
				x = xEnd;
				y = yEnd;
				ye = yStart;
			}

			Draw(x, y, c, color);

			for (i = 0; y < ye; i++)
			{
				y += 1;
				if (py <= 0)
					py += (2 * dxA);

				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						x += 1;
					else
						x -= 1;

					py += 2 * (dxA - dyA);
				}

				Draw(x, y, c, color);
			}
		}

	}

	// Fill an area with character and color
	void Fill(int xStart, int yStart, int xEnd, int yEnd, wchar_t c = PIXEL_FULL, short color = FG_WHITE)
	{
		BoundaryCheck(xStart, yStart);
		BoundaryCheck(xEnd, yEnd);
		for (int x = xStart; x < xEnd; x++)
		{
			for (int y = yStart; y < yEnd; y++)
				Draw(x, y, c, color);
		}
	}

	// Check if coordinates are out of bounds, in which case limit then to in-bounds
	void BoundaryCheck(int &x, int &y)
	{
		if (x < 0)
			x = 0;
		if (x >= screenWidth)
			x = screenWidth;
		if (y < 0)
			y = 0;
		if (y >= screenHeight)
			y = screenHeight;
	}

	// Draw wireframe model to coordinates, with rotation and scale
	void WireFrame(const vector<pair<float, float>> modelCoordinates, float x, float y, float rotation = 0.0f, float scale = 1.0f, wchar_t c = PIXEL_FULL, short color = FG_WHITE)
	{
		// Create translated model vector from the pairs of coordinates
		vector<pair<float, float>> transCoordinates;
		int vertices = modelCoordinates.size(); // Amount of vertices in the model
		transCoordinates.resize(vertices);

		// Rotate
		for (int i = 0; i < vertices; i++)
		{
			transCoordinates[i].first = modelCoordinates[i].first * cosf(rotation) - modelCoordinates[i].second * sinf(rotation);
			transCoordinates[i].second = modelCoordinates[i].first * sinf(rotation) + modelCoordinates[i].second * cosf(rotation);
		}

		// Scale
		for (int i = 0; i < vertices; i++)
		{
			transCoordinates[i].first = transCoordinates[i].first * scale;
			transCoordinates[i].second = transCoordinates[i].second * scale;
		}

		// Translate
		for (int i = 0; i < vertices; i++)
		{
			transCoordinates[i].first = transCoordinates[i].first + x;
			transCoordinates[i].second = transCoordinates[i].second + y;
		}

		// Draw lines from vertices to vertices
		for (int i = 0; i < vertices; i++)
		{
			Line((int)transCoordinates[i % vertices].first, (int)transCoordinates[i % vertices].second,
				(int)transCoordinates[(i+1) % vertices].first, (int)transCoordinates[(i + 1) % vertices].second, c, color);
		}
	}

	// Destructor
	~ConsoleEngine()
	{
		SetConsoleActiveScreenBuffer(consoleOriginal);
		delete[] bufferScreen;
	}

	void Start()
	{
		// Start thread
		atomActive = true;
		std::thread thread = std::thread(&ConsoleEngine::GameThread, this);

		// Wait for thread to exit
		thread.join();
	}


	// These must be overridden
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float elapsedTime) = 0;

	// Cleaning up
	virtual bool OnUserDestroy() { return true; }

private:

	void GameThread()
	{

		// Create user resources
		if (!OnUserCreate())
			atomActive = false;


		// Frame time
		auto tp1 = chrono::steady_clock::now();
		auto tp2 = chrono::steady_clock::now();

		while (atomActive)
		{
			// Run
			while (atomActive)
			{
				// Frame time
				auto tp2 = chrono::steady_clock::now();
				chrono::duration<float> timeDifference = tp2 - tp1;
				tp1 = tp2;
				float elapsedTime = timeDifference.count();

				// Handle and store input for each key
				for (int i = 0; i < 256; i++)
				{
					keyNewState[i] = GetAsyncKeyState(i);

					keys[i].isPressed = false;
					keys[i].isReleased = false;

					if (keyNewState[i] != keyOldState[i])
					{
						if (keyNewState[i] & 0x8000)
						{
							keys[i].isPressed = !keys[i].isHeld;
							keys[i].isHeld = true;
						}
						else
						{
							keys[i].isReleased = true;
							keys[i].isHeld = false;
						}
					}

					keyOldState[i] = keyNewState[i];
				}


				// Handle mouse input
				INPUT_RECORD inputBuffer[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(consoleInput, &events);
				if (events > 0)
					ReadConsoleInput(consoleInput, inputBuffer, events, &events);

				// Handle each mouse event and store input
				for (DWORD i = 0; i < events; i++)
				{
					switch (inputBuffer[i].EventType)
					{
					case FOCUS_EVENT:
					{
						consoleInFocus = inputBuffer[i].Event.FocusEvent.bSetFocus;
					}
						break;

					case MOUSE_EVENT:
						switch (inputBuffer[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
						{
							mousePositionX = inputBuffer[i].Event.MouseEvent.dwMousePosition.X;
							mousePositionY = inputBuffer[i].Event.MouseEvent.dwMousePosition.Y;
						}
							break;

						case 0:
						{
							for (int j = 0; j < 5; j++)
								mouseNewState[j] = (inputBuffer[i].Event.MouseEvent.dwButtonState & (1 << j)) > 0;
						}
							break;
						default:
							break;
						}
						break;
					default:
						break;
					}
				}

				for (int j = 0; j < 5; j++)
				{
					mouse[j].isPressed = false;
					mouse[j].isReleased = false;

					if (mouseNewState[j] != mouseOldState[j])
					{
						if (mouseNewState[j])
						{
							mouse[j].isPressed = !mouse[j].isHeld;
							mouse[j].isHeld = true;
						}
						else
						{
							mouse[j].isReleased = true;
							mouse[j].isHeld = false;
						}
					}

					mouseOldState[j] = mouseNewState[j];
				}

				// Frame update
				if (!OnUserUpdate(elapsedTime))
					atomActive = false;

				// Update window title and finally the screen buffer
				wchar_t title[256];
				swprintf_s(title, 256, L"Console Engine - %s - FPS: %3.2f", appName.c_str(), 1.0f / elapsedTime);
				SetConsoleTitle(title);
				WriteConsoleOutput(console, bufferScreen, { (short)screenWidth, (short)screenHeight }, { 0,0 }, &rectWindow);
			}

			// If overridden, allow resources to be freed
			if (OnUserDestroy())
			{
				// Clean up and exit
				delete[] bufferScreen;
				SetConsoleActiveScreenBuffer(consoleOriginal);
				gameFinished.notify_one();

			}
			else // Continue running
				atomActive = true;
		}
	}

protected:

	// Error output handler
	int Error(const wchar_t* message)
	{
		wchar_t buffer[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		SetConsoleActiveScreenBuffer(consoleOriginal);

		// Print error message
		wprintf(L"ERROR: %s\n\t%s\n", message, buffer);
		return 0;
	}

	// Called from separate OS Thread
	static BOOL CloseHandler(DWORD evt)
	{
		// Must wait for game to be finished cleaning up, otherwise the process is killed before OnUserDestroy() is finished
		if (evt == CTRL_CLOSE_EVENT)
		{
			atomActive = false;
			std::unique_lock<std::mutex> ul(mutexGame);
			gameFinished.wait(ul);
		}

		return true;

	}

	// Settings
	int screenWidth;
	int screenHeight;
	HANDLE console;
	HANDLE consoleInput;
	HANDLE consoleOriginal;
	wstring appName;
	SMALL_RECT rectWindow;
	CHAR_INFO *bufferScreen;
	bool consoleInFocus = true;


	// Input
	short keyOldState[256] = { 0 };
	short keyNewState[256] = { 0 };
	short mouseOldState[5] = { 0 };
	short mouseNewState[5] = { 0 };

	struct KeyState
	{
		bool isPressed;
		bool isReleased;
		bool isHeld;
	} keys[256], mouse[5];

	int mousePositionX;
	int mousePositionY;

	// Related to OS OnDestroy calls
	static std::atomic<bool> atomActive;
	static std::condition_variable gameFinished;
	static std::mutex mutexGame;

};

