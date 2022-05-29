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

#include "Worms.h"

vector<pair<float, float>> DefineDummy()
{
	vector<pair<float, float>> model;

	// Produce list of points in an unit circle
	model.push_back({ 0.0f, 0.0f });
	for (int i = 0; i < 10; i++)
		model.push_back({ cosf(i / 9.0f * 2.0f * M_PI), sinf(i / 9.0f * 2.0f * M_PI) });

	return model;
}

vector<pair<float, float>> DefineDebris()
{
	// Draw a wireframe square
	vector<pair<float, float>> model;
	model.push_back({0.0f, 0.0f});
	model.push_back({1.0f, 0.0f});
	model.push_back({1.0f, 1.0f});
	model.push_back({0.0f, 1.0f});

	return model;
}

// Factory function for creation
vector<pair<float, float>> Debris::model = DefineDebris();

// Factory function for creation
vector<pair<float, float>> Dummy::model = DefineDummy();

void Worms::CreateMap()
{
	// Create 1D Perlin Noise
	float *surface = new float[mapWidth];
	float *noiseSeed = new float[mapWidth];

	// Use Rand() to generate noise
	for (int i = 0; i < mapWidth; i++)
		noiseSeed[i] = (float)rand() / (float)RAND_MAX;

	// Start and End of terrain are up the screen, as Perlin Noise has fixed intervals and ranges from 0 to 1
	noiseSeed[0] = 0.5f;
	PerlinNoise1D(mapWidth, noiseSeed, 8, 2.0f, surface);

	// Put values on map based on the surface
	// These values are either 1 or 0, depending on if there is ground or not
	for (int i = 0; i < mapWidth; i++)
	{
		for (int j = 0; j < mapHeight; j++)
		{
			if (j >= surface[i] * mapHeight) 
				map[j * mapWidth + i] = 1;	// Set as land
			else
				map[j * mapWidth + i] = 0;	// Set as sky

		}
	}

	// Delete the two allocated arrays
	delete[] surface;
	delete[] noiseSeed;
}

void Worms::PerlinNoise1D(int count, float *seed, int octaves, float bias, float *output)
{
	// 1D Perlin Noise
	for (int x = 0; x < count; x++)
	{
		float noise = 0.0f;
		float scaleAcc = 0.0f;
		float scale = 1.0f;

		for (int o = 0; o < octaves; o++)
		{
			// Divide pitch by two
			int pitch = count >> o;
			// Get sample point, division and multiplication are because of the loss of information caused by integers
			int sample1 = (x / pitch) * pitch;
			// Loop around, allow for tesselation
			int sample2 = (sample1 + pitch) % count;

			float blend = (float)(x - sample1) / (float)pitch;

			float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

			scaleAcc += scale;
			noise += sample * scale;

			// Scale is divided as the value decreases each octave
			scale = scale / bias;
		}

		// Scale to seed range
		output[x] = noise / scaleAcc;
	}
}

bool Worms::OnUserCreate()
{
	// Create and initialize the map
	map = new unsigned char[mapWidth * mapHeight];
	memset(map, 0, mapWidth * mapHeight * sizeof(unsigned char));

	CreateMap();

    return true;
}

