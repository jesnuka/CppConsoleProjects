#pragma once
// Based on Command Line Asteroids by @Javidx9 at OneLoneCoder.com 

#include "ConsoleEngine.h"


class Asteroids : public ConsoleEngine
{
public:
	Asteroids()
	{

	}

private:
	struct gameObject
	{
		float x;
		float y;
		float xDir; // Speed and direction X of object
		float yDir; // Speed and direction Y of object
		int size;

	};

	vector<gameObject> asteroids;

protected:
	virtual bool OnUserCreate()
	{
		asteroids.push_back({20.0f , 20.0f, 8.0f, -6.0f, (int)16});

		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		// Empty screen
		Fill(0, 0, screenWidth, screenHeight, L' ', 0);

		// Update and draw asteroids
		for (auto& ast : asteroids)
		{
			// Position = Velocity * Time (+ Old Position)
			ast.x += ast.xDir * elapsedTime;
			ast.y += ast.yDir * elapsedTime;
			WrapCoordinates(ast.x, ast.y, ast.x, ast.y);

			for (int i = 0; i < ast.size; i++)
			{
				for (int j = 0; j < ast.size; j++)
				{
					Draw(ast.x + i, ast.y + j, PIXEL_QUARTER, FG_RED);
				}

			}
		}

		return true;
	}
	
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
