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

#include "ConsoleEngine3D.h"

bool ConsoleEngine3D::OnUserCreate()
{
	// Create Cube Mesh, with Unit length sides
	// Triangles are built by going clockwise for the vertices

	meshCube.tris =
	{
		// South
		{0.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f },
		{0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f },

		// East
		{1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f },
		{1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 1.0f },

		// North
		{1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f },
		{1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f },

		// West
		{0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f },
		{0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f },

		// Top
		{0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f },
		{0.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 0.0f },

		// Bottom
		{1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 0.0f },
		{1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f }
	};

	// Projection Matrix is populated, as screen dimensions and FoV are not going to change for now
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	float fov = 90.0f;
	float aspectRatio = (float)screenHeight / (float)screenWidth;
	// Tangent calculation of the projection, converted from degrees to radians
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * M_PI);

	// Create the Projection Matrix
	matProjection.m[0][0] = aspectRatio * fovRad;
	matProjection.m[1][1] = fovRad;
	matProjection.m[2][2] = farPlane / (farPlane - nearPlane);
	matProjection.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	matProjection.m[2][3] = 1.0f;
	matProjection.m[3][3] = 0.0f;



	return true;
}

bool ConsoleEngine3D::OnUserUpdate(float elapsedTime) 
{
	// Empty the screen
	Fill(0, 0, screenHeight, screenWidth);

	// Draw the Triangles of meshes
	for (auto tri : meshCube.tris)
	{
		triangle triProj;
	}

	return true;
}
