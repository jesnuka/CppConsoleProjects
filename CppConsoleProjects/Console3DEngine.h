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
using namespace std;


class ConsoleEngine
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

};