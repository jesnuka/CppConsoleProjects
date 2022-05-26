#include "ConsoleEngine.h"
#include "Racing.h"
using namespace std;

bool Racing::OnUserCreate()
{
	return true;
}

bool Racing::OnUserUpdate(float elapsedTime)
{
	// Get input

	if (keys[VK_UP].isHeld)
		distance += 10.0f * elapsedTime;

	// Empty screen
	Fill(0, 0, screenWidth, screenHeight, L' ', 0);

	// Draw the track
	for (int j = 0; j < (screenHeight / 2); j++)
	{
		for (int i = 0; i < screenWidth; i++)
		{
			// Assume screenWidth is a normalized space, with 0 on the left and 1 on the right

			// Perspective percentage, with smaller y values it goes towards 0 and towards 1 with larger y values
			float perspective = (float)j / (screenWidth / 2);

			float middlePoint = 0.5f;
			// Road width uses perspective value to become less wide towards the middle of the screen
			// However, it also has minimum and maximum values
			float roadWidth = 0.1f + perspective * 0.8f;
			float roadEdgeWidth = roadWidth * 0.15f;

			// Half the road width to allow for symmetrical road placement
			roadWidth *= 0.5f;

			// Draw grass colour using a modified sin wave, to get a perspective effect
			// Sin function is between {-1, 1}, where negative values are given darker coloring
			short grassColour = sinf(20.0f * powf(1.0f - perspective, 3) + distance * 0.1f) > 0.0f ? FG_GREEN : FG_DARK_GREEN;

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
				Draw(i, bottomRow, PIXEL_FULL, FG_RED);
			// Draw in road
			if (i >= roadEdgeLeft && i < roadEdgeRight)
				Draw(i, bottomRow, PIXEL_FULL, FG_GREY);
			// Draw in right road edge
			if (i >= roadEdgeRight && i < grassRight)
				Draw(i, bottomRow, PIXEL_FULL, FG_RED);
			// Draw in grass
			if (i >= grassRight && i < screenWidth)
				Draw(i, bottomRow, PIXEL_FULL, grassColour);

		}
	}

	// Draw in the player car
	int carPosition = (screenWidth / 2) + ((int)(screenWidth * carPositionFloat) / 2) - carSpriteWidth;

	DrawString(carPosition, carPositionheight + 0, L"     ###     ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 1, L"  ||#####||  ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 2, L"    #####    ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 3, L"    #####    ", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 4, L"|| ####### ||", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 5, L"||#########||", FG_WHITE, true);
	DrawString(carPosition, carPositionheight + 6, L"||  #####  ||", FG_WHITE, true);

	return true;
}
