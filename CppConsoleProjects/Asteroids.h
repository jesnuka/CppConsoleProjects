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
};
