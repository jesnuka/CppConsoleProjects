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
};

struct Spline // Catmull-Rom Spline
{
	vector<Point2> points;

	Point2 GetPoint(float t)
	{
		int p1 = (int)t + 1;
		int p2 = p1 + 1;
		int p3 = p1 + 1;
		int p0 = p1 - 1;

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

protected:

	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};