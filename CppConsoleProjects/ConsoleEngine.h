#pragma once

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
using namespace std;

// Based on Command Line Game Engine by @Javidx9 at OneLoneCoder.com 
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

class ConsoleEngine
{
public:

	ConsoleEngine()
	{
		screenWidth = 120;
		screenHeight = 40;

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
		if (!SetConsoleMode(console, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			Error(L"SetConsoleMode");

		// Allocate screen buffer memory
		bufferScreen = new CHAR_INFO[screenWidth * screenHeight];
		memset(bufferScreen, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);


		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);

	}

	// Draw a character and color on coordinates
	virtual void Draw(int x, int y, wchar_t c = 0x2588, short color = 0x000F)
	{
		// Draw on coordinates of screen buffer, if in bounds
		if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
		{
			bufferScreen[y * screenWidth + x].Char.UnicodeChar = c;
			bufferScreen[y * screenWidth + x].Attributes = color;

		}
	}

	// Fill an area with character and color
	void Fill(int xStart, int yStart, int xEnd, int yEnd, wchar_t c = 0x2588, short color = 0x000F)
	{
		BoundaryCheck(xStart, yStart);
		BoundaryCheck(xEnd, yEnd); // TODO: Clip
		for (int i = xStart; i < xEnd; i++)
		{
			for (int j = yStart; j < yEnd; j++)
				Draw(i, j, c, color);
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
						consoleInFocus = inputBuffer[i].Event.FocusEvent.bSetFocus;
						break;

					case MOUSE_EVENT:
						switch (inputBuffer[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
							mousePositionX = inputBuffer[i].Event.MouseEvent.dwMousePosition.X;
							mousePositionY = inputBuffer[i].Event.MouseEvent.dwMousePosition.Y;
							break;

						case 0:
							for (int j = 0; j < 5; j++)
								mouseNewState[j] = (inputBuffer[i].Event.MouseEvent.dwButtonState & (1 << j)) > 0;
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

// Define static variables
std::atomic<bool> ConsoleEngine::atomActive(false);
std::condition_variable ConsoleEngine::gameFinished;
std::mutex ConsoleEngine::mutexGame;
