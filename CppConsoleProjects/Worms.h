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

class PhysicsObject
{
public:
	float posX = 0.0f;
	float posY = 0.0f;
	float velocityX = 0.0f;
	float velocityY = 0.0f;
	float accelX = 0.0f;
	float accelY = 0.0f;
	float friction = 0.8f;

	float radius = 4.0f;
	// Stable objects do not move, and don't need to have more calculations
	bool stable = false;

	PhysicsObject(float x = 0.0f, float y = 0.0f)
	{
		posX = x;
		posY = y;
	}

	virtual void Draw(ConsoleEngine *engine, float offsetX, float offsetY) = 0;
};

class Debris : public PhysicsObject
{
public:
	Debris(float x = 0.0f, float y = 0.0f) : PhysicsObject(x, y)
	{
		// Have initial velocity for debris in a circular direction
		velocityX = 10.0f * cosf((float)rand() / (float)RAND_MAX * 2.0F * M_PI);
		velocityY = 10.0f * sinf((float)rand() / (float)RAND_MAX * 2.0F * M_PI);
		radius = 1.0f;
		friction = 0.8f;
	}

	virtual void Draw(ConsoleEngine* engine, float offsetX, float offsetY)
	{
		// Draw wireframe model
		// Position is offset by camera, atan2f to get the angle using velocities, Scale is done using radius to see the bounding radius
		engine->WireFrame(model, posX - offsetX, posY - offsetY, atan2f(velocityY, velocityX), radius, PIXEL_FULL, FG_DARK_GREEN);
	}

private:
	// Share model across models of the same class
	static vector<pair<float, float>> model;
};

class Dummy : public PhysicsObject
{
public:
	Dummy(float x= 0.0f, float y = 0.0f) : PhysicsObject(x,y)
	{

	}

	virtual void Draw(ConsoleEngine *engine, float offsetX, float offsetY)
	{
		// Draw wireframe model of dummy, 
		// Position is offset by camera, atan2f to get the angle using velocities, Scale is done using radius to see the bounding radius
		engine->WireFrame(model, posX - offsetX, posY - offsetY, atan2f(velocityY, velocityX),radius, PIXEL_FULL, FG_WHITE);
	}

private:
	// Share model across models of the same class
	static vector<pair<float, float>> model;
};

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

	// List of all the physics objects, as pointers because they are abstract
	// Subclasses can therefore also be added to this list
	list<PhysicsObject*> objects;

	unsigned char* map = nullptr;

	void CreateMap();
	void PerlinNoise1D(int nCount, float* fSeed, int nOctaves, float fBias, float* fOutput);

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);
};