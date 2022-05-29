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

#include "ConsoleFPS.h"
#include "ConsoleEngine.h"
#include "SpriteEditor.h"
#include "Asteroids.h"
#include "Racing.h"
#include "ImprovedConsoleFPS.h"
#include "ColoredConsoleFPS.h"
#include "Splines.h"
#include "Worms.h"

using namespace std;

// Define static variables for Console Engine
std::atomic<bool> ConsoleEngine::atomActive(false);
std::condition_variable ConsoleEngine::gameFinished;
std::mutex ConsoleEngine::mutexGame;

class ProjectMenu : public ConsoleEngine
{

private: 
	// Projects

	Racing racingGame;
	Asteroids asteroidsGame;
	ConsoleFPS FPSGame;
	ImprovedConsoleFPS improvedFPSGame;
	ColoredConsoleFPS coloredFPSGame;
	Splines splines;
	SpriteEditor spriteEditor;
	Worms wormsGame;


	int projectSelection = 0;
	int projectAmount = 6;
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
			racingGame.ConstructConsole(160, 100, 8, 8);
			racingGame.Start();
			break;
		case 1:
			asteroidsGame.ConstructConsole(160, 100, 8, 8);
			asteroidsGame.Start();
			break;
		case 2:
			splines.ConstructConsole(160, 80, 9, 9);
			splines.Start();
			break;
		case 3:
			improvedFPSGame.ConstructConsole(120, 40, 9, 9);
			improvedFPSGame.Start();
				break;
		case 4:
			wormsGame.ConstructConsole(256, 160, 5, 5);
			wormsGame.Start();
			break;
		case 5:
			spriteEditor.ConstructConsole(160, 100, 8, 8);
			spriteEditor.Start();
			break;
		/*case 3:
			coloredFPSGame.ConstructConsole(200, 100, 6, 6);
			coloredFPSGame.Start();*/
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
			projectSelection = ((projectSelection + 1) % projectAmount + projectAmount) % projectAmount;
			selectionColors[projectSelection] = FG_RED;
		}
		if (keys[VK_UP].isPressed)
		{
			selectionColors[projectSelection] = FG_WHITE;
			projectSelection = ((projectSelection - 1) % projectAmount + projectAmount) % projectAmount;
			selectionColors[projectSelection] = FG_RED;
		}
		if (keys[VK_SPACE].isPressed || keys[VK_RETURN].isPressed)
		{
			StartProject(projectSelection);
		}

		// Draw menu on the screen
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4, L"C++ CONSOLE ENGINE PROJECTS MENU");
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 3, L"- RACING - ", selectionColors[0]);
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 6, L"- ASTEROIDS - ", selectionColors[1]);
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 9, L"- SPLINES - ", selectionColors[2]);
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 12, L"- RAY CAST FPS - ", selectionColors[3]);
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 15, L"- WORMS - ", selectionColors[4]);
		DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 18, L"- SPRITE EDITOR - ", selectionColors[5]);
		//DrawString(screenWidth/2 - 10, screenHeight/2 - screenHeight / 4 + 15, L"- ADVANCED RAY CAST FPS - ", selectionColors[4]);

		// Draw selection lines


		return true;
	}

};

int main()
{
	ProjectMenu project;
	project.ConstructConsole(100, 80, 9, 9);
	project.Start();

	return 0;
}
