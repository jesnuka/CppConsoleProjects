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

	depthBuffer = new float[screenWidth * screenHeight];

	meshCube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,1.0f,    0.0f, 1.0f, 0.0f,1.0f,    1.0f, 1.0f, 0.0f,1.0f,	0.0f, 1.0f, 1.0f,		0.0f, 0.0f,1.0f,		1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f,1.0f,    1.0f, 1.0f, 0.0f,1.0f,    1.0f, 0.0f, 0.0f,1.0f,	0.0f, 1.0f, 1.0f,		1.0f, 0.0f,1.0f,		1.0f, 1.0f, 1.0f },
						   												 		  				   
		// EAST                                                     	 		  				    
		{ 1.0f, 0.0f, 0.0f,1.0f,    1.0f, 1.0f, 0.0f,1.0f,    1.0f, 1.0f, 1.0f,1.0f,	0.0f, 1.0f, 1.0f,		0.0f, 0.0f,1.0f,		1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,1.0f,    1.0f, 1.0f, 1.0f,1.0f,    1.0f, 0.0f, 1.0f,1.0f,	0.0f, 1.0f, 1.0f,		1.0f, 0.0f,1.0f,		1.0f, 1.0f, 1.0f },
						   																		
		// NORTH                                   												   
		{ 1.0f, 0.0f, 1.0f,1.0f,    1.0f, 1.0f, 1.0f,1.0f,    0.0f, 1.0f, 1.0f,1.0f,	0.0f, 1.0f, 1.0f,		0.0f, 0.0f,1.0f,		1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,1.0f,    0.0f, 1.0f, 1.0f,1.0f,    0.0f, 0.0f, 1.0f,1.0f,	0.0f, 1.0f, 1.0f,		1.0f, 0.0f,1.0f,		1.0f, 1.0f, 1.0f },
						   																		  
		// WEST                                    								 				  
		{ 0.0f, 0.0f, 1.0f,1.0f,    0.0f, 1.0f, 1.0f,1.0f,    0.0f, 1.0f, 0.0f,1.0f,	0.0f, 1.0f, 1.0f,		0.0f, 0.0f,1.0f,		1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f,1.0f,    0.0f, 1.0f, 0.0f,1.0f,    0.0f, 0.0f, 0.0f,1.0f,	0.0f, 1.0f, 1.0f,		1.0f, 0.0f,1.0f,		1.0f, 1.0f, 1.0f },
																				   				   
		// TOP            														 				 
		{ 0.0f, 1.0f, 0.0f,1.0f,    0.0f, 1.0f, 1.0f,1.0f,    1.0f, 1.0f, 1.0f,1.0f,	0.0f, 1.0f, 1.0f,		0.0f, 0.0f,1.0f,		1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,1.0f,    1.0f, 1.0f, 1.0f,1.0,    1.0f, 1.0f, 0.0f,1.0f ,	0.0f, 1.0f, 1.0f,		1.0f, 0.0f,1.0f,		1.0f, 1.0f, 1.0f },
						 														 				   
		// BOTTOM       														 				 
		{ 1.0f, 0.0f, 1.0f,1.0f,    0.0f, 0.0f, 1.0f,1.0f,    0.0f, 0.0f, 0.0f,1.0f,	0.0f, 1.0f, 1.0f,		0.0f, 0.0f,1.0f,		1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,1.0f,    0.0f, 0.0f, 0.0f,1.0f,    1.0f, 0.0f, 0.0f,1.0f,	0.0f, 1.0f, 1.0f,		1.0f, 0.0f,1.0f,		1.0f, 1.0f, 1.0f },

	};

	// Pre-load the sprite
	spriteTexture1 = new ConsoleSprite(L"wormStand.spr");

//	if(!meshCurrent.LoadObjectFile("teapot.obj"))
		// File could not be loaded

	//meshCurrent.LoadObjectFile("teapot.obj");
	meshCurrent = meshCube;

	// Create the Projection Matrix using the utility function
	matProjection = MatrixMakeProjection(90.0f, (float)screenHeight / (float)screenWidth, 0.1f, 1000.0f);
	
	return true;
}

