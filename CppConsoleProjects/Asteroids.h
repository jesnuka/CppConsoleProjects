#pragma once
// Based on Command Line Asteroids by @Javidx9 at OneLoneCoder.com 

#include "ConsoleEngine.h"


class Asteroids : public ConsoleEngine
{
public:
	Asteroids()
	{

	}

protected:
	virtual bool OnUserCreate()
	{

		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		// Empty screen
		Fill(0, 0, screenWidth, screenHeight, L' ', 0);

		return true;
	}
};
