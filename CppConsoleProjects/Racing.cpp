#include "ConsoleEngine.h"
#include "Racing.h"
using namespace std;

bool Racing::OnUserCreate()
{
	// Create the racing track

	// The start / finish line
	track.push_back(make_pair(0.0f, 10.0f));

	track.push_back(make_pair(0.0f, 200.0f)); // Straight for 200 units
	track.push_back(make_pair(1.0f, 200.0f)); // Sharp bend for 200 units
	track.push_back(make_pair(0.0f, 100.0f)); // etc.
	track.push_back(make_pair(-1.0f, 300.0f)); 
	track.push_back(make_pair(0.0f, 200.0f)); 
	track.push_back(make_pair(-1.0f, 400.0f)); 
	track.push_back(make_pair(1.0f, 200.0f)); 
	track.push_back(make_pair(-0.5f, 200.0f)); 
	track.push_back(make_pair(0.5f, 200.0f)); 

	return true;
}

bool Racing::OnUserUpdate(float elapsedTime)
{
	// Get input

	if (keys[VK_UP].isHeld)
	{
		if (carVelocity < carVelocityMax)
			carVelocity += carAcceleration * elapsedTime;
		else
			carVelocity = carVelocityMax;
	}
	else if(!keys[VK_UP].isHeld && carVelocity > 0.0f)
	{
		if ((carVelocity - ((carAcceleration * 2) * elapsedTime)) > 0.0f)
			carVelocity -= (carAcceleration * 2) * elapsedTime;
		else
			carVelocity = 0.0f;
	}

	if (keys[VK_LEFT].isHeld)
	{
		carCurvature -= 0.7f * elapsedTime;
	}
	if (keys[VK_RIGHT].isHeld)
	{
		carCurvature += 0.7f * elapsedTime;
	}

	// Check if player is too far off the perfect curvature, meaning they are not in the middle of the track, and slow them down
	if (fabs(trackPerfectCurvature - carCurvature) >= 0.7f)
	{
		carVelocity -= (carAcceleration * 2) * elapsedTime;
	}

	if (carVelocity > 0.0f)
	{
		distance += carVelocity * elapsedTime;


		// Get the current location on the track, which section we are in
		float trackDistance = 0;
		trackSection = 0;

		for (int i = 0; i < track.size(); i++)
		{
			if (trackDistance > distance)
			{
				trackSection = i - 1;
				break;
			}
			else
				trackDistance += track[i].second;

		}

		// Get the curvature of the current track section for displaying it
		float trackCurvature = track[trackSection].first;

		float curvatureDifference = (trackCurvature - currentCurvature) * elapsedTime * (carVelocity / carVelocityMax);
		currentCurvature += curvatureDifference;

		// Calculate the target curvature the player should aim for
		trackPerfectCurvature += currentCurvature * elapsedTime * (carVelocity / carVelocityMax);
	}

	// Empty screen
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);

	// Draw the track
	for (int j = 0; j < (screenHeight / 2); j++)
	{
		for (int i = 0; i < screenWidth; i++)
		{
			// Assume screenWidth is a normalized space, with 0 on the left and 1 on the right

			// Perspective percentage, with smaller y values it goes towards 0 and towards 1 with larger y values
			float perspective = (float)j / (screenWidth / 2.5);

			float middlePoint = 0.5f + currentCurvature * powf(1.0f - perspective, 3);
			// Road width uses perspective value to become less wide towards the middle of the screen
			// However, it also has minimum and maximum values
			float roadWidth = 0.1f + perspective * 0.8f;
			float roadEdgeWidth = roadWidth * 0.15f;

			// Half the road width to allow for symmetrical road placement
			roadWidth *= 0.5f;

			// Draw grass colour using a modified sin wave, to get a perspective effect
			// Sin function is between {-1, 1}, where negative values are given darker coloring
			short grassColour = sinf(20.0f * powf(1.0f - perspective, 3) + distance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;
			// Use the same method for drawing the road edge coloring
			short roadEdgeColour = sinf(80.0f * powf(1.0f - perspective, 2) + distance) > 0.0f ? FG_WHITE : FG_RED;

			// Define the areas where grass and road edges are located
			int grassLeft = (middlePoint - roadWidth - roadEdgeWidth) * screenWidth;
			int roadEdgeLeft = (middlePoint - roadWidth) * screenWidth;
			int grassRight= (middlePoint + roadWidth + roadEdgeWidth) * screenWidth;
			int roadEdgeRight = (middlePoint + roadWidth) * screenWidth;


			// To draw to the bottom half of the screen, create a new variable
			int bottomRow = (screenHeight / 2) + j;

			// Draw in grass
			if (i >= 0 && i < grassLeft)
				Draw(i, bottomRow, PIXEL_FULL, grassColour);
			// Draw in left road edge
			if (i >= grassLeft && i < roadEdgeLeft)
				Draw(i, bottomRow, PIXEL_FULL, roadEdgeColour);
			// Draw in road
			if (i >= roadEdgeLeft && i < roadEdgeRight)
				Draw(i, bottomRow, PIXEL_FULL, FG_GREY);
			// Draw in right road edge
			if (i >= roadEdgeRight && i < grassRight)
				Draw(i, bottomRow, PIXEL_FULL, roadEdgeColour);
			// Draw in grass
			if (i >= grassRight && i < screenWidth)
				Draw(i, bottomRow, PIXEL_FULL, grassColour);

		}
	}

	// Draw in the player car
	carPositionFloat = carCurvature - trackPerfectCurvature;

	int carPosition = (screenWidth / 2) + ((int)(screenWidth * carPositionFloat) / 2) - carSpriteWidth;

	DrawString(carPosition, carPositionheight + 0, L"     ###     ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 1, L"  ||#####||  ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 2, L"    #####    ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 3, L"    #####    ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 4, L"|| ####### ||", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 5, L"||#########||", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 6, L"||  #####  ||", FG_WHITE, true);

	// Draw Player Stats
	DrawString(0,0, L"DISTANCE: " + to_wstring(distance));
	DrawString(0,1, L"TRACK CURVATURE: " + to_wstring(trackPerfectCurvature));
	DrawString(0,2, L"CAR CURVATURE: " + to_wstring(carCurvature));
	DrawString(0,3, L"SPEED: " + to_wstring(carVelocity));

	return true;
}
