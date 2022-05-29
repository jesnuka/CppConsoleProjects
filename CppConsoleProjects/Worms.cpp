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

void Worms::CreateMap(int mapWidth, int mapHeight, int mapMode)
{

}

bool Worms::OnUserCreate()
{
	// Create and initialize the map
	map = new unsigned char[mapWidth * mapHeight];
	memset(map, 0, mapWidth * mapHeight * sizeof(unsigned char));

    return true;
}

bool Worms::OnUserUpdate(float elapsedTime)
{
    return true;
}
