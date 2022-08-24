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

//	if(!meshCurrent.LoadObjectFile("teapot.obj"))
		// File could not be loaded

	meshCurrent.LoadObjectFile("teapot.obj");

	// Create the Projection Matrix using the utility function
	matProjection = MatrixMakeProjection(90.0f, (float)screenHeight / (float)screenWidth, 0.1f, 1000.0f);
	
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

	matRotationZ = MatrixMakeRotationZ(rotationTheta * 0.5f);
	matRotationX = MatrixMakeRotationX(rotationTheta);

	// Create the translation matrix
	mat4x4 matTranslation;
	matTranslation = MatrixMakeTranslation(0.0f, 0.0f, 5.0f);

	// Create the world matrix, which is first rotated, then translated
	mat4x4 matWorld;
	matWorld = MatrixMakeIdentity();
	matWorld = MatrixMultiplyMatrix(matRotationZ, matRotationX);
	matWorld = MatrixMultiplyMatrix(matWorld, matTranslation);

	// Store triangles that will be drawn to the screen for later sorting
	vector <triangle> trianglesToDraw;

	// Draw the Triangles of meshes
	for (auto tri : meshCurrent.tris)
	{
		
		triangle triProjected;
		// Transformed triangle, which has worldMatrix applied
		triangle triTransformed;

		triTransformed.p[0] = VectorMultiplyMatrix(tri.p[0], matWorld);
		triTransformed.p[1] = VectorMultiplyMatrix(tri.p[1], matWorld);
		triTransformed.p[2] = VectorMultiplyMatrix(tri.p[2], matWorld);

		vec3d normal;
		vec3d line1;
		vec3d line2;

		//Get the lines of the triangle
		line1 = VectorSubtract(triTransformed.p[1], triTransformed.p[0]);
		line2 = VectorSubtract(triTransformed.p[2], triTransformed.p[0]);

		// Cross product of lines to get triangle surface normal, and normalize the result
		normal = VectorCrossProduct(line1, line2);
		normal = VectorNormalize(normal);

		// Normalize the normal, to get an unit vector
		float len = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= len;
		normal.y /= len;
		normal.z /= len;

		// Get a ray from triangle to camera
		vec3d centroid = triTransformed.Centroid();
		vec3d cameraRay = VectorSubtract(centroid, camera);

		// Check if ray is aligned with the normal. If it is, the triangle will be visible
		if(VectorDotProduct(normal, cameraRay) < 0.0f)
		{

			// Illuminate using a single directional light
			vec3d lightDirection = { 0.0f, 0.0f, -1.0f };
			lightDirection = VectorNormalize(lightDirection);

			// Dot product, get the similarity between the triangle normal and the light direction
			// Clamp to make positive
			float dotProduct = max(0.1f, VectorDotProduct(lightDirection, normal));

			// Use Dot Product to get a gray shading color, as is necessary for the console
			CHAR_INFO col = GetColor(dotProduct);
			triTransformed.color = col.Attributes;
			triTransformed.symbol = col.Char.UnicodeChar;


			// Project from 3D to 2D 
			triProjected.p[0] = VectorMultiplyMatrix(triTransformed.p[0], matProjection);
			triProjected.p[1] = VectorMultiplyMatrix(triTransformed.p[1], matProjection);
			triProjected.p[2] = VectorMultiplyMatrix(triTransformed.p[2], matProjection);
			// Remember to copy the color and symbol information as well
			triProjected.color = triTransformed.color;
			triProjected.symbol = triTransformed.symbol;

			// Normalize the coordinates
			triProjected.p[0] = VectorDivide(triProjected.p[0], triProjected.p[0].w);
			triProjected.p[1] = VectorDivide(triProjected.p[1], triProjected.p[1].w);
			triProjected.p[2] = VectorDivide(triProjected.p[2], triProjected.p[2].w);
		
			// Offset the vertices to visible normalized space
			vec3d offsetView = { 1.0f, 1.0f, 0.0f};

			triProjected.p[0] = VectorAdd(triProjected.p[0], offsetView);
			triProjected.p[1] = VectorAdd(triProjected.p[1], offsetView);
			triProjected.p[2] = VectorAdd(triProjected.p[2], offsetView);

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
