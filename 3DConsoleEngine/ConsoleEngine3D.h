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

struct vec2d
{
	float u = 0.0f;
	float v = 0.0f;
	float w = 1.0f;
};

struct vec3d
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct triangle
{
	// Points
	vec3d p[3];
	// Texture coordinates
	vec2d t[3];

	wchar_t symbol;
	short color;

	// Get the triangle centroid
	vec3d Centroid()
	{
		vec3d c;
		c.x = (p[0].x + p[1].x + p[2].x) / 3.0f;
		c.y = (p[0].y + p[1].y + p[2].y) / 3.0f;
		c.z = (p[0].z + p[1].z + p[2].z) / 3.0f;
		return c;
	}
};

struct mesh
{
	vector<triangle> tris;

	bool LoadObjectFile(string filename, bool hasTexture = false)
	{
		// Open filename, if it exists
		ifstream f(filename);
		if (!f.is_open())
			return false;

		// Temporal vertice pool
		vector<vec3d> vertices;
		vector<vec2d> textures;

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
				// Line is either Vertex or Texture coordinate
				if (line[1] == 't')
				{
					vec2d v;
					// Read vertice values from stream 
					s >> j >> j >> v.u >> v.v;
					textures.push_back(v);
				}
				else
				{
					vec3d v;
					// Read vertice values from stream 
					s >> j >> v.x >> v.y >> v.z;
					vertices.push_back(v);
				}
			}
			if (!hasTexture)
			{
				if (line[0] == 'f')
				{
					// Create faces and push them as triangles to the tris vector
					// Vertices are fetched from the temporary vertice pool, using the index given in the object file
					// Indexing in the obj file starts from 1, so subtract the index by 1
					int f[3];
					s >> j >> f[0] >> f[1] >> f[2];
					tris.push_back({ vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1] });
				}
			}
			else // Object has Texture
			{
				// Texture and Vertex data is stored as 3 pairs, divided by / symbol.
				// Get this data from the lines accordingly
				if (line[0] == 'f')
				{
					s >> j;
					string tokens[6];
					int tokenCount = -1;

					while (!s.eof())
					{
						char c = s.get(); 
						if (c == ' ' || c == '/')
							tokenCount++;
						else
							tokens[tokenCount].append(1, c);
					}

					tokens[tokenCount].pop_back();

					tris.push_back(
						{
							vertices[stoi(tokens[0]) - 1],
							vertices[stoi(tokens[2]) - 1],
							vertices[stoi(tokens[4]) - 1],
							textures[stoi(tokens[1]) - 1],
							textures[stoi(tokens[3]) - 1],
							textures[stoi(tokens[5]) - 1],
						});
				}
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

	// 0 == Wireframe, 1 == Filled , 2 == Filled with wireframe
	//int drawMode = 0;

private:
	mesh meshCube;
	mesh meshCurrent;
	float rotationTheta = 0.0f;
	mat4x4 matProjection;

	// Represents the camera's position for now
	vec3d camera;
	// Camera's look direction unit vector
	vec3d lookDirection;
	float yaw;

	float* depthBuffer = nullptr;

	ConsoleSprite* spriteTexture1;

	// Input XY-coordinate pairs from the Screen Space, as well as UV-Coordinate pairs, with also the sprite as the texture
	void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
						  int x2, int y2, float u2, float v2, float w2,
						  int x3, int y3, float u3, float v3, float w3,
						  ConsoleSprite *tex)
	{
		// Sort the coordinates from low Y to high Y
		if (y2 < y1)
		{
			swap(y1, y2);
			swap(x1, x2);
			swap(u1, u2);
			swap(v1, v2);
			swap(w1, w2);
		}
		if (y3 < y1)
		{
			swap(y1, y3);
			swap(x1, x3);
			swap(u1, u3);
			swap(v1, v3);
			swap(w1, w3);
		}
		if (y3 < y2)
		{
			swap(y2, y3);
			swap(x2, x3);
			swap(u2, u3);
			swap(v2, v3);
			swap(w2, w3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		float dv1 = v2 - v1;
		float du1 = u2 - u1;
		float dw1 = w2 - w1;


		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		float dv2 = v3 - v1;
		float du2 = u3 - u1;
		float dw2 = w3 - w1;

		float texU;
		float texV;
		float texW;

		float xStepValueA = 0;
		float xStepValueB = 0;

		float uStepValue1 = 0;
		float uStepValue2 = 0;

		float vStepValue1 = 0;
		float vStepValue2 = 0;

		float wStepValue1 = 0;
		float wStepValue2 = 0;

		// If the line between the two points on the triangle is horizontal, dx will be infinity, so test against that
		if (dy1)
			xStepValueA = dx1 / (float)abs(dy1);
		if (dy2)
			xStepValueB = dx2 / (float)abs(dy2);

		if (dy1)
			uStepValue1 = du1 / (float)abs(dy1);
		if (dy1)
			vStepValue1 = dv1 / (float)abs(dy1);
		if (dy1)
			wStepValue1 = dw1 / (float)abs(dy1);
		// Other side as well
		if (dy2)
			uStepValue2 = du2 / (float)abs(dy2);
		if (dy2)
			vStepValue2 = dv2 / (float)abs(dy2);
		if (dy2)
			wStepValue2 = dw2 / (float)abs(dy2);

		// Scanline filling

		// If the line exists, it means we are drawing the top half of the triangle
		if (dy1)
		{
			// Draw until we reach y2, as y2 is either the flat bottom or the middle of the triangle
			for (int i = y1; i <= y2; i++)
			{
				// Get vertex position along the edge
				int ax = x1 + (float)(i - y1) * xStepValueA;
				int bx = x1 + (float)(i - y1) * xStepValueB;

				// Following one line, get the starting value
				float texStartU = u1 + (float)(i - y1) * uStepValue1;
				float texStartV = v1 + (float)(i - y1) * vStepValue1;
				float texStartW = w1 + (float)(i - y1) * wStepValue1;

				// Following the other, get the ending value
				float texEndU = u1 + (float)(i - y1) * uStepValue2;
				float texEndV = v1 + (float)(i - y1) * vStepValue2;
				float texEndW = w1 + (float)(i - y1) * wStepValue2;

				// However, also make sure we are always drawing from a smaller x value before going to larger values
				if (ax > bx)
				{
					swap(ax, bx);
					swap(texStartU, texEndU);
					swap(texStartV, texEndV);
					swap(texStartW, texEndW);
				}

				texU = texStartU;
				texV = texStartV;
				texW = texStartW;

				// tStep is 1 divided by the number of scanline pixels on this line
				float tStep = 1.0f / ((float)(bx - ax));
				// t represents where we are at in the scanline
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					// Linearly interpolate between the starting and ending point
					texU = (1.0f - t) * texStartU + t * texEndU;
					texV = (1.0f - t) * texStartV + t * texEndV;
					texW = (1.0f - t) * texStartW + t * texEndW;

					// Check from Depth Buffer if we have already drawn something in front of this pixel
					if (texW > depthBuffer[i * screenWidth + j])
					{
						// Draw a single pixel
						Draw(j, i, tex->SampleSymbol(texU / texW, texV / texW), tex->SampleColour(texU / texW, texV / texW));
						// Add new depth value to depthBuffer
						depthBuffer[i * screenWidth + j] = texW;
						// Increase t every pixel
						t += tStep;
					}
				}
			}
		}

		// Update the gradients using the new point 3
		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1)
			xStepValueA = dx1 / (float)abs(dy1);
		if (dy2)
			xStepValueB = dx2 / (float)abs(dy2);

		uStepValue1 = 0.0f;
		vStepValue1 = 0.0f;

		if (dy1)
			uStepValue1 = du1 / (float)abs(dy1);
		if (dy1)
			vStepValue1 = dv1 / (float)abs(dy1);
		if (dy1)
			wStepValue1 = dw1 / (float)abs(dy1);

		// Draw the rest of the triangle
		for (int i = y2; i <= y3; i++)
		{
			// Get vertex position along the edge
			int ax = x2 + (float)(i - y2) * xStepValueA;
			int bx = x1 + (float)(i - y1) * xStepValueB;

			// Following one line, get the starting value
			float texStartU = u2 + (float)(i - y2) * uStepValue1;
			float texStartV = v2 + (float)(i - y2) * vStepValue1;
			float texStartW = w2 + (float)(i - y2) * wStepValue1;

			// Following the other, get the ending value
			float texEndU = u1 + (float)(i - y1) * uStepValue2;
			float texEndV = v1 + (float)(i - y1) * vStepValue2;
			float texEndW = w1 + (float)(i - y1) * wStepValue2;

			// However, also make sure we are always drawing from a smaller x value before going to larger values
			if (ax > bx)
			{
				swap(ax, bx);
				swap(texStartU, texEndU);
				swap(texStartV, texEndV);
				swap(texStartW, texEndW);
			}

			texU = texStartU;
			texV = texStartV;
			texW = texStartW;

			// tStep is 1 divided by the number of scanline pixels on this line
			float tStep = 1.0f / ((float)(bx - ax));
			// t represents where we are at in the scanline
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				// Linearly interpolate between the starting and ending point
				texU = (1.0f - t) * texStartU + t * texEndU;
				texV = (1.0f - t) * texStartV + t * texEndV;
				texW = (1.0f - t) * texStartW + t * texEndW;
				// Check from Depth Buffer if we have already drawn something in front of this pixel
				if (texW > depthBuffer[i * screenWidth + j])
				{
					// Draw a single pixel
					Draw(j, i, tex->SampleSymbol(texU / texW, texV / texW), tex->SampleColour(texU / texW, texV / texW));
					// Add new depth value to depthBuffer
					depthBuffer[i * screenWidth + j] = texW;
					// Increase t every pixel
					t += tStep;
					
				}
			}
		}

	}

	// Add two vectors together
	vec3d VectorAdd(vec3d &v1, vec3d &v2)
	{
		return
		{
			v1.x + v2.x, v1.y + v2.y, v1.z + v2.z
		};
	}

	// Subtract two vectors
	vec3d VectorSubtract(vec3d &v1, vec3d &v2)
	{
		return
		{
			v1.x - v2.x, v1.y - v2.y, v1.z - v2.z
		};
	}

	// Scale a vector
	vec3d VectorMultiply(vec3d &v1, float s)
	{
		return
		{
			v1.x * s, v1.y * s, v1.z * s
		};
	}

	// Divide a vector
	vec3d VectorDivide(vec3d &v1, float s)
	{
		return
		{
			v1.x / s, v1.y / s, v1.z / s
		};
	}

	// Vector dot product
	float VectorDotProduct(vec3d &v1, vec3d &v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	float VectorLength(vec3d& v)
	{
		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	// Normalize a vector to create a unit vector
	vec3d VectorNormalize(vec3d &v)
	{
		float len = VectorLength(v);
		return
		{
			v.x / len, v.y / len, v.z / len
		};
	}

	// Vector cross product
	vec3d VectorCrossProduct(vec3d& v1, vec3d& v2)
	{
		vec3d v;
		v.x = v1.y * v2.z - v1.z * v2.y;
		v.y = v1.z * v2.x - v1.x * v2.z;
		v.z = v1.x * v2.y - v1.y * v2.x;
		return v;
	}



	// Multiply vector with a matrix
	vec3d VectorMultiplyMatrix(vec3d &input, mat4x4& matrix)
	{
		vec3d output;
		output.x = input.x * matrix.m[0][0] + input.y * matrix.m[1][0] + input.z * matrix.m[2][0] + input.w * matrix.m[3][0];
		output.y = input.x * matrix.m[0][1] + input.y * matrix.m[1][1] + input.z * matrix.m[2][1] + input.w * matrix.m[3][1];
		output.z = input.x * matrix.m[0][2] + input.y * matrix.m[1][2] + input.z * matrix.m[2][2] + input.w * matrix.m[3][2];
		output.w = input.x * matrix.m[0][3] + input.y * matrix.m[1][3] + input.z * matrix.m[2][3] + input.w * matrix.m[3][3];
		return output;
	}

	// Return the point where line intersects with the plane, if it does
	vec3d VectorIntersectPlane(vec3d &planePoint, vec3d &planeNormal, vec3d &lineStart, vec3d &lineEnd, float &t)
	{
		planeNormal = VectorNormalize(planeNormal);
		float planeD = -VectorDotProduct(planeNormal, planePoint);
		float ad = VectorDotProduct(lineStart, planeNormal);
		float bd = VectorDotProduct(lineEnd, planeNormal);
		// The distance along the line between the two points where the intersection happened
		t = (-planeD - ad) / (bd - ad);

		vec3d lineStartToEnd = VectorSubtract(lineEnd, lineStart);
		vec3d lineToIntersect = VectorMultiply(lineStartToEnd, t);

		return VectorAdd(lineStart, lineToIntersect);
	}

	// The Clipping Function. Return integer is how many triangles are returned by the function, from 0-2
	int TriangleClipAgainstPlane(vec3d planePoint, vec3d planeNormal, triangle &inTriangle, triangle &outTriangle1, triangle &outTriangle2)
	{
		// Normalize plane to make sure it is normalized
		planeNormal = VectorNormalize(planeNormal);

		// Check whether points are inside or outside the plane
		// Returns a signed shortest distance from the point to the plane.
		auto dist = [&](vec3d& point)
		{
			vec3d normal = VectorNormalize(point);
			return (planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z - VectorDotProduct(planeNormal, planePoint));
		};

		// Temporary storage arrays for classifying points on either side of the plane
		// Negative distance sign means outside of the plane, positive means inside of the plane
		vec3d* insidePoints[3];
		vec3d* outsidePoints[3];
		int insidePointCount = 0;
		int outsidePointCount = 0;

		// Also track inside and outside points for Texture Coordinates
		vec2d* insideTexPoints[3];
		vec2d* outsideTexPoints[3];
		int insideTexPointCount = 0;
		int outsideTexPointCount = 0;

		// Signed distance of each point in triangle to plane
		float d0 = dist(inTriangle.p[0]);
		float d1 = dist(inTriangle.p[1]);
		float d2 = dist(inTriangle.p[2]);

		// Determine, based on the sign, if the points are inside or outside
		if (d0 >= 0)
		{
			insidePoints[insidePointCount++] = &inTriangle.p[0];
			insideTexPoints[insideTexPointCount++] = &inTriangle.t[0];
		}
		else
		{
			outsidePoints[outsidePointCount++] = &inTriangle.p[0];
			outsideTexPoints[outsideTexPointCount++] = &inTriangle.t[0];
		}

		if (d1 >= 0)
		{
			insidePoints[insidePointCount++] = &inTriangle.p[1];
			insideTexPoints[insideTexPointCount++] = &inTriangle.t[1];
		}
		else
		{
			outsidePoints[outsidePointCount++] = &inTriangle.p[1];
			outsideTexPoints[outsideTexPointCount++] = &inTriangle.t[1];
		}

		if (d2 >= 0)
		{
			insidePoints[insidePointCount++] = &inTriangle.p[2];
			insideTexPoints[insideTexPointCount++] = &inTriangle.t[2];
		}
		else
		{
			outsidePoints[outsidePointCount++] = &inTriangle.p[2];
			outsideTexPoints[outsideTexPointCount++] = &inTriangle.t[2];
		}

		// Classify triangle points and break the input triangle into 
		// smaller output triangles if necessary, with 4 possible outcomes
		if (insidePointCount == 0)
		{
			// All points outside the plane, 
			// The whole triangle will be clipped
			return 0;
		}

		if (insidePointCount == 3)
		{
			// All points are inside the plane,
			// The whole triangle is passed through
			outTriangle1 = inTriangle;

			return 1; // Return the original triangle, so 1
		}

		if (insidePointCount == 1 && outsidePointCount == 2)
		{
			// Clip the triangle, two points are outside
			// This means the triangle will be turned into a smaller triangle

			// Copy appearance
			outTriangle1.color = inTriangle.color;
			outTriangle1.symbol = inTriangle.symbol;

			// Keep the point that is inside
			outTriangle1.p[0] = *insidePoints[0];
			outTriangle1.t[0] = *insideTexPoints[0];

			// Two new points are at points where the triangle intersects with the plane
			float t;
			outTriangle1.p[1] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0], t);
			// Calculate new texture coordinates using the two points
			outTriangle1.t[1].u = t * (outsideTexPoints[0]->u - insideTexPoints[0]->u) + insideTexPoints[0]->u;
			outTriangle1.t[1].v = t * (outsideTexPoints[0]->v - insideTexPoints[0]->v) + insideTexPoints[0]->v;
			outTriangle1.t[1].w = t * (outsideTexPoints[0]->w - insideTexPoints[0]->w) + insideTexPoints[0]->w;

			outTriangle1.p[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[1], t);
			outTriangle1.t[2].u = t * (outsideTexPoints[1]->u - insideTexPoints[0]->u) + insideTexPoints[0]->u;
			outTriangle1.t[2].v = t * (outsideTexPoints[1]->v - insideTexPoints[0]->v) + insideTexPoints[0]->v;
			outTriangle1.t[2].w = t * (outsideTexPoints[1]->w - insideTexPoints[0]->w) + insideTexPoints[0]->w;

			return 1; // Return the new triangle that was formed
		}

		if (insidePointCount == 2 && outsidePointCount == 1)
		{
			// Clip the triangle, one point is outside
			// This means the triangle will be turned into a quad, using two smaller triangles

			// Copy appearance
			outTriangle1.color = inTriangle.color;
			outTriangle1.symbol = inTriangle.symbol;

			outTriangle2.color = inTriangle.color;
			outTriangle2.symbol = inTriangle.symbol;

			// Set the points using the inside points, and the intersecting new points
			float t;
			// The first has the two inside points, and new intersecting point
			outTriangle1.p[0] = *insidePoints[0];
			outTriangle1.p[1] = *insidePoints[1];
			outTriangle1.t[0] = *insideTexPoints[0];
			outTriangle1.t[1] = *insideTexPoints[1];

			outTriangle1.p[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0], t);
			outTriangle1.t[2].u = t * (outsideTexPoints[0]->u - insideTexPoints[0]->u) + insideTexPoints[0]->u;
			outTriangle1.t[2].v = t * (outsideTexPoints[0]->v - insideTexPoints[0]->v) + insideTexPoints[0]->v;
			outTriangle1.t[2].w = t * (outsideTexPoints[0]->w - insideTexPoints[0]->w) + insideTexPoints[0]->w;

			// The second has one of the inside points, an intersecting point, and the new point created for outTriangle1
			outTriangle2.p[0] = *insidePoints[1];
			outTriangle2.t[0] = *insideTexPoints[1];

			outTriangle2.p[1] = outTriangle1.p[2];
			outTriangle2.t[1] = outTriangle1.t[2];

			outTriangle2.p[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[1], *outsidePoints[0], t);
			outTriangle2.t[2].u = t * (outsideTexPoints[0]->u - insideTexPoints[1]->u) + insideTexPoints[1]->u;
			outTriangle2.t[2].v = t * (outsideTexPoints[0]->v - insideTexPoints[1]->v) + insideTexPoints[1]->v;
			outTriangle2.t[2].w = t * (outsideTexPoints[0]->w - insideTexPoints[1]->w) + insideTexPoints[1]->w;

			return 2; // Return the 2 new triangles that were formed
		}
	}

	// Create an identity matrix (Main diagonal ones, zeros elsewhere)
	mat4x4 MatrixMakeIdentity()
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	// Create an X axis rotation matrix
	mat4x4 MatrixMakeRotationX(float rad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = cosf(rad);
		matrix.m[1][2] = sinf(rad);
		matrix.m[2][1] = -sinf(rad);
		matrix.m[2][2] = cosf(rad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	// Create an Y axis rotation matrix
	mat4x4 MatrixMakeRotationY(float rad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cosf(rad);
		matrix.m[0][2] = sinf(rad);
		matrix.m[2][0] = -sinf(rad);
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = cosf(rad);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	// Create an Z axis rotation matrix
	mat4x4 MatrixMakeRotationZ(float rad)
	{
		mat4x4 matrix;
		matrix.m[0][0] = cosf(rad);
		matrix.m[0][1] = sinf(rad);
		matrix.m[1][0] = -sinf(rad);
		matrix.m[1][1] = cosf(rad);
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

	// Create a translation matrix
	mat4x4 MatrixMakeTranslation(float x, float y, float z)
	{
		mat4x4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = 1.0f;
		matrix.m[2][2] = 1.0f;
		matrix.m[3][3] = 1.0f;
		matrix.m[3][0] = x;
		matrix.m[3][1] = y;
		matrix.m[3][2] = z;
		return matrix;
	}

	// Create a projection matrix
	mat4x4 MatrixMakeProjection(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
	{
		float fovRad = 1.0f / tanf(fovDegrees * 0.5f / 180.0f * M_PI);
		mat4x4 matrix;
		matrix.m[0][0] = aspectRatio * fovRad;
		matrix.m[1][1] = fovRad;
		matrix.m[2][2] = farPlane / (farPlane - nearPlane);
		matrix.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
		matrix.m[2][3] = 1.0f;
		matrix.m[3][3] = 0.0f;
		return matrix;
	}

	// Multiply matrix with another matrix
	mat4x4 MatrixMultiplyMatrix(mat4x4 &m1, mat4x4 &m2)
	{
		mat4x4 matrix;
		for (int c = 0; c < 4; c++)
			for (int r = 0; r < 4; r++)
				matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];

		return matrix;
	}

	mat4x4 MatrixPointAt(vec3d &pos, vec3d &target, vec3d &up)
	{
		// Calculate a new Forward direction
		vec3d newForward = VectorSubtract(target, pos);
		newForward = VectorNormalize(newForward);

		// Calculate a new Up direction
		vec3d a = VectorMultiply(newForward, VectorDotProduct(up, newForward));
		vec3d newUp = VectorSubtract(up, a);
		newUp = VectorNormalize(newUp);

		// Calculate a new Right direction, which is just cross product of the two other vectors
		vec3d newRight = VectorCrossProduct(newUp, newForward);

		// Construct a Dimensioning and Translation Matrix (Point At Matrix)
		mat4x4 matrix;
		matrix.m[0][0] = newRight.x;
		matrix.m[0][1] = newRight.y;
		matrix.m[0][2] = newRight.z;
		matrix.m[0][3] = 0.0f;


		matrix.m[1][0] = newUp.x;
		matrix.m[1][1] = newUp.y;
		matrix.m[1][2] = newUp.z;
		matrix.m[1][3] = 0.0f;


		matrix.m[2][0] = newForward.x;
		matrix.m[2][1] = newForward.y;
		matrix.m[2][2] = newForward.z;
		matrix.m[2][3] = 0.0f;


		matrix.m[3][0] = pos.x;
		matrix.m[3][1] = pos.y;
		matrix.m[3][2] = pos.z;
		matrix.m[3][3] = 0.0f;

		return matrix;
	}

	// Invert Matrix, works only for Rotation / Translation Matrices
	mat4x4 MatrixQuickInverse(mat4x4 &m)
	{
		mat4x4 matrix;
		matrix.m[0][0] = m.m[0][0]; 
		matrix.m[0][1] = m.m[1][0]; 
		matrix.m[0][2] = m.m[2][0]; 
		matrix.m[0][3] = 0.0f;

		matrix.m[1][0] = m.m[0][1]; 
		matrix.m[1][1] = m.m[1][1]; 
		matrix.m[1][2] = m.m[2][1]; 
		matrix.m[1][3] = 0.0f;

		matrix.m[2][0] = m.m[0][2]; 
		matrix.m[2][1] = m.m[1][2]; 
		matrix.m[2][2] = m.m[2][2]; 
		matrix.m[2][3] = 0.0f;

		matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;

		return matrix;
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
