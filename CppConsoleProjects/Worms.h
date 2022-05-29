#pragma once

// Based on Console Worms by @Javidx9 at OneLoneCoder.com 
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

#include "ConsoleEngine.h"
using namespace std;

class Worms : public ConsoleEngine
{
public:
	Worms()
	{
		appName = L"Worms";
	}

private:

	// Map
	int mapWidth = 1024;
	int mapHeight = 512;

	// Camera
	float cameraPosX = 0.0f;
	float cameraPosY = 0.0f;
	float mapScrollSpeed = 200.0f;

	unsigned char* map = nullptr;

	void CreateMap();
	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput);

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);
};