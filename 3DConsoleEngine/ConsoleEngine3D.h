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
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct triangle
{
	vec3d p[3];

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

	// 0 == Wireframe, 1 == Filled , 2 == Filled with wireframe
	int drawMode = 2;

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
	vec3d VectorIntersectPlane(vec3d &planePoint, vec3d &planeNormal, vec3d &lineStart, vec3d &lineEnd)
	{
		planeNormal = VectorNormalize(planeNormal);
		float planeD = -VectorDotProduct(planeNormal, planePoint);
		float ad = VectorDotProduct(lineStart, planeNormal);
		float bd = VectorDotProduct(lineEnd, planeNormal);
		float t = (-planeD - ad) / (bd - ad);

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

		// Signed distance of each point in triangle to plane
		float d0 = dist(inTriangle.p[0]);
		float d1 = dist(inTriangle.p[1]);
		float d2 = dist(inTriangle.p[2]);

		// Determine, based on the sign, if the points are inside or outside
		if (d0 >= 0) 
			insidePoints[insidePointCount++] = &inTriangle.p[0];
		else
			outsidePoints[outsidePointCount++] = &inTriangle.p[0];

		if (d1 >= 0)
			insidePoints[insidePointCount++] = &inTriangle.p[1];
		else
			outsidePoints[outsidePointCount++] = &inTriangle.p[1];

		if (d2 >= 0)
			insidePoints[insidePointCount++] = &inTriangle.p[2];
		else
			outsidePoints[outsidePointCount++] = &inTriangle.p[2];

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

			// Two new points are at points where the triangle intersects with the plane
			outTriangle1.p[1] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0]);
			outTriangle1.p[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[1]);

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

			// The first has the two inside points, and new intersecting point
			outTriangle1.p[0] = *insidePoints[0];
			outTriangle1.p[1] = *insidePoints[1];
			outTriangle1.p[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[0], *outsidePoints[0]);

			// The second has one of the inside points, an intersecting point, and the new point created for outTriangle1
			outTriangle2.p[0] = *insidePoints[1];
			outTriangle2.p[1] = outTriangle1.p[2];
			outTriangle2.p[2] = VectorIntersectPlane(planePoint, planeNormal, *insidePoints[1], *outsidePoints[0]);

			return 2; // Return the new triangle that was formed
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
