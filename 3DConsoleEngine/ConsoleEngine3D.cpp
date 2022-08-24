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
	/*meshCube.tris =
	{
		// South
		{0.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f, 0.0f },
		{0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f },

		// East
		{1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,	1.0f, 1.0f, 1.0f },
		{1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 1.0f },

		// North
		{1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 1.0f },
		{1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f },

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
	*/

//	if(!meshCurrent.LoadObjectFile("bunnyLo.obj"))
		// File could not be loaded

	meshCurrent.LoadObjectFile("bunnyHi.obj");
	//meshCurrent.LoadObjectFile("bunnyLo.obj");

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

	// Store triangles that will be drawn to the screen 
	vector <triangle> trianglesToDraw;

	// Draw the Triangles of meshes
	for (auto tri : meshCurrent.tris)
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
		triTranslated.p[0].z = triRotatedZX.p[0].z + 0.25f;
		triTranslated.p[1].z = triRotatedZX.p[1].z + 0.25f;
		triTranslated.p[2].z = triRotatedZX.p[2].z + 0.25f;

		vec3d normal;
		vec3d lineA;
		vec3d lineB;

		lineA.x = triTranslated.p[1].x - triTranslated.p[0].x;
		lineA.y = triTranslated.p[1].y - triTranslated.p[0].y;
		lineA.z = triTranslated.p[1].z - triTranslated.p[0].z;


		lineB.x = triTranslated.p[2].x - triTranslated.p[0].x;
		lineB.y = triTranslated.p[2].y - triTranslated.p[0].y;
		lineB.z = triTranslated.p[2].z - triTranslated.p[0].z;

		// Get the normal as the cross product of the lines
		normal.x = lineA.y * lineB.z - lineA.z * lineB.y;
		normal.y = lineA.z * lineB.x - lineA.x * lineB.z;
		normal.z = lineA.x * lineB.y - lineA.y * lineB.x;

		// Normalize the normal, to get an unit vector
		float len = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= len;
		normal.y /= len;
		normal.z /= len;

		// World Space to Screen Space, only if the triangle is visible to the camera,
		// This is true, when the dot product of the normal and the camera is < 0.0f
		if(normal.x * (triTranslated.p[0].x - camera.x) +
		   normal.y * (triTranslated.p[0].y - camera.y) +
		   normal.z * (triTranslated.p[0].z - camera.z) < 0.0f)
		{

			// Illuminate using a single directional light
			vec3d lightDir = { 0.0f, 0.0f, -1.0f };
			// Normalize
			float len = sqrtf(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);
			lightDir.x /= len;
			lightDir.y /= len;
			lightDir.z /= len;

			// Dot product, get the similarity between the triangle normal and the light direction
			float dotProduct = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;

			// Use Dot Product to get a gray shading color, as is necessary for the console
			CHAR_INFO col = GetColor(dotProduct);
			triTranslated.color = col.Attributes;
			triTranslated.symbol = col.Char.UnicodeChar;


			// Project from 3D to 2D 
			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProjection);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProjection);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProjection);
			// Remember to copy the color and symbol information as well
			triProjected.color = triTranslated.color;
			triProjected.symbol = triTranslated.symbol;
		
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

			// Store the triangle for sorting, to determine if it will be drawn or not
			trianglesToDraw.push_back(triProjected);

			}
		}

		// Sort triangles from back to front, to determine which triangles to draw
		sort(trianglesToDraw.begin(), trianglesToDraw.end(), [](triangle& t1, triangle& t2)
			{
				// Get midpoint Z value of both triangles, to determine approximately which triangle is closer
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});

		// Draw triangles to be drawn
		for(auto &tri : trianglesToDraw)
		{
			switch (drawMode)
			{
			case 0: // Wireframe
				DrawTriangle(tri.p[0].x, tri.p[0].y,
					tri.p[1].x, tri.p[1].y,
					tri.p[2].x, tri.p[2].y);
				break;
			case 1: // Filled
				FillTriangle(tri.p[0].x, tri.p[0].y,
					tri.p[1].x, tri.p[1].y,
					tri.p[2].x, tri.p[2].y,
					tri.symbol, tri.color);
				break;
			default: // Wireframe by default
				DrawTriangle(tri.p[0].x, tri.p[0].y,
					tri.p[1].x, tri.p[1].y,
					tri.p[2].x, tri.p[2].y);
				break;
		}
	}

	return true;
}
