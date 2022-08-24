#pragma once
// Based on Console 3D Engine by Javidx9
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

#include "../CppConsoleProjects/ConsoleEngine.h" 
#include <fstream>
#include <strstream>
#include <algorithm>

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];

	wchar_t symbol;
	short color;
};

struct mesh
{
	vector<triangle> tris;

	bool LoadObjectFile(string filename)
	{
		// Open filename, if it exists
		ifstream f(filename);
		if (!f.is_open())
			return false;

		// Temporal vertice pool
		vector<vec3d> vertices;

		while (!f.eof())
		{
			// Assume for now that line length doesn't exceed 128 characters
			char line[128];
			f.getline(line, 128);

			// Turn the line into a string stream
			strstream s;
			s << line;

			char j;

			if (line[0] == 'v')
			{
				vec3d v;
				// Read vertice values from stream 
				s >> j >> v.x >> v.y >> v.z;
				vertices.push_back(v);
			}
			if (line[0] == 'f')
			{
				// Create faces and push them as triangles to the tris vector
				// Vertices are fetched from the temporary vertice pool, using the index given in the object file
				// Indexing in the obj file starts from 1, so subtract the index by 1
				int f[3];
				s >> j >> f[0] >> f[1] >> f[2];
				tris.push_back({vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1] });
			}
		}

		return true;
	}
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class ConsoleEngine3D : public ConsoleEngine
{
public:
	ConsoleEngine3D()
	{
		appName = L"Console Engine 3D";
	}

	// 0 == Wireframe, 1 == Filled 
	int drawMode = 1;

private:
	mesh meshCube;
	mesh meshCurrent;
	float rotationTheta = 0.0f;
	mat4x4 matProjection;

	// Represents the camera's position for now
	vec3d camera;

	void MultiplyMatrixVector(vec3d& input, vec3d& output, mat4x4& matrix)
	{
		output.x = input.x * matrix.m[0][0] + input.y * matrix.m[1][0] + input.z * matrix.m[2][0] + matrix.m[3][0];
		output.y = input.x * matrix.m[0][1] + input.y * matrix.m[1][1] + input.z * matrix.m[2][1] + matrix.m[3][1];
		output.z = input.x * matrix.m[0][2] + input.y * matrix.m[1][2] + input.z * matrix.m[2][2] + matrix.m[3][2];
		float w = input.x * matrix.m[0][3] + input.y * matrix.m[1][3] + input.z * matrix.m[2][3] + matrix.m[3][3];

		if (w != 0.0f)
		{
			output.x /= w;
			output.y /= w;
			output.z /= w;
		}
	}

	CHAR_INFO GetColor(float luminance)
	{
		// Choose color and symbol based on luminance, to create a shading effect
		short bg_col;
		short fg_col;
		wchar_t symbol;

		// Turn luminance (0-1) to int between (0 - 12)
		int pixel_bw = (int)(13.0f * luminance);

		switch (pixel_bw)
		{
			case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; symbol = PIXEL_FULL; break;

			case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; symbol = PIXEL_QUARTER; break;
			case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; symbol = PIXEL_HALF; break;
			case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; symbol = PIXEL_THREEQUARTERS; break;
			case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; symbol = PIXEL_FULL; break;

			case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; symbol = PIXEL_QUARTER; break;
			case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; symbol = PIXEL_HALF; break;
			case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; symbol = PIXEL_THREEQUARTERS; break;
			case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; symbol = PIXEL_FULL; break;

			case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; symbol = PIXEL_QUARTER; break;
			case 10: bg_col = BG_GREY; fg_col = FG_WHITE; symbol = PIXEL_HALF; break;
			case 11: bg_col = BG_GREY; fg_col = FG_WHITE; symbol = PIXEL_THREEQUARTERS; break;
			case 12: bg_col = BG_GREY; fg_col = FG_WHITE; symbol = PIXEL_FULL; break;
			default:
				bg_col = BG_BLACK; fg_col = FG_BLACK; symbol = PIXEL_FULL;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = symbol;
		return c;
	 }

protected:
	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};
