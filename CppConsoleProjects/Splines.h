#pragma once

// Based on Splines by Javidx9
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


#include "ConsoleEngine.h"
using namespace std;



class Splines : public ConsoleEngine
{
public:
	Splines
	{
		appName = "Splines";
	}
protected:

	void RestartGame();

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};