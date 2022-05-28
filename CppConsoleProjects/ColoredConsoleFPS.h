#pragma once
#pragma once
#include "ConsoleEngine.h"

// Based on Ultimate Console FPS by @Javidx9 at OneLoneCoder.com 

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