bool Worms::OnUserUpdate(float elapsedTime)
{
	// Debug Input
	if (keys[L'M'].isReleased)
		CreateMap();

	if (mouse[2].isReleased)
	{
		Dummy *dummy = new Dummy(mousePositionX + cameraPosX, mousePositionY + cameraPosY);
		objects.push_back(dummy);
	}
	if (mouse[0].isReleased)
	{
		// Launch debris
		for (int i = 0; i < 20; i++)
			objects.push_back(new Debris(mousePositionX + cameraPosX, mousePositionY + cameraPosY));
	}


	// Sides of the screen allow for moving the camera around the map
	if (mousePositionX < 5) 
		cameraPosX -= mapScrollSpeed * elapsedTime;
	if (mousePositionX > screenWidth - 5) 
		cameraPosX += mapScrollSpeed * elapsedTime;
	if (mousePositionY < 5) 
		cameraPosY -= mapScrollSpeed * elapsedTime;
	if (mousePositionY > screenHeight - 5) 
		cameraPosY += mapScrollSpeed * elapsedTime;

	// Clamp the map boundaries to avoid going of bounds
	if (cameraPosX < 0) 
		cameraPosX = 0;
	if (cameraPosX >= mapWidth - screenWidth) 
		cameraPosX = mapWidth - screenWidth;
	if (cameraPosY < 0) 
		cameraPosY = 0;
	if (cameraPosY >= mapHeight - screenHeight) 
		cameraPosY = mapHeight - screenHeight;

	// Perform 10 loops of physics calculation per update
	for (int i = 0; i < 10; i++)
	{
		// Update physics of physics objects
		for (auto &o : objects)
		{
			// Apply Gravity to object
			//o->accelY += 9.81f;
			o->accelY += 2.0f;

			// Update Velocity in respect to acceleration
			o->velocityX += o->accelX * elapsedTime;
			o->velocityY += o->accelY * elapsedTime;

			// Update position according to the velocity
			// However, use potential variables because collision needs to be tested first
			float potentialPosX = o->posX + o->velocityX * elapsedTime;
			float potentialPosY = o->posY + o->velocityY * elapsedTime;

			// Reset acceleration at the end
			o->accelX = 0.0f;
			o->accelY = 0.0f;
			o->stable = false;

			// Check map collision
			float angle = atan2f(o->velocityY, o->velocityX);
			// Response vector is generated using points that collide with the terrain,
			// to then calculate the direction the physics object continues moving in
			float responseX = 0.0f;
			float responseY = 0.0f;
			bool collision = false;

			// Iterate around a semi-circle of the object, as collision only happens in the other side (99% of the time)
			// Step size could be further calculated, to make sure all points are within unit distance from each other
			for (float r = angle - (M_PI / 2.0f); r < angle + (M_PI / 2.0f); r += M_PI / 8.0f)
			{
				float testPosX = (o->radius) * cosf(r) + potentialPosX;
				float testPosY = (o->radius) * sinf(r) + potentialPosY;

				// Clamp points to map size
				if (testPosX < 0.0f)
					testPosX = 0.0f;
				if (testPosX >= mapWidth)
					testPosX = mapWidth - 1;
				if (testPosY < 0.0f)
					testPosY = 0.0f;
				if (testPosY >= mapHeight)
					testPosY = mapHeight - 1;

				// Check for intersection with terrain in that position
				if (map[(int)testPosY * mapWidth + (int)testPosX] != 0)
				{
					// Accumulate collision points to create escape response to object
					responseX += potentialPosX - testPosX;
					responseY += potentialPosY - testPosY;
					collision = true;
				}
			}

			float velocityMagnitude = sqrtf(powf(o->velocityX, 2) + powf(o->velocityY, 2));
			float responseMagnitude = sqrtf(powf(responseX, 2) + powf(responseY, 2));

			if (collision)
			{
				// Object has technically not YET collided, but potentially will, 
				// but is still stopped beforehand to avoid intersection with terrain
				o->stable = true;

				// Calculate reflection vector of the objects velocity
				// Dot product between velocity vector and the normalized response vector
				float dot = o->velocityX * (responseX / responseMagnitude) + o->velocityY * (responseY / responseMagnitude);
				// Calculate new velocity using the reflection equation "r=d-2(d.n)n"
				// Additionally, calculate in the friction coefficient, loss of energy to dampen the response
				o->velocityX = o->friction * (-2.0f * dot * (responseX / responseMagnitude) + o->velocityX);
				o->velocityY = o->friction * (-2.0f * dot * (responseY / responseMagnitude) + o->velocityY);

			}
			else // With no collision, move object to the potential position
			{
				o->posX = potentialPosX;
				o->posY = potentialPosY;
			}

			// When object barely moves anymore, change it to stable
			if (velocityMagnitude < 0.1f)
				o->stable = true;
		}
	}
	// Empty screen
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);


	// Draw the Landscape
	for (int i = 0; i < screenWidth; i++)
	{
		for (int j = 0; j < screenHeight; j++)
		{
			switch (map[(j + (int)(cameraPosY)) * mapWidth + (i + (int)cameraPosX)])
			{
			case 0:
				Draw(i, j, PIXEL_FULL, FG_CYAN);
				break;
			case 1:
				Draw(i, j, PIXEL_FULL, FG_DARK_GREEN);
				break;
			default:
				Draw(i, j, PIXEL_FULL, FG_CYAN);
				break;
			}
		}
	}

	// Draw all the objects using each objects own Draw method
	for (auto& o : objects)
		o->Draw(this, cameraPosX, cameraPosY);

    return true;
}
