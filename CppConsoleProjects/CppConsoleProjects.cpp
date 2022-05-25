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
	virtual bool OnUserCreate()
	{
		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		return true;
	}

};

int main()
{
	//ConsoleFPS FPSGame;
	//FPSGame.RunEngine();

	Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}
