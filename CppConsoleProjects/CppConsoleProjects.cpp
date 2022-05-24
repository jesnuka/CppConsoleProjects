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

//#include "ConsoleFPS.h";
#include "ConsoleEngine.h"
#include "Asteroids.h";

using namespace std;

// Define static variables
std::atomic<bool> ConsoleEngine::atomActive(false);
std::condition_variable ConsoleEngine::gameFinished;
std::mutex ConsoleEngine::mutexGame;

// int selection = 0;

int main()
{
	Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();

	return 0;
}
