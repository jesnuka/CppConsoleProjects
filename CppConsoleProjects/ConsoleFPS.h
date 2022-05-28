#pragma once

// Based on Console FPS by Javidx9
/*
License
	~~~~~~~
	Copyright (C) 2018  Javidx9
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
using namespace std;


//class ConsoleFPS : public ConsoleEngine
class ConsoleFPS
{
private:
	// Settings
	int screenWidth = 120;
	int screenHeight = 40;
	bool paused = false;

	// Player
	float playerX = 8.0f;
	float playerY = 8.0f;
	float playerAngle = 0.0f;

	float playerRotationSpeed = 2.0f;
	float playerMovementSpeed = 5.0f;

	float FOV = M_PI / 4.0;
	float depth = 16.0f;

	//Map
	int mapHeight = 16;
	int mapWidth = 16;
	int mapMode = 0;


public:
	wstring CreateMap(int mapWidth, int mapHeight, int mapMode);
	void RunEngine();

/*protected:
	virtual bool OnUserCreate()
	{
		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		return true;
	}
	*/
};