bool ConsoleEngine3D::OnUserUpdate(float elapsedTime) 
{
	// User Input

	// Camera movement
	if (keys[VK_UP].isHeld)
		camera.y += 8.0f * elapsedTime;
	if (keys[VK_DOWN].isHeld)
		camera.y -= 8.0f * elapsedTime;
	if (keys[VK_LEFT].isHeld)
		camera.x -= 8.0f * elapsedTime;
	if (keys[VK_RIGHT].isHeld)
		camera.x += 8.0f * elapsedTime;

	// Temporary forward vector for moving forward and backward
	vec3d forwardTemp = VectorMultiply(lookDirection, 8.0f * elapsedTime);

	// Move Camera forward and backward
	if (keys[L'W'].isHeld) 
		camera = VectorAdd(camera, forwardTemp);
	if (keys[L'S'].isHeld)
		camera = VectorSubtract(camera, forwardTemp);

	// Camera rotation
	if (keys[L'A'].isHeld)
		yaw += 2.0f * elapsedTime;
	if (keys[L'D'].isHeld)
		yaw -= 2.0f * elapsedTime;

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


	// Camera 
	
	// Initialize lookDirection to be along the z-axis at first
	vec3d vectorUp = { 0, 1, 0 };
	vec3d vectorTarget = { 0,0,1 };
	// Rotate camera using a rotation matrix created using the yaw value, first along the origin
	mat4x4 matrixCameraRotation = MatrixMakeRotationY(yaw);
	lookDirection = VectorMultiplyMatrix(vectorTarget, matrixCameraRotation);
	// Offset to the camera location
	vectorTarget = VectorAdd(camera, lookDirection);

	// Create the camera transformation matrix, which places and orients the camera in world space
	mat4x4 matrixCamera = MatrixPointAt(camera, vectorTarget, vectorUp);

	// Make a view matrix from the camera matrix, which is inverse of the camera's transformation matrix
	mat4x4 matrixView = MatrixQuickInverse(matrixCamera);

	// Store triangles that will be drawn to the screen for later sorting
	vector <triangle> trianglesToDraw;

	// Draw the Triangles of meshes
	for (auto tri : meshCurrent.tris)
	{
		
		triangle triProjected;
		// Transformed triangle, which has worldMatrix applied
		triangle triTransformed;
		triangle triViewed;

		// Transform the object using the World Matrix
		triTransformed.p[0] = VectorMultiplyMatrix(tri.p[0], matWorld);
		triTransformed.p[1] = VectorMultiplyMatrix(tri.p[1], matWorld);
		triTransformed.p[2] = VectorMultiplyMatrix(tri.p[2], matWorld);
		// Also copy the texture coordinate information over to the new triangles
		triTransformed.t[0] = tri.t[0];
		triTransformed.t[1] = tri.t[1];
		triTransformed.t[2] = tri.t[2];

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

			// Convert from World Space to View Space
			triViewed.p[0] = VectorMultiplyMatrix(triTransformed.p[0], matrixView);
			triViewed.p[1] = VectorMultiplyMatrix(triTransformed.p[1], matrixView);
			triViewed.p[2] = VectorMultiplyMatrix(triTransformed.p[2], matrixView);
			// Remember to copy the color and symbol information as well
			triViewed.color = triTransformed.color;
			triViewed.symbol = triTransformed.symbol;
			// Also copy the texture coordinate information over to the new triangles
			triViewed.t[0] = triTransformed.t[0];
			triViewed.t[1] = triTransformed.t[1];
			triViewed.t[2] = triTransformed.t[2];

			// Clip Viewed Triangle against the near plane
			int clippedTriangles = 0;
			triangle clipped[2];
			clippedTriangles = TriangleClipAgainstPlane({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, triViewed, clipped[0], clipped[1]);

			for (int n = 0; n < clippedTriangles; n++)
			{

				// Project from 3D to 2D 
				triProjected.p[0] = VectorMultiplyMatrix(clipped[n].p[0], matProjection);
				triProjected.p[1] = VectorMultiplyMatrix(clipped[n].p[1], matProjection);
				triProjected.p[2] = VectorMultiplyMatrix(clipped[n].p[2], matProjection);
				triViewed.color = clipped[n].color;
				triViewed.symbol = clipped[n].symbol;
				// Also copy the texture coordinate information over to the new triangles
				triProjected.t[0] = clipped[n].t[0];
				triProjected.t[1] = clipped[n].t[1];
				triProjected.t[2] = clipped[n].t[2];

				// Use the vertex vector W component to transform texture coordinates
				triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
				triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
				triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

				triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
				triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
				triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

				triProjected.t[0].w = 1.0f / triProjected.p[0].w;
				triProjected.t[1].w = 1.0f / triProjected.p[1].w;
				triProjected.t[2].w = 1.0f / triProjected.p[2].w;

				// Normalize the coordinates
				triProjected.p[0] = VectorDivide(triProjected.p[0], triProjected.p[0].w);
				triProjected.p[1] = VectorDivide(triProjected.p[1], triProjected.p[1].w);
				triProjected.p[2] = VectorDivide(triProjected.p[2], triProjected.p[2].w);


				triProjected.p[0].y *= -1.0f;
				triProjected.p[1].y *= -1.0f;
				triProjected.p[2].y *= -1.0f;

		
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
	}

		// Sorting is not needed if no transparency is used
		// Sort triangles from back to front, to determine which triangles to draw
	/*	sort(trianglesToDraw.begin(), trianglesToDraw.end(), [](triangle& t1, triangle& t2)
			{
				// Get midpoint Z value of both triangles, to determine approximately which triangle is closer
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 > z2;
			});*/

		// Empty the screen
		Fill(0, 0, screenWidth, screenHeight, L' ', 0);

		// Clear the Depth Buffer
		for (int i = 0; i < screenWidth * screenHeight; i++)
			depthBuffer[i] = 0.0f;

		// Rasterize triangles
		for (auto& tri : trianglesToDraw)
		{
			// First clip triangles against all four edges of the screen
			// 
			triangle clipped[2];
			list<triangle> listTriangles;
			listTriangles.push_back(tri);
			int newTriangles = 1;

			// Test against all 4 planes
			// Once triangles are tested, they are guaranteed to lie inside of the plane
			for (int p = 0; p < 4; p++)
			{
				int trisToAdd = 0;
				while (newTriangles > 0)
				{
					triangle triTest = listTriangles.front();
					listTriangles.pop_front();
					newTriangles--;

					switch (p)
					{
					case 0: // Top of the screen
						trisToAdd = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, triTest, clipped[0], clipped[1]); 
						break;
					case 1: // Bottom of the screen
						trisToAdd = TriangleClipAgainstPlane({ 0.0f, (float)screenHeight - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, triTest, clipped[0], clipped[1]); 
						break;
					case 2: // Screen left side
						trisToAdd = TriangleClipAgainstPlane({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, triTest, clipped[0], clipped[1]);
						break;
					case 3: // Screen right side
						trisToAdd = TriangleClipAgainstPlane({(float)screenWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, triTest, clipped[0], clipped[1]);
						break;
					}

					for (int w = 0; w < trisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				newTriangles = listTriangles.size();
			}

			// Finally, the triangles can be drawn
			for (auto& tri : listTriangles)
			{
				TexturedTriangle(tri.p[0].x, tri.p[0].y, tri.t[0].u, tri.t[0].v, tri.t[0].w,
							 	 tri.p[1].x, tri.p[1].y, tri.t[1].u, tri.t[1].v, tri.t[1].w,
								 tri.p[2].x, tri.p[2].y, tri.t[2].u, tri.t[2].v, tri.t[2].w,
									spriteTexture1);

				DrawTriangle(tri.p[0].x, tri.p[0].y,
					tri.p[1].x, tri.p[1].y,
					tri.p[2].x, tri.p[2].y, PIXEL_FULL, FG_WHITE);
			/*	switch (drawMode)
				{
				case 0: // Wireframe
					DrawTriangle(tri.p[0].x, tri.p[0].y,
						tri.p[1].x, tri.p[1].y,
						tri.p[2].x, tri.p[2].y, PIXEL_FULL, FG_WHITE);
					break;
				case 1: // Filled
					FillTriangle(tri.p[0].x, tri.p[0].y,
						tri.p[1].x, tri.p[1].y,
						tri.p[2].x, tri.p[2].y,
						tri.symbol, tri.color);
					break;

				case 2: // Filled with wireframe
					FillTriangle(tri.p[0].x, tri.p[0].y,
						tri.p[1].x, tri.p[1].y,
						tri.p[2].x, tri.p[2].y,
						tri.symbol, tri.color);

					DrawTriangle(tri.p[0].x, tri.p[0].y,
						tri.p[1].x, tri.p[1].y,
						tri.p[2].x, tri.p[2].y);
					break;
				default: // Wireframe by default
					DrawTriangle(tri.p[0].x, tri.p[0].y,
						tri.p[1].x, tri.p[1].y,
						tri.p[2].x, tri.p[2].y);
					break;
				}*/
			}

		}

	return true;
}
