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
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);

	// Rotate cube over time
	mat4x4 matRotationZ;
	mat4x4 matRotationX;
	rotationTheta += 1.0f * elapsedTime;

	// Rotation matrix for the Z axis
	matRotationZ.m[0][0] = cosf(rotationTheta);
	matRotationZ.m[0][1] = sinf(rotationTheta);
	matRotationZ.m[1][0] = -sinf(rotationTheta);
	matRotationZ.m[1][1] = cosf(rotationTheta);
	matRotationZ.m[2][2] = 1;
	matRotationZ.m[3][3] = 1;

	// Rotation matrix for the X axis
	matRotationX.m[0][0] = 1;
	matRotationX.m[1][1] = cosf(rotationTheta * 0.5f);
	matRotationX.m[1][2] = sinf(rotationTheta * 0.5f);
	matRotationX.m[2][1] = -sinf(rotationTheta * 0.5f);
	matRotationX.m[2][2] = cosf(rotationTheta * 0.5f);
	matRotationX.m[3][3] = 1;

	// Draw the Triangles of meshes
	for (auto tri : meshCube.tris)
	{
		
		triangle triProjected;
		// Translated triangle, offset from the screen
		triangle triTranslated;

		// Used for rotating the triangle in different speeds
		triangle triRotatedZ;
		triangle triRotatedZX;

		// Rotate the triangle
		MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotationZ);
		MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotationZ);
		MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotationZ);

		MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotationX);
		MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotationX);
		MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotationX);


		// Translate the object in the Z plane, offsetting them to the screen
		triTranslated = triRotatedZX;
		triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
		triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
		triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

		// Project from 3D to 2D
		MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProjection);
		MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProjection);
		MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProjection);

		// Scale the vertices to view

		// Shift coordinates between 0 - 2
		triProjected.p[0].x += 1.0f;
		triProjected.p[1].x += 1.0f;
		triProjected.p[2].x += 1.0f;
			   
		triProjected.p[0].y += 1.0f;
		triProjected.p[1].y += 1.0f;
		triProjected.p[2].y += 1.0f;

		// Divide by 2, scale to the correct axis size
		triProjected.p[0].x *= 0.5f * (float)screenWidth;
		triProjected.p[1].x *= 0.5f * (float)screenWidth;
		triProjected.p[2].x *= 0.5f * (float)screenWidth;
			   
		triProjected.p[0].y *= 0.5f * (float)screenHeight;
		triProjected.p[1].y *= 0.5f * (float)screenHeight;
		triProjected.p[2].y *= 0.5f * (float)screenHeight;


		DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
			triProjected.p[1].x, triProjected.p[1].y,
			triProjected.p[2].x, triProjected.p[2].y);
	}

	return true;
}
