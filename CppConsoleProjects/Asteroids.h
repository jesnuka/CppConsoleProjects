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
		float angle;

	};

	vector<gameObject> asteroids;
	gameObject player;

protected:
	virtual bool OnUserCreate()
	{
		// Spawn single asteroid
		asteroids.push_back({20.0f , 20.0f, 8.0f, -6.0f, (int)16, 0});

		// Initialize the Player
		player.x = screenWidth / 2;
		player.y = screenHeight / 2;
		player.xDir = 0;
		player.yDir = 0;
		player.angle = 0;

		
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
		
		// Player Steering
		if (keys[VK_LEFT].isHeld)
			player.angle -= 5.0f * elapsedTime;
		if (keys[VK_RIGHT].isHeld)
			player.angle += 5.0f * elapsedTime;

		// Player Thrust Movement (Acceleration)
		if (keys[VK_UP].isHeld)
		{
			// Acceleration = Velocity * Time
			// cos is negative, because 0 is at the top, and screenHeight is at the bottom of the screen
			player.xDir += sin(player.angle) * 20.0f * elapsedTime;
			player.yDir += -cos(player.angle) * 20.0f * elapsedTime;

		}
		// Can't go backwards
		/*if (keys[VK_DOWN].isHeld)
		{
			// Acceleration = Velocity * Time
			// cos is negative, because 0 is at the top, and screenHeight is at the bottom of the screen
			player.xDir -= sin(player.angle) * 20.0f * elapsedTime;
			player.yDir -= -cos(player.angle) * 20.0f * elapsedTime;

		}*/

		player.x += player.xDir * elapsedTime;
		player.y += player.yDir * elapsedTime;

		WrapCoordinates(player.x, player.y, player.x, player.y);


		// Draw the Player Ship
		// Ship consists of 3 points to form a triangle
		// These never change
		float xModel[3] = {0.0f, -2.5f, 2.5f};
		float yModel[3] = {-5.5f, 2.5f, 2.5f};

		// These are the points that are rotated and drawn to the screen
		float xShip[3];
		float yShip[3];

		// Rotate points
		for (int i = 0; i < 3; i++)
		{
			xShip[i] = xModel[i] * cosf(player.angle) - yModel[i] * sinf(player.angle);
			yShip[i] = xModel[i] * sinf(player.angle) + yModel[i] * cosf(player.angle);
		}

		// Translate points to player's location
		for (int i = 0; i < 3; i++)
		{
			xShip[i] = xShip[i] + player.x;
			yShip[i] = yShip[i] + player.y;
		}

		// Draw the points now, that are rotated and translated
		for (int i = 0; i < 4; i++)
		{
			int j = (i + 1);
			//Draw(xShip[i % 3], yShip[i % 3], PIXEL_FULL, FG_BLUE);
			//Line(xShip[i % 3], yShip[i % 3], xShip[(i+1) % 3], yShip[(i + 1) % 3]);
			Line(xShip[i % 3], yShip[i % 3], xShip[j % 3], yShip[j % 3]);
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
