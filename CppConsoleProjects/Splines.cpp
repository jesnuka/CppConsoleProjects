#include "Splines.h"

// Based on Splines by Javidx9
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

bool Splines::OnUserCreate()
{
	// Populate path with points, creating the path
	path.points = { {15, 35}, { 25, 35 }, { 35, 35 }, { 45, 35 },
					{55, 35}, { 65, 35 }, { 75, 35 }, { 85, 35 } };

	return true;
}

bool Splines::OnUserUpdate(float elapsedTime)
{
	// Empty screen
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);

	// Input, change which point is selected
	if (keys[0x0058].isReleased)
	{
		selectedPoint += 1;
		if (selectedPoint >= path.points.size())
			selectedPoint = 0;
	}
	if (keys[0x005A].isReleased)
	{
		selectedPoint -= 1;
		if (selectedPoint < 0)
			selectedPoint = path.points.size() - 1;
	}

	// Move selected point on the screen

	if (keys[VK_LEFT].isHeld)
	{
		path.points[selectedPoint].x -= 20.0f * elapsedTime;
	}
	if (keys[VK_RIGHT].isHeld)
	{
		path.points[selectedPoint].x += 20.0f * elapsedTime;
	}
	if (keys[VK_UP].isHeld)
	{
		path.points[selectedPoint].y -= 20.0f * elapsedTime;
	}
	if (keys[VK_DOWN].isHeld)
	{
		path.points[selectedPoint].y += 20.0f * elapsedTime;
	}

	// Draw the Spline, but don't draw two end control points
	for (float t = 0; t < (float)path.points.size() - 3.0f; t += 0.005f)
	{
		Point2 point = path.GetPoint(t);
		Draw(point.x, point.y);
	}


	// Draw Points
	for (int i = 0; i < path.points.size(); i++)
	{
		Fill(path.points[i].x - 1,path.points[i].y - 1, path.points[i].x + 2, path.points[i].y + 2, PIXEL_FULL, FG_RED);
		DrawString(path.points[i].x, path.points[i].y, to_wstring(i));
	}
	Fill(path.points[selectedPoint].x - 1, path.points[selectedPoint].y - 1, path.points[selectedPoint].x + 2, path.points[selectedPoint].y + 2, PIXEL_FULL, FG_YELLOW);
	DrawString(path.points[selectedPoint].x, path.points[selectedPoint].y, to_wstring(selectedPoint));

	return true;
}
