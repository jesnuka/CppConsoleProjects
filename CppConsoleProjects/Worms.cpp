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

void Worms::PerlinNoise1D(int count, float* seed, int octaves, float bias, float* output)
{
	// 1D Perlin Noise
	for (int x = 0; x < count; x++)
	{
		float noise = 0.0f;
		float scaleAcc = 0.0f;
		float scale = 1.0f;

		for (int o = 0; o < octaves; o++)
		{
			int pitch = count >> o;
			int sample1 = (x / pitch) * pitch;
			int sample2 = (sample1 + pitch) % count;

			float blend = (float)(x - sample1) / (float)pitch;

			float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

			scaleAcc += scale;
			noise += sample * scale;
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
				Draw(i, j, PIXEL_FULL, FG_GREEN);
				break;
			default:
				Draw(i, j, PIXEL_FULL, FG_CYAN);
				break;
			}
		}
	}

    return true;
}
