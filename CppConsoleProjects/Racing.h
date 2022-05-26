#pragma once

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

	// Fixed height on the screen
	int carPositionheight = 80;
	int carSpriteWidth = 7;

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};
