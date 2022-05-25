#pragma once
// Based on Command Line Asteroids by @Javidx9 at OneLoneCoder.com 

#include "ConsoleEngine.h"


class Asteroids : public ConsoleEngine
{
private:
	struct gameObject
	{
		float x;
		float y;
		float xDir; // Speed and direction X of object
		float yDir; // Speed and direction Y of object
		int size;
		float angle;
		bool dead;

	};

	// Store asteroids
	vector<gameObject> asteroids;
	// Store player bullets
	vector<gameObject> bullets;
	gameObject player;
	vector<pair<float, float>> modelShip;
	vector<pair<float, float>> modelAsteroid;

	// Player Values
	int score = 0;
	int highScore = 0;

protected:

	void RestartGame();

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);
	
	// Wrap coordinates around toroid
	void WrapCoordinates(float xInput, float yInput, float &xOutput, float &yOutput)
	{
		xOutput = xInput;
		yOutput = yInput;

		//Check if boundaries are crossed
		// If they are, place output on the other side of the screen
		if (xInput < 0.0f)
			xOutput = xInput + (float)screenWidth;
		if (xInput >= screenWidth)
			xOutput = xInput - (float)screenWidth;
		if (yInput < 0.0f)
			yOutput = yInput + (float)screenHeight;
		if (yInput >= screenHeight)
			yOutput = yInput - (float)screenHeight;
	}

	// Function for checking if a point is inside a circle by checking the distance
	bool PointInsideCircle(int x1, int y1, float radius, int x2, int y2)
	{
		return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2) * (y1 - y2)) < radius;
	}

	// Override draw to make use of WrapCoordinates function
	// Draw a character and color on coordinates
	virtual void Draw(int x, int y, wchar_t c = 0x2588, short color = 0x000F)
	{
		// Before actually drawing, wrap the coordinates
		float xWrapped;
		float yWrapped;
		WrapCoordinates(x, y, xWrapped, yWrapped);
		ConsoleEngine::Draw(xWrapped, yWrapped, c, color);
	}
};
