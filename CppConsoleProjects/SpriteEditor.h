#pragma once

// Based on the Console Sprite Editor by @Javidx9 at OneLoneCoder.com 

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

#define _USE_MATH_DEFINES
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <chrono>
#include <vector>
#include <utility>
#include <algorithm>
#include <condition_variable>
#include <list>

#include "ConsoleEngine.h"

using namespace std;

class SpriteEditor : public ConsoleEngine
{
public: 
	SpriteEditor()
	{
		appName = L"Sprite Editor";
	}

private:
	int posX = 0;
	int posY = 0;
	int offsetX = 0;
	int offsetY = 0;

	int zoom = 4;

	int currentSymbol = PIXEL_FULL;
	int currentColourFG = FG_RED;
	int currentColourBG = FG_BLACK;

	ConsoleSprite *sprite = nullptr;
	wstring spriteFile;

protected:

	virtual bool OnUserCreate()
	{
		// Create sprite
		sprite = new ConsoleSprite(8, 8);
		spriteFile = L"worm.spr";

		return true;
	}

	virtual bool OnUserUpdate(float elapsedTime)
	{
		// Zooming Input
		if (keys[VK_PRIOR].isReleased)
			zoom <<= 1;

		if (keys[VK_NEXT].isReleased)
			zoom >>= 1;

		if (zoom > 32) 
			zoom = 32;

		if (zoom < 2) 
			zoom = 2;

		// Brushes Input
		if (keys[VK_F1].isReleased) currentSymbol = PIXEL_FULL;
		if (keys[VK_F2].isReleased) currentSymbol = PIXEL_THREEQUARTERS;
		if (keys[VK_F3].isReleased) currentSymbol = PIXEL_HALF;
		if (keys[VK_F4].isReleased) currentSymbol = PIXEL_QUARTER;

		// Colours
		for (int i = 0; i < 8; i++)
		{
			if (keys[L"01234567"[i]].isReleased)
			{
				if (keys[VK_SHIFT].isHeld)
					currentColourFG = i + 8;
				else
					currentColourFG = i;
			}
		}

		if (keys[VK_F7].isReleased)
			currentColourBG -= 1;

		if (keys[VK_F8].isReleased)
			currentColourBG += 1;

		if (currentColourBG < 0) 
			currentColourBG = 15;

		if (currentColourBG > 15) 
			currentColourBG = 0;

		if (keys[VK_SHIFT].isHeld)
		{
			if (keys[VK_UP].isReleased) 
				offsetY++;
			if (keys[VK_DOWN].isReleased) 
				offsetY--;
			if (keys[VK_LEFT].isReleased) 
				offsetX++;
			if (keys[VK_RIGHT].isReleased) 
				offsetX--;
		}
		else
		{
			if (keys[VK_UP].isReleased) 
				posY--;
			if (keys[VK_DOWN].isReleased) 
				posY++;
			if (keys[VK_LEFT].isReleased) 
				posX--;
			if (keys[VK_RIGHT].isReleased) 
				posX++;
		}

		if (sprite != nullptr)
		{
			if (posX < 0) posX = 0;
			if (posX >= sprite->sWidth) posX = sprite->sWidth - 1;
			if (posY < 0) posY = 0;
			if (posY >= sprite->sHeight) posY = sprite->sHeight - 1;

			if (keys[VK_SPACE].isReleased)
			{
				sprite->SetSymbol(posX - 0, posY - 0, currentSymbol);
				sprite->SetColour(posX - 0, posY - 0, currentColourFG | (currentColourBG << 4));
			}

			if (keys[VK_DELETE].isReleased)
			{
				sprite->SetSymbol(posX - 0, posY - 0, L' ');
				sprite->SetColour(posX - 0, posY - 0, 0);
			}

			if (keys[VK_F9].isReleased)
			{
				sprite->Load(spriteFile);
			}

			if (keys[VK_F10].isReleased)
			{
				sprite->Save(spriteFile);
			}
		}

		// Empty Screen
		Fill(0, 0, screenWidth, screenHeight, L' ', 0);

		// Draw the menu with tools and controls
		DrawString(1, 1, L"F1 = 100%  F2 = 75%  F3 = 50%  F4 = 25%    F9 = Load File  F10 = Save File");
		for (int i = 0; i < 8; i++)
		{
			DrawString(1 + 6 * i, 3, to_wstring(i) + L" = ");
			if (keys[VK_SHIFT].isHeld)
				Draw(1 + 6 * i + 4, 3, PIXEL_FULL, (i + 8));
			else
				Draw(1 + 6 * i + 4, 3, PIXEL_FULL, (i));
		}

		DrawString(1, 5, L"Current Brush = ");
		Draw(18, 5, currentSymbol, currentColourFG | (currentColourBG << 4));

		// Draw the Canvas
		for (int x = 9; x < 138; x++)
		{
			Draw(x, 9);
			Draw(x, 74);
		}

		for (int y = 9; y < 75; y++)
		{
			Draw(9, y);
			Draw(138, y);
		}

		// Draw the visible Sprite
		if (sprite != nullptr)
		{
			int nVisiblePixelsX = 128 / zoom;
			int nVisiblePixelsY = 64 / zoom;

			for (int x = 0; x < nVisiblePixelsX; x++)
				for (int y = 0; y < nVisiblePixelsY; y++)
				{
					if (x - offsetX < sprite->sWidth && y - offsetY < sprite->sHeight && x - offsetX >= 0 && y - offsetY >= 0)
					{
						// Draw Sprite
						Fill(x * zoom + 10, y * zoom + 10, (x + 1) * zoom + 10, (y + 1) * zoom + 10, sprite->GetSymbol(x - offsetX, y - offsetY), sprite->GetColour(x - offsetX, y - offsetY));


						// Draw Pixel Markers
						if (sprite->GetSymbol(x - offsetX, y - offsetY) == L' ')
							Draw((x)*zoom + 10, (y)*zoom + 10, L'.');
					}

					if (x - offsetX == posX && y - offsetY == posY)
						Draw((x)*zoom + 10, (y)*zoom + 10, L'O');
				}
		}

		// Draw the actual sprite
		for (int x = 0; x < sprite->sWidth; x++)
		{
			for (int y = 0; y < sprite->sHeight; y++)
				Draw(x + 10, y + 80, sprite->GetSymbol(x, y), sprite->GetColour(x, y));
		}

		return true;
	}
};