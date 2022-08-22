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

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	vector<triangle> tris;
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

private:
	mesh meshCube;
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

protected:
	virtual bool OnUserCreate();

	virtual bool OnUserUpdate(float elapsedTime);

};
