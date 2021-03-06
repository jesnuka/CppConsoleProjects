#pragma once

// Based on Command Line Arcade Racer Game by @Javidx9 at OneLoneCoder.com 
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

class Racing : public ConsoleEngine
{
public:
	Racing()
	{
		appName = L"Racing";
	}

private:
	// Racing stats
	// Distance driven around the track
	float distance = 0.0f;

	// Track
	// Tracks consist of Curvature and Distance
	// Whole track is formed out of these track segments
	vector<pair<float, float>> track;
	int trackSection = 0;
	float lapDistance = 0.0f;
	float lapTime = 0.0f;
	float currentCurvature = 0.0f;
	float trackPerfectCurvature = 0.0f;
	list<float> lapTimes;

	// Player Car
	float carPositionFloat = 0.0f;
	float carAcceleration = 50.0f;
	float carVelocity = 0.0f;
	float carVelocityMax = 100.0f;
	float carCurvature = 0.0f;
	int carDirection = 0;

	// Fixed height on the screen
	int carPositionheight = 80;
	int carSpriteWidth = 7;

private: 

	void DrawCarDirection(int carPosition);

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};
