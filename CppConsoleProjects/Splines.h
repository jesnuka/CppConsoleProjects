#pragma once

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


#include "ConsoleEngine.h"
using namespace std;


struct Point2
{
	float x;
	float y;
	float length;
};

struct Spline // Catmull-Rom Spline
{
	vector<Point2> points;
	float totalSplineLength;

	Point2 GetPoint(float t, bool loop = false)
	{
		int p1, p2, p3, p0;

		if (!loop) // Non-looped spline
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p1 + 2;
			p0 = p1 - 1;
		}
		else // Looped around spline, where end goes back around to the beginning
		{
			p1 = (int)t;
			p2 = (p1 + 1) % points.size();
			p3 = (p1 + 2) % points.size();
			p0 = p1 >= 1 ? (p1 - 1) : points.size() - 1;
		}

		// Get leftover of floor(t)
		t = t - (int)t;

		float t2 = t * t;
		float t3 = t2 * t;

		float y1 = -t3 + 2.0f * t2 - t;
		float y2 = 3.0f * t3 - 5.0f * t2 + 2.0f;
		float y3 = -3.0f * t3 + 4.0f * t2 + t;
		float y4 = t3 - t2;

		float resX = points[p0].x * y1 + 
					 points[p1].x * y2 + 
					 points[p2].x * y3 + 
					 points[p3].x * y4;

		float resY = points[p0].y * y1 +
					 points[p1].y * y2 +
					 points[p2].y * y3 +
					 points[p3].y * y4;

		resX *= 0.5f;
		resY *= 0.5f;

		return {resX, resY};
	}

	// Allows getting the gradient of the position in the spline
	Point2 GetGradient(float t, bool loop = false)
	{
		int p1, p2, p3, p0;

		if (!loop) // Non-looped spline
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p1 + 2;
			p0 = p1 - 1;
		}
		else // Looped around spline, where end goes back around to the beginning
		{
			p1 = (int)t;
			p2 = (p1 + 1) % points.size();
			p3 = (p1 + 2) % points.size();
			p0 = p1 >= 1 ? (p1 - 1) : points.size() - 1;
		}

		// Get leftover of floor(t)
		t = t - (int)t;

		float t2 = t * t;
		float t3 = t2 * t;

		float y1 = -3.0f * t2 + 4.0f * t - 1;
		float y2 = 9.0f * t2 - 10.0f * t;
		float y3 = -9.0f * t2 + 8.0f * t + 1;
		float y4 = 3.0f * t2 - 2.0f * t;

		float resX = points[p0].x * y1 +
			points[p1].x * y2 +
			points[p2].x * y3 +
			points[p3].x * y4;

		float resY = points[p0].y * y1 +
			points[p1].y * y2 +
			points[p2].y * y3 +
			points[p3].y * y4;

		resX *= 0.5f;
		resY *= 0.5f;

		return { resX, resY };
	}

	float GetSegmentLength(int node, bool loop = false)
	{
		 // Calculate the length of a spline segment

		float length = 0.0f;
		float stepSize = 0.05f;

		Point2 pOld;
		Point2 pNew;
		pOld = GetPoint((float)node, loop);

		for (float t = 0; t < 1.0f; t += stepSize)
		{
			// Get the precise point on the spline
			pNew = GetPoint((float)node + t, loop);

			// Pythagoras
			length += sqrtf(powf(pNew.x - pOld.x, 2) + powf(pNew.y - pOld.y, 2));
			pOld = pNew;
		}

		return length;
	}

	// Get spline location using x,y position
	float GetSegmentNormalizedOffset(float position)
	{
		// First find the base point in the spline
		int i = 0;
		while (position > points[i].length)
		{
			position -= points[i].length;
			i += 1;
		}

		// Now get the remainder of the length, which is the offset
		return (float)i + (position / points[i].length);
	}
};

class Splines : public ConsoleEngine
{
public:
	Splines()
	{
		appName = L"Splines";
	}
private:
	Spline path;
	int selectedPoint = 0;
	float agentPosition = 0.0f;

	vector<pair<float, float>> modelShip;

protected:

	


	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};