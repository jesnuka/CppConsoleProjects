#pragma once
#pragma once
#include "ConsoleEngine.h"

// Based on Ultimate Console FPS by Javidx9
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
class ColoredConsoleFPS : public ConsoleEngine
{
public:
	ColoredConsoleFPS()
	{
		appName = L"Colored Console FPS";
	}

private:
	// Settings
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
	wstring map;
	int mapHeight = 32;
	int mapWidth = 32;
	int mapMode = 0;

private:
	wstring CreateMap(int mapWidth, int mapHeight, int mapMode);

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};