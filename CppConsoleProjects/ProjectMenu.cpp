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

#include "ConsoleFPS.h";
#include "ConsoleEngine.h"
#include "Asteroids.h";

using namespace std;

// Define static variables for Console Engine
std::atomic<bool> ConsoleEngine::atomActive(false);
std::condition_variable ConsoleEngine::gameFinished;
std::mutex ConsoleEngine::mutexGame;

// int selection = 0;

class ProjectMenu : public ConsoleEngine
{

private: 
	// Projects

	Asteroids asteroidsGame;
	ConsoleFPS FPSGame;


	int projectSelection = 0;
	int projectAmount = 2;
	vector<short> selectionColors;

	virtual bool OnUserCreate()
	{
		for (int i = 0; i < projectAmount; i++)
			selectionColors.push_back(FG_WHITE);

		selectionColors[0] = FG_RED;

		return true;
	}

	int StartProject(int project)
	{
		switch (project)
		{
		case 0:
			asteroidsGame.ConstructConsole(160, 100, 8, 8);
			asteroidsGame.Start();
			break;
		case 1:
			// Does not use ConsoleEngine currently
			ProjectMenu::ConstructConsole(120, 40, 9, 9);
			FPSGame.RunEngine();
			break;
		default:
			asteroidsGame.ConstructConsole(160, 100, 8, 8);
			asteroidsGame.Start();
			break;
		}
		return 1;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		// Empty the screen
		Fill(0, 0, screenWidth, screenHeight, L' ', 0);

		if (keys[VK_DOWN].isPressed)
		{
			selectionColors[projectSelection] = FG_WHITE;
			projectSelection = (projectSelection + 1) % projectAmount;
			selectionColors[projectSelection] = FG_RED;
		}
		if (keys[VK_UP].isPressed)
		{
			selectionColors[projectSelection] = FG_WHITE;
			projectSelection = (projectSelection - 1) % projectAmount;
			selectionColors[projectSelection] = FG_RED;
		}
		if (keys[VK_SPACE].isPressed || keys[VK_RETURN].isPressed)
		{
			StartProject(projectSelection);
		}

		// Draw menu on the screen
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4, L"C++ CONSOLE ENGINE PROJECTS MENU");
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 3, L"- ASTEROIDS - ", selectionColors[0]);
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 6, L"- FIRST PERSON RAY CAST MAZE - ", selectionColors[1]);

		// Draw selection lines


		return true;
	}

};

int main()
{
	ProjectMenu project;
	project.ConstructConsole(100, 80, 9, 9);
	project.Start();

//	

	return 0;
